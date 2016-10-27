#include "terminal_p.h"
#include <QtEndian>
#include <QJsonDocument>
#include <QTimer>
using namespace QBackgroundProcess;

TerminalPrivate::TerminalPrivate(QLocalSocket *socket, QObject *parent) :
	QObject(parent),
	socket(socket),
	status(),
	autoDelete(false),
	isLoading(true),
	disconnecting(false)
{
	socket->setParent(this);

	connect(socket, &QLocalSocket::disconnected,
			this, &TerminalPrivate::disconnected);
	connect(socket, QOverload<QLocalSocket::LocalSocketError>::of(&QLocalSocket::error),
			this, &TerminalPrivate::error);
	connect(socket, &QLocalSocket::readyRead,
			this, &TerminalPrivate::readyRead);
	connect(socket, &QLocalSocket::bytesWritten,
			this, &TerminalPrivate::writeReady);
}

void TerminalPrivate::beginSoftDisconnect()
{
	this->disconnecting = true;
	if(this->socket->bytesToWrite() > 0) {
		QTimer::singleShot(3000, this, [=](){
			if(this->socket->state() == QLocalSocket::ConnectedState)
				this->socket->disconnectFromServer();
		});
		this->socket->flush();
	} else
		this->socket->disconnectFromServer();
}

void TerminalPrivate::disconnected()
{
	if(this->isLoading) {
		emit statusLoadComplete(this, false);
		this->socket->close();
	} else if(this->autoDelete && this->parent())
		this->parent()->deleteLater();
}

void TerminalPrivate::error(QLocalSocket::LocalSocketError socketError)
{
	if(socketError != QLocalSocket::PeerClosedError) {
		if(this->isLoading) {
			qWarning() << "Terminal closed due to connection error while loading terminal status:"
					   << qUtf8Printable(this->socket->errorString());
		}
		this->socket->disconnectFromServer();
	}
}

void TerminalPrivate::readyRead()
{
	if(this->isLoading) {
		if(this->socket->bytesAvailable() < sizeof(quint32))
			return;
		auto size = qFromBigEndian<quint32>(this->socket->peek(sizeof(quint32)));
		if(this->socket->bytesAvailable() >= (qint64)(size + sizeof(quint32))) {
			this->socket->read(sizeof(quint32));
			auto doc = QJsonDocument::fromBinaryData(this->socket->read(size));

			if(doc.isNull()) {
				qWarning() << "Invalid Terminal status received. Data is corrupted. Terminal will be disconnected";
				this->socket->disconnectFromServer();
				return;
			} else {
				this->status = doc.object();
				this->isLoading = false;
				emit statusLoadComplete(this, true);
			}
		}
	}
}

void TerminalPrivate::writeReady()
{
	if(this->disconnecting && this->socket->bytesToWrite() == 0) {
		QTimer::singleShot(500, this, [=](){
			if(this->socket->state() == QLocalSocket::ConnectedState)
				this->socket->disconnectFromServer();
		});
	}
}

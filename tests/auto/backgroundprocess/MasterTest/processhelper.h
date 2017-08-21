#ifndef PROCESSHELPER_H
#define PROCESSHELPER_H

#include <QObject>
#include <QProcess>

class ProcessHelper : public QObject
{
	Q_OBJECT

public:
	static const char Stamp;

	explicit ProcessHelper(QObject *parent = nullptr);

	void setExitCode(int code);
	void start(const QByteArrayList &commands, bool logpath = false);

	void waitForFinished();
	void verifyLog(const QByteArrayList &log, bool isError = false);
	static void waitForFinished(const QList<ProcessHelper*> &helpers);

	static void clearLog();
	static void verifyMasterLog(const QByteArrayList &log);

private Q_SLOTS:
	void errorOccurred(QProcess::ProcessError error);
	void finished(int exitCode, QProcess::ExitStatus exitStatus);

private:
	QProcess *process;
	int exitCode;

	static void testLog(const QByteArrayList &log, QIODevice *device);
};

#endif // PROCESSHELPER_H

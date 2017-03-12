TARGET = QtBackgroundProcess

QT = core network

include(../3rdparty/QCtrlSignals/qctrlsignals.pri)

HEADERS += \
	qbackgroundprocess_global.h \
	app.h \
	globalterminal.h \
	terminal.h \
	app_p.h \
	masterconnecter_p.h \
	terminal_p.h

SOURCES += \
	app_p.cpp \
	app.cpp \
	globalterminal.cpp \
	masterconnecter.cpp \
	terminal_p.cpp \
	terminal.cpp

load(qt_module)

win32 {
	QMAKE_TARGET_PRODUCT = "QtBackgroundProcess"
	QMAKE_TARGET_COMPANY = "Skycoder42"
	QMAKE_TARGET_COPYRIGHT = "Felix Barz"

	LIBS += -lUser32
} else:mac {
	QMAKE_TARGET_BUNDLE_PREFIX = "de.skycoder42."
}

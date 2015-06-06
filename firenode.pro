TEMPLATE = app
CONFIG += qt debug console
TARGET = firenode
QT += core network serialport
DEFINES += QT_LARGEFILE_SUPPORT QT_DLL QT_NETWORK_LIB

SOURCES +=  src/networking.cpp \
            src/main.cpp \
            src/unpacker.cpp \
            src/serial.cpp

HEADERS +=  src/version.h \
            src/networking.h \
            src/portability.h \
            src/unpacker.h \
            src/serial.h \
            src/color_correct.h

win32 {
    LIBS += -L"../zeromq-4.1.0/bin" -lzmq
    INCLUDEPATH += "../zeromq-4.1.0/include"
}

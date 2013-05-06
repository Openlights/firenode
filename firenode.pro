TEMPLATE = app
CONFIG += qt debug console
TARGET = firenode
QT += core network
DEFINES += QT_LARGEFILE_SUPPORT QT_DLL QT_NETWORK_LIB

SOURCES += src/networking.cpp src/main.cpp src/unpacker.cpp src/usb.cpp

HEADERS += src/version.h src/networking.h src/portability.h src/unpacker.h src/usb.h

LIBS += -lusb-1.0

INCLUDEPATH += /usr/include/libusb-1.0/
DEPENDPATH += /usr/local/lib/
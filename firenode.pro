TEMPLATE = app
CONFIG += qt debug console serialport
TARGET = firenode
QT += core network
DEFINES += QT_LARGEFILE_SUPPORT QT_DLL QT_NETWORK_LIB

SOURCES += src/networking.cpp src/main.cpp src/serial.cpp src/unpacker.cpp

HEADERS += src/version.h src/networking.h src/portability.h src/serial.h src/unpacker.h

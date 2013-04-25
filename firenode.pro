TEMPLATE = app
CONFIG += qt debug console serial
TARGET = firenode
QT += core network
DEFINES += QT_LARGEFILE_SUPPORT QT_DLL QT_NETWORK_LIB

SOURCES += src/networking.cpp src/main.cpp

HEADERS += src/version.h src/networking.h src/portability.h src/msgpack.hpp

QT += core network
QT -= gui

CONFIG += c++11

TARGET = rso_election
CONFIG += console
CONFIG -= app_bundle

TEMPLATE = app

SOURCES += main.cpp \
    sserver.cpp \
    bullyalgorithm.cpp \
    listenmodule.cpp \
    servserverlisten.cpp \
    configuration.cpp \
    ../communication/cipheradapter.cpp \
    ../communication/rsaencrypter.cpp \
    ../communication/tcpserver.cpp

HEADERS += \
    sserver.h \
    bullyalgorithm.h \
    listenmodule.h \
    servserverlisten.h \
    frametype.h \
    configuration.h \
    ../communication/cipheradapter.h \
    ../communication/rsaencrypter.h \
    ../communication/tcpserver.h

DISTFILES += \
    settings.ini
LIBS += -lcrypto -ldl -lssl

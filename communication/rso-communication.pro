#-------------------------------------------------
#
# Project created by QtCreator 2016-05-18T15:53:05
#
#-------------------------------------------------

QT       += core network

QT       -= gui

TARGET = rso-communication
CONFIG   += console
CONFIG   -= app_bundle

CONFIG += c++11
QMAKE_CXXFLAGS += -std=c++0x

TEMPLATE = app


SOURCES += \
    tcpserver.cpp \
    tester.cpp \
    comm-test.cpp \
    cipheradapter.cpp \
    rsaencrypter.cpp

HEADERS += \
    tcpserver.h \
    tester.h \
    cipheradapter.h \
    rsaencrypter.h

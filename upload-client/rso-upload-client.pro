#-------------------------------------------------
#
# Project created by QtCreator 2016-05-14T12:45:15
#
#-------------------------------------------------

QT       += core network

QT       -= gui

TARGET = rso-upload-client
CONFIG   += console
CONFIG   -= app_bundle

CONFIG += c++11
QMAKE_CXXFLAGS += -std=c++0x

TEMPLATE = app


SOURCES += main.cpp \
    client.cpp

HEADERS += \
    client.h \
    server.h

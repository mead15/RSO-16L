QT += core network

TARGET = ServerDaemon
#CONFIG += console
#CONFIG -= app_bundle

TEMPLATE = app


SOURCES += \
    main.cpp \
    settings.cpp \
    securefileuploader.cpp

HEADERS += \
    settings.h \
    securefileuploader.h

INCLUDEPATH += ../QSsh-master/src/libs/ssh
include(../QSsh-master/qssh.pri)
LIBS += -L../qssh_compiled -lQSsh -L../qssh_compiled -lBotan

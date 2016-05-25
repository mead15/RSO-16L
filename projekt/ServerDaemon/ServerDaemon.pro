QT += core network

TARGET = ServerDaemon
#CONFIG += console
#CONFIG -= app_bundle

TEMPLATE = app


SOURCES += \
    main.cpp \
    settings.cpp \
    filebooster.cpp \
    callbacks/fileboostcallback.cpp \
    remote/remoteprocess.cpp \
    boostmanager.cpp

HEADERS += \
    settings.h \
    filebooster.h \
    callbacks/fileboostcallback.h \
    remote/argumentscollector.h \
    remote/remoteprocess.h \
    boostmanager.h

INCLUDEPATH += ../QSsh-master/src/libs/ssh
include(../QSsh-master/qssh.pri)
LIBS += -L../qssh_compiled -lQSsh -L../qssh_compiled -lBotan

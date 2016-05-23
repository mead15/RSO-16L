#ifndef LISTENMODULE_H
#define LISTENMODULE_H
#include <QObject>
#include <QSettings>
#include <QStringList>
#include <QString>
#include <QTcpSocket>
#include <iostream>
#include <list>
#include "../communication/cipheradapter.h"
#include "../communication/tcpserver.h"
#include "sserver.h"
#include "configuration.h"


class ListenModule : public QObject
{
    Q_OBJECT
public:
    //explicit ListenModule(QObject *parent = 0);
    ListenModule(const QString& configFileName, TcpServer* server);
    ListenModule(const QString& configFileName, CipherAdapter* server);
    virtual ~ListenModule();
    TcpServer* server;
    CipherAdapter* cipher;
    void loadSettings(const QString& configFileName);
    Configuration config;

protected:
    virtual void onReceive(QStringList what) { }
    virtual void onFailedToSend(QString err) { }


signals:

public slots:
    void log(QString what);
    void error(QString err);
    void frame(QTcpSocket* socket, QStringList what);
};

#endif // LISTENMODULE_H

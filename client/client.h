#ifndef CLIENT_H
#define CLIENT_H

#include <QObject>
#include <QTcpSocket>
#include <QStringList>
#include <QVector>
#include <QSettings>
#include "server.h"
#include <iostream>
#include <QFile>

enum exitCode{
    OK,
    NO_SERVERS,
};

class Client : public QObject{
    Q_OBJECT
public:
    Client();
    QTcpSocket socket;
    QSettings* settings;
    void setServers(QVector<Server>* servers) { servers_ = servers; }
    exitCode send(QStringList what);
private:
    QVector<Server>* servers_;
public slots:
    void receive();
    void handle(QStringList what);
};

#endif // CLIENT_H

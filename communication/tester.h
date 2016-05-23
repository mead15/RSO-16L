#ifndef TESTER_H
#define TESTER_H

#include <QObject>
#include <QStringList>
#include <QString>
#include <QTcpSocket>
#include <iostream>
#include "tcpserver.h"
#include "cipheradapter.h"

class tester : public QObject
{
    Q_OBJECT
public:
    tester(TcpServer* server);
    tester(CipherAdapter* server);
    TcpServer* server;
    CipherAdapter* cipher;
signals:

public slots:
    void log(QString what);
    void frame(QTcpSocket*socket, QStringList what);
};

#endif // TESTER_H

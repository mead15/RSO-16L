#ifndef TESTER_H
#define TESTER_H

#include <QObject>
#include <QStringList>
#include <QString>
#include <QTcpSocket>
#include <iostream>
#include "tcpserver.h"

class tester : public QObject
{
    Q_OBJECT
public:
    explicit tester(QObject *parent = 0);
    TcpServer* server;
signals:

public slots:
    void log(QString what);
    void frame(QTcpSocket*socket, QStringList what);
};

#endif // TESTER_H

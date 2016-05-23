#include <QCoreApplication>
//#include <QDebug>
#include <iostream>
#include "servserverlisten.h"



int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);
    TcpServer server(2000);
    ServServerListen sServerListen("settings.ini", &server);
    sServerListen.start();

    QObject::connect(&server, SIGNAL(log(QString)), &sServerListen, SLOT(log(QString)));
    QObject::connect(&server, SIGNAL(error(QString)), &sServerListen, SLOT(error(QString)));
    QObject::connect(&server, SIGNAL(frameContent(QTcpSocket*,QStringList)), &sServerListen, SLOT(frame(QTcpSocket*,QStringList)));


    server.start();
    return a.exec();
}

#include <QCoreApplication>
#include <QObject>

#include "tester.h"
#include "tcpserver.h"
#include "cipheradapter.h"

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    //trzeba postawić serwer, podpiąć mu sloty i wystartować
    TcpServer server(2000);
    CipherAdapter sc(3000);
    tester t2;
    tester t;

    QObject::connect(&server, SIGNAL(log(QString)), &t, SLOT(log(QString)));
    QObject::connect(&server, SIGNAL(frameContent(QTcpSocket*,QStringList)), &t, SLOT(frame(QTcpSocket*,QStringList)));

    QObject::connect(&sc, SIGNAL(log(QString)), &t2, SLOT(log(QString)));
    QObject::connect(&sc, SIGNAL(frameContent(QTcpSocket*,QStringList)), &t2, SLOT(frame(QTcpSocket*,QStringList)));

    server.start();
    sc.start();

    return a.exec();
}

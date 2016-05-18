#include <QCoreApplication>
#include <QObject>

#include "tester.h"
#include "tcpserver.h"

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    //trzeba postawić serwer, podpiąć mu sloty i wystartować
    TcpServer server(2000);
    tester t;

    QObject::connect(&server, SIGNAL(log(QString)), &t, SLOT(log(QString)));
    QObject::connect(&server, SIGNAL(frameContent(QTcpSocket*,QStringList)), &t, SLOT(frame(QTcpSocket*,QStringList)));

    server.start();

    return a.exec();
}

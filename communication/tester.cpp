#include "tester.h"

tester::tester(QObject *parent) :
    QObject(parent)
{
}

void tester::log(QString what)
{
    std::cout << what.toStdString() << std::endl;
}

void tester::frame(QTcpSocket* socket, QStringList what)
{
    std::cout << what.join(" / ").toStdString() << std::endl;
    server->sendFrame(socket, what << "!");
}

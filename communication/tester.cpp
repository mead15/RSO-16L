#include "tester.h"

tester::tester(TcpServer* server) : server(server)
{
}

tester::tester(CipherAdapter *server) : cipher(server)
{
}

void tester::log(QString what)
{
    std::cout << what.toStdString() << std::endl;
}

void tester::frame(QTcpSocket* socket, QStringList what)
{
    std::cout << what.join(" / ").toStdString() << std::endl;
    if(server)
        server->sendFrame(socket, what << "!");
    else
        cipher->sendFrame(socket, what << "!", "klucz");
}

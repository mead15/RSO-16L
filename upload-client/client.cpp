#include "client.h"

Client::Client()
{
    connect(&socket, SIGNAL(readyRead()), this, SLOT(receive()));
}

exitCode Client::send(QStringList what)
{
    bool probowacDalej = true;
    int serverId = 0;

        socket.connectToHost(servers_->at(serverId).ip, servers_->at(serverId).port);
        if(socket.waitForConnected(3000))
        {
            socket.write(QString("("+what.join(",")+")").toStdString().c_str());
            if(!socket.waitForBytesWritten(3000))
            {
                //połączono, ale nie odebrano zapytania
            }
            else
            {
                return OK;
            }
        }
        else
        {
            //nie udało się połączyć
        }


    return NO_SERVERS;
}

void Client::receive()
{
    QString response = socket.readAll();

    int pos1 = response.indexOf("(");
    int pos2 = response.indexOf(")");
    int len = pos2-pos1-1;
    if(len > 0)
        handle(response.mid(pos1+1, len).split(","));
}

void Client::handle(QStringList what)
{
    if(what.at(1) == "OK")
    {
        std::cout << "OK" << std::endl;
        QCoreApplication::instance()->exit(0);
    }
}

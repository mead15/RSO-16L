#include <QCoreApplication>
#include <QSettings>
#include <QCommandLineParser>
#include <QTcpSocket>
#include <QHostAddress>
#include <QVector>
#include <QString>
#include <iostream>
#include "client.h"
#include "server.h"

// parametry: ip, port, polecenie, parametry polecenia
// upload nazwapliku
// insert tabela kolumna kolumna (...)
// attach nazwapliku_lokalna
// delete tabela id
// unlink nazwapliku_zdalna

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);
    QStringList args = QCoreApplication::arguments();
    if(args.size() < 4)
    {
        std::cerr << "Za mało parametrów" << std::endl;
        exit(2);
    }

    Server server;
    QVector<Server> servers;
    server.ip = QHostAddress(args.at(1));
    server.port = args.at(2).toInt();
    servers.push_back(server);

    Client client;
    client.setServers(&servers);

    //sprawdzenie parametrów wywołania

    if(args.at(3) == "upload")
    {
        QString data;
        QFile plik(args.at(4));
        data = plik.readAll().toBase64();
        if(client.send(QStringList() << "UPLOAD" << data) == NO_SERVERS)
        {
            std::cout << "nie można się z niczym połączyć lub nikt nie akceptuje zapytań\n";
            exit(10);
        }
    }
    else if(args.at(3) == "insert")
    {
        QStringList columns;
        for(int i=3; i<args.size(); ++i)
        {
            columns << args.at(i);
        }
        if(client.send(QStringList() << "INSERT" << args.at(2) << columns) == NO_SERVERS)
        {
            std::cout << "nie można się z niczym połączyć lub nikt nie akceptuje zapytań\n";
            exit(10);
        }
    }
    else if(args.at(3) == "attach")
    {
        QString data;
        QFile plik(args.at(4));
        data = plik.readAll().toBase64();
        if(client.send(QStringList() << "ATTACH" << args.at(4) << data) == NO_SERVERS)
        {
            std::cout << "nie można się z niczym połączyć lub nikt nie akceptuje zapytań\n";
            exit(10);
        }
    }
    else if(args.at(3) == "delete")
    {
        if(client.send(QStringList() << "DELETE" << args.at(4) << args.at(5)) == NO_SERVERS)
        {
            std::cout << "nie można się z niczym połączyć lub nikt nie akceptuje zapytań\n";
            exit(10);
        }
    }
    else if(args.at(3) == "unlink")
    {
        if(client.send(QStringList() << "UNLINK" << args.at(4) << args.at(3)) == NO_SERVERS)
        {
            std::cout << "nie można się z niczym połączyć lub nikt nie akceptuje zapytań\n";
            exit(10);
        }
    }

    std::cerr << "Nieznany błąd" << std::endl;
    exit(11);

    return a.exec();
}

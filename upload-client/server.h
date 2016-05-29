#ifndef SERVER_H
#define SERVER_H

#include <QString>
#include <QHostAddress>

struct Server{
    QString name;
    QHostAddress ip;
    int port;
    //QString pubKey;
};

#endif // SERVER_H

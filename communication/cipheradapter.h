#ifndef CIPHERADAPTER_H
#define CIPHERADAPTER_H

#include <QObject>
#include "tcpserver.h"
#include "rsaencrypter.h"

#include <iostream>

class CipherAdapter : public QObject
{
    Q_OBJECT
public:
    CipherAdapter(int port);
    ~CipherAdapter() {}
    void start();
private:
    TcpServer server;
signals:
    void frameContent(QTcpSocket*, QStringList);
    void log(QString);
    void error(QString);    //todo; QString albo int
public slots:
    void sendFrame(QHostAddress recipient, int port, QStringList content, QString key);
    void sendFrame(QTcpSocket* recipient, QStringList content, QString key);
private slots:
    void catchFrameContent(QTcpSocket* socket, QStringList content);
    void catchLog(QString content);
    void catchError(QString content);

};

#endif // CIPHERADAPTER_H

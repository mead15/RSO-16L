#ifndef TCPSERVER_H
#define TCPSERVER_H

#include <QTcpServer>
#include <QHostAddress>
#include <QStringList>
#include <QTcpSocket>

class TcpServer : public QTcpServer
{
    Q_OBJECT
public:
    TcpServer(int port);
    ~TcpServer() {}
    void start();
private:
    int port_;
signals:
    void frameContent(QTcpSocket*, QStringList);
    void log(QString);
    void error(QString);    //todo; QString albo int
public slots:
    void sendFrame(QHostAddress recipient, int port, QStringList content);
    void sendFrame(QTcpSocket* recipient, QStringList content);
private slots:
    void newConnection();
    void disconnected();
    void grabFrame();
};

#endif // TCPSERVER_H

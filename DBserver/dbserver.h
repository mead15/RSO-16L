#ifndef SERVSERVERLISTEN_H
#define SERVSERVERLISTEN_H

#include <QObject>
#include "../communication/cipheradapter.h"
#include "../communication/tcpserver.h"
#include "configuration.h"
#include <QQueue>
#include <QStringList>
#include <QTcpSocket>
#include <QMap>
#include "frametype.h"
#include <iostream>
#include <QVector>
#include "sserver.h"
#include <QHostAddress>
#include <QTime>
#include <QCoreApplication>

class dbServer : public QObject
{
    Q_OBJECT
public:
    dbServer(int extPort, int dbPort, int clientPort);
    void start();

    struct Request{
        QStringList msg;
        QTcpSocket* socket;
    };

    struct LamportRequest : public Request{
        int time;
    };

public slots:
    void frameExtRecived(QTcpSocket*socket, QStringList msg);
    void frameDBRecived(QTcpSocket*socket, QStringList msg);
    void frameClientRecived(QTcpSocket*socket, QStringList msg);
    void frameExtRecivedError(QString error, QString ip);
    void frameDBRecivedError(QString error, QString ip);
    void frameClientRecivedError(QString error, QString ip);
    void stop();
    void log(QString what);

private:
    void mainLoop();
    void masterAction();
    void startElection();
    void frameExtAnalyze(Request& r);
    void frameDBAnalyze(Request r);
    void frameClientAnalyze(LamportRequest r);
    void askForState();
    QStringList getDBState();
    void sendDBStateToAll();
    QStringList makeFrame(QString frameType);
    QStringList makeFrame(QString frameType, QStringList data);
    QStringList makeClientFrame(QString frameType);
    QStringList makeClientFrame(QString frameType, QStringList data);

    QQueue<Request> extQueue;
    QQueue<Request> dbQueue;
    QQueue<LamportRequest> clientQueue;

    CipherAdapter* extPortListener;
    CipherAdapter* dbPortListener;
    TcpServer* clientPortListener;

    typedef void (dbServer::*RsponseFunc)(Request&, int);
    QMap<QString, RsponseFunc> extFunctionMap;
    QMap<QString, RsponseFunc> dbFunctionMap;
    QMap<QString, RsponseFunc> clientFunctionMap;

    bool running;
    QTime lastAskingTime;
    QTime lastBeingAskedTime;

    void status(Request& r, int sender);
    void statusOK(Request& r, int sender);
    void election(Request& r, int sender);
    void coordinator(Request& r, int sender);
    void upload(Request& r, int sender);
    void insert(Request& r, int sender);
    void attach(Request& r, int sender);
    void deletion(Request& r, int sender);
    void unlink(Request& r, int sender);
    void getActiveServersDB(Request& r, int sender);
    void activeServersDB(Request& r, int sender);
    void getAvailableResults(Request& r, int sender);
    void getResult(Request& r, int sender);
    void getStatistics(Request& r, int sender);
    void uploadClient(Request& r, int sender=0);
    void insertClient(Request& r, int sender=0);
    void attachClient(Request& r, int sender=0);
    void deletionClient(Request& r, int sender=0);
    void unlinkClient(Request& r, int sender=0);

};

#endif // SERVSERVERLISTEN_H

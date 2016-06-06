#ifndef EXTSERVER_H
#define EXTSERVER_H

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
#include <QFile>
#include <QCryptographicHash>

class extServer : public QObject
{
    Q_OBJECT
public:
    extServer(int extPort, int dbPort, int clientPort);
    void start();
    bool isMasterCandidate;
    int elecErrorCnt;

    struct Request{
        QStringList msg;
        QTcpSocket* socket;
    };

    static QString logFile;

public slots:
    void frameExtReceived(QTcpSocket*socket, QStringList msg);
    void frameDBReceived(QTcpSocket*socket, QStringList msg);
    void frameClientReceived(QTcpSocket*socket, QStringList msg);
    void frameExtReceivedError(QString ip, QString error);
    void frameDBReceivedError(QString ip, QString error);
    void frameClientReceivedError(QString ip, QString error);
    void stop();
    void log(QString text);

private:
    int randInt(int low, int high);
    void mainLoop();
    void masterAction();
    void startElection();
    void frameExtAnalyze(Request& r);
    void frameDBAnalyze(Request r);
    void frameClientAnalyze(Request r);
    void askForState();
    QStringList getExtState();
    void sendExtStateToAll();
    void sendNewMasterToAll();

    QStringList makeFrame(QString frameType);
    QStringList makeFrame(QString frameType, QStringList data);
    QStringList makeClientFrame(QString frameType);
    QStringList makeClientFrame(QString frameType, QStringList data);

    QQueue<Request> extQueue;
    QQueue<Request> dbQueue;
    QQueue<Request> clientQueue;

    CipherAdapter* extPortListener;
    CipherAdapter* dbPortListener;
    TcpServer* clientPortListener;

    typedef void (extServer::*RsponseFunc)(Request&, int);
    QMap<QString, RsponseFunc> extFunctionMap;
    QMap<QString, RsponseFunc> dbFunctionMap;
    QMap<QString, RsponseFunc> clientFunctionMap;

    bool running;
    QTime lastAskingTime;
    QTime lastBeingAskedTime;

    QMap<QString, QTcpSocket*> clientSocketMap;

    void status(Request& r, int sender);
    void statusOK(Request& r, int sender);
    void election(Request& r, int sender);
    void electionStop(Request& r, int sender);
    void coordinator(Request& r, int sender);

//    void getActiveServersDB(Request& r, int sender);
    void activeServersDB(Request& r, int sender);
    void getActiveServersExt(Request& r, int sender);
    void activeServersExt(Request& r, int sender);
    void getAvailableResults(Request& r, int sender);
    void results(Request& r, int sender);
    void getResult(Request& r, int sender);
    void result(Request& r, int sender);
    void getStatistics(Request& r, int sender);
    void statistics(Request& r, int sender);

    void activeServers(Request& r, int sender=0);

    void errorFrame(Request& r, int sender);
};

#endif // EXTSERVER_H

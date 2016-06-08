#ifndef SERVSERVERLISTEN_H
#define SERVSERVERLISTEN_H

#include <QObject>
#include "../communication/cipheradapter.h"
#include "../communication/tcpserver.h"
#include "../DBHandler/dbhandler.h"
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
#include <QProcess>
#include <QDomDocument>

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

    static QString logFile;

public slots:
    void frameExtRecived(QTcpSocket*socket, QStringList msg);
    void frameDBRecived(QTcpSocket*socket, QStringList msg);
    void frameClientRecived(QTcpSocket*socket, QStringList msg);
    void frameExtRecivedError(QString error, QString ip);
    void frameDBRecivedError(QString error, QString ip);
    void frameClientRecivedError(QString error, QString ip);
    void stop();
    void log(QString text);

private:
    void mainLoop();
    void masterAction();
    void synchronizeClock(int timeStamp);
    void requestOrder(LamportRequest r);
    void startElection();
    void frameExtAnalyze(Request & r);
    void frameDBAnalyze(Request & r);
    void frameDBAnalyze(LamportRequest & r);
    void frameClientAnalyze(Request & r);
    void askForState();
    QStringList getDBState();
    void sendDBStateToAll();
    QStringList makeFrame(QString frameType);
    QStringList makeFrame(QString frameType, QStringList data);
    QStringList makeFrame(QStringList data, int timeStamp);
    QStringList makeClientFrame(QString frameType, QStringList data);
    QStringList makeClientFrame(QString frameType);

    QQueue<Request> extQueue;
    QQueue<Request> dbQueue;
    QQueue<LamportRequest> dbQueueModify;
    QQueue<Request> clientQueue;

    DBHandler* dbh;

    TcpServer* extPortListener;
    TcpServer* dbPortListener;
    TcpServer* clientPortListener;

    typedef void (dbServer::*RsponseFunc)(Request&, int);
    typedef void (dbServer::*RsponseFuncLR)(LamportRequest&, int);
    QMap<QString, RsponseFunc> extFunctionMap;
    QMap<QString, RsponseFunc> dbFunctionMap;
    QMap<QString, RsponseFuncLR> dbFunctionMapLR; //dla wywołań z LamportRequest

    QMap<int, QTcpSocket*> clientSocketMap;
    QMap<int, int> responseNumMap;

    bool running;
    QTime lastAskingTime;
    QTime lastBeingAskedTime;
    int lockalTime;

    void status(Request& r, int sender);
    void statusOK(Request& r, int sender);
    void election(Request& r, int sender);
    void coordinator(Request& r, int sender);
    void upload(LamportRequest & r, int sender);
    void insert(LamportRequest & r, int sender);
    void attach(LamportRequest & r, int sender);
    void deletion(LamportRequest & r, int sender);
    void unlink(LamportRequest & r, int sender);
    void okReceived(LamportRequest& r, int sender);
    void errorReceived(LamportRequest& r, int sender);
    void getActiveServersDB(Request& r, int sender);
    void activeServersDB(Request& r, int sender);
    void getAvailableResults(Request& r, int sender);
    void getResult(Request& r, int sender);
    void getStatistics(Request& r, int sender);
    void checkAllReceived(QString frameType, int stamp);
    void sendErrorFrame(Request &r, int sender, int code);
    void sendErrorFrame(LamportRequest &r, int sender, int code);
};

#endif // SERVSERVERLISTEN_H

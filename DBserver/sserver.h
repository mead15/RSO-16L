#ifndef SSERVER_H
#define SSERVER_H

#include <string>
#include <sstream>
#include <QDebug>
#include <QString>
#include <srvtype.h>

class SServer
{
public:
    SServer(){}
    SServer(int num, QString ip, int portExt, int portDB, int portClient, QString pubKey, SrvType type);

    int getNum() {return num_;}
    QString getIp() { return ip_; }
    int getPortExt() {return portExt_; }
    int getPortDB() {return portDB_; }
    int getPortClient() {return portClient_; }
    QString getPubKey() {return pubKey_;}
    void setActive(bool active) {active_=active;}
    bool isActive(){return active_;}
    SrvType type(){return type_;}

    bool operator==(const SServer& other);

private:
    int num_;
    QString ip_;
    int portExt_;
    int portDB_;
    int portClient_;
    QString pubKey_;
    bool active_;
    SrvType type_;

};

#endif // SSERVER_H

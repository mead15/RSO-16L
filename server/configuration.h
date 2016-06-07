#ifndef CONFIGURATION_H
#define CONFIGURATION_H

#include <QString>
#include <QDebug>
#include <QVector>
#include <QMap>
#include "sserver.h"
#include "srvtype.h"
#include <QSettings>

class Configuration
{
public:
    static Configuration& getInstance();
    bool loadConfig(QString& path);
    void setMyNum(int num) {num_=num;}
    int myNum(){return num_;}
    int interval() {return interval_;}
    QVector<SServer> getExtServers(int maxNum);
    QVector<SServer> getDBServers(int maxNum);
    QVector<SServer> getExtServersUnderMe();
    QVector<SServer> getDBServersUnderMe();
    QMap<int, SServer>& getExtServers() { return servers;}
    QMap<int, SServer>& getDBServers() {return DBServers;}
    SServer& getExtServer(int num){return servers[num];}
    SServer& getDBServer(int num){return DBServers[num];}
    void setServerActive(int num, bool active);
    bool isMaster(){return num_==masterNum;}
    void setMaster(int master){masterNum= master;}
    QVector<int> getActiveDBServers();
    QVector<int> getActiveExtServers();

private:
    Configuration(){}
    Configuration(const Configuration&) = delete;
    Configuration& operator=(const Configuration&) = delete;
    int num_;
    int threads_num_;
    int queue_size_;
    int servers_nb_;
    int DBServers_nb_;
    int interval_;
    void addServer(SServer &srv);
    QMap<int, SServer> servers;
    QMap<int, SServer> DBServers;
    int masterNum;

};

//QDebug operator<<(QDebug qdb, const Configuration& conf);

#endif // CONFIGURATION_H

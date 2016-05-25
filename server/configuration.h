#ifndef CONFIGURATION_H
#define CONFIGURATION_H

#include <QString>
#include <QDebug>
#include <list>
#include <sserver.h>

class Configuration
{
    friend QDebug operator<<(QDebug qdb, const Configuration &conf);
public:
    Configuration();
    void addServer(SServer srv, bool DB);
    void addServers(std::list<SServer> srvs, bool DB);
    void setMyServer(SServer srv);
    void removeServer(SServer srv, bool DB);
    void removeAllServers(bool DB);
    SServer myServer;
    std::list<SServer> servers;
    std::list<SServer> DBServers;
    int threads_num;
    int queue_size;
    int servers_nb;
    int DBServers_nb;
    int interval;

};

QDebug operator<<(QDebug qdb, const Configuration& conf);

#endif // CONFIGURATION_H

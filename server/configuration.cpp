#include "configuration.h"


Configuration::Configuration() : threads_num(0), queue_size(0), servers_nb(0), DBServers_nb(0), interval(0)
{

}

QDebug operator<<(QDebug qdb, const Configuration &conf){
   qdb << "Configuration (";
   qdb << "threads_nb/queue_size/srv_nb/DBSrv_nb" << conf.threads_num << conf.queue_size << conf.servers_nb << conf.DBServers_nb;
   qdb << "Interval: " << conf.interval;
   qdb << "MYSERVER: " << conf.myServer;
   qdb << "SERVERS: ";
   for (auto &s : conf.servers) {
      qdb << s;
   }
   qdb << "DBSERVERS: ";
   for (auto &s : conf.DBServers) {
      qdb << s;
   }
   qdb << ")";
   return qdb;
}

void Configuration::addServer(SServer srv, bool DB)
{
    if(DB) {
        auto it = std::find(DBServers.begin(), DBServers.end(), srv);
        if(it == DBServers.end())
            DBServers.push_back(srv);
    } else {
        auto it = std::find(servers.begin(), servers.end(), srv);
        if(it == servers.end())
            servers.push_back(srv);
    }
}

void Configuration::addServers(std::list<SServer> srvs, bool DB)
{
    for (auto srv : srvs) {
        addServer(srv, DB);
    }
}

void Configuration::setMyServer(SServer srv)
{
    myServer.setIp(srv.getIp());
    myServer.setName(srv.getName());
    myServer.setPort(srv.getPort());
    myServer.setPortClient(srv.getPortClient());
    myServer.setPortDB(srv.getPortDB());
}

void Configuration::removeServer(SServer srv, bool DB)
{
    if(DB)
        DBServers.remove(srv);
    else
        servers.remove(srv);
}

void Configuration::removeAllServers(bool DB)
{
    if(DB)
        DBServers.clear();
    else
        servers.clear();

}

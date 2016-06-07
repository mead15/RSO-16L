#include "configuration.h"


Configuration& Configuration::getInstance(){
    static Configuration instance;
    return instance;
}

void Configuration::addServer(SServer& srv)
{
    if(srv.type() == SrvType::DB) {
        DBServers[srv.getNum()] = srv;
    } else {
        servers[srv.getNum()] = srv;
    }
}

QVector<SServer> Configuration::getExtServers(int maxNum=0){
    QVector<SServer> result;
    for(auto i = servers.begin(); i!=servers.end(); i++){
        if (i.value().getNum()<=maxNum){
            result.push_back(i.value());
        }
    }
    return result;
}

QVector<SServer> Configuration::getDBServers(int maxNum=0){
    QVector<SServer> result;
    for(auto i = DBServers.begin(); i!=DBServers.end(); i++){
        if (i.value().getNum()<=maxNum){
            result.push_back(i.value());
        }
    }
    return result;
}

QVector<SServer> Configuration::getExtServersUnderMe(){
    QVector<SServer> result;
    for(auto i = servers.begin(); i!=servers.end(); i++){
        if (i.value().getNum() < num_){
            result.push_back(i.value());
        }
    }
    return result;
}

QVector<SServer> Configuration::getDBServersUnderMe(){
    QVector<SServer> result;
    for(auto i = DBServers.begin(); i!=DBServers.end(); i++){
        if (i.value().getNum() < num_){
            result.push_back(i.value());
        }
    }
    return result;
}

void Configuration::setServerActive(int num, bool active){
    if(servers.find(num)!=servers.end()){
        servers[num].setActive(active);
        return;
    }
    if(DBServers.find(num)!=DBServers.end()){
        DBServers[num].setActive(active);
        return;
    }

}

bool Configuration::loadConfig(QString& path){
    QSettings setting(path, QSettings::IniFormat);
    setting.sync();
    this->threads_num_ = setting.value("settings/threads_num",0).toInt();
    this->queue_size_ = setting.value("settings/queue_size",0).toInt();
    this->servers_nb_ = setting.value("settings/servers_number",0).toInt();
    this->DBServers_nb_ = setting.value("settings/servers_DB_number",0).toInt();
    this->interval_ = setting.value("master_settings/interval",0).toInt();
    for(int i = 1; i<=this->servers_nb_ + this->DBServers_nb_; ++i) {
        QString ip = setting.value("serwer"+QString::number(i)+"/ip","0.0.0.0").toString();
        int portExt = setting.value("serwer"+QString::number(i)+"/portExt", 0).toInt();
        int portDB = setting.value("serwer"+QString::number(i)+"/portDB", 0).toInt();
        int portClient = setting.value("serwer"+QString::number(i)+"/portClient", 0).toInt();
        QString stype = setting.value("serwer"+QString::number(i)+"/type","srv").toString();
        SrvType type;
        if(stype == "srv"){
            type = SrvType::SRV;
        }
        else{
            if(stype == "db"){
                type = SrvType::DB;
            }
        }
        QString pubKey = setting.value("serwer"+QString::number(i)+"/key","").toString();
        SServer srv(i, ip, portExt, portDB, portClient, pubKey, type);

        addServer(srv);
    }
}

QVector<int> Configuration::getActiveDBServers(){
    QVector<int> result;
    for(auto i = DBServers.begin(); i!=DBServers.end(); i++){
        if (i.value().isActive()){
            result.push_back(i.key());
        }
    }
    return result;
}

QVector<int> Configuration::getActiveExtServers(){
    QVector<int> result;
    for(auto i = servers.begin(); i!=servers.end(); i++){
        if (i.value().isActive()){
            result.push_back(i.key());
        }
    }
    return result;
}





//Configuration::Configuration() : threads_num(0), queue_size(0), servers_nb(0), DBServers_nb(0), interval(0)
//{

//}

//QDebug operator<<(QDebug qdb, const Configuration &conf){
//   qdb << "Configuration (";
//   qdb << "threads_nb/queue_size/srv_nb/DBSrv_nb" << conf.threads_num << conf.queue_size << conf.servers_nb << conf.DBServers_nb;
//   qdb << "Interval: " << conf.interval;
//   qdb << "MYSERVER: " << conf.myServer;
//   qdb << "SERVERS: ";
//   for (auto &s : conf.servers) {
//      qdb << s;
//   }
//   qdb << "DBSERVERS: ";
//   for (auto &s : conf.DBServers) {
//      qdb << s;
//   }
//   qdb << ")";
//   return qdb;
//}

//void Configuration::addServer(SServer srv, bool DB)
//{
//    if(DB) {
//        auto it = std::find(DBServers.begin(), DBServers.end(), srv);
//        if(it == DBServers.end())
//            DBServers.push_back(srv);
//    } else {
//        auto it = std::find(servers.begin(), servers.end(), srv);
//        if(it == servers.end())
//            servers.push_back(srv);
//    }
//}

//void Configuration::addServers(std::list<SServer> srvs, bool DB)
//{
//    for (auto srv : srvs) {
//        addServer(srv, DB);
//    }
//}

//void Configuration::setMyServer(SServer srv)
//{
//    myServer.setIp(srv.getIp());
//    myServer.setName(srv.getName());
//    myServer.setPort(srv.getPort());
//    myServer.setPortClient(srv.getPortClient());
//    myServer.setPortDB(srv.getPortDB());
//}

//void Configuration::removeServer(SServer srv, bool DB)
//{
//    if(DB)
//        DBServers.remove(srv);
//    else
//        servers.remove(srv);
//}

//void Configuration::removeAllServers(bool DB)
//{
//    if(DB)
//        DBServers.clear();
//    else
//        servers.clear();

//}

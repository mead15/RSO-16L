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

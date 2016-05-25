#include "listenmodule.h"
#include <QDebug>

//ListenModule::ListenModule(QObject *parent) : QObject(parent)
//{

//}

ListenModule::ListenModule(const QString &configFileName, TcpServer *server) : server(server)
{
    loadSettings(configFileName);
    qDebug() << config;
    std::cout << "Configuration Loaded" << std::endl;
}

ListenModule::ListenModule(const QString &configFileName, CipherAdapter *server) : cipher(server)
{
    loadSettings(configFileName);
    qDebug() << config;
    std::cout << "Configuration Loaded" << std::endl;
}

ListenModule::~ListenModule()
{
    if(server)
        server->close();
    //else
    //cipher->
}

void ListenModule::loadSettings(const QString &configFileName)
{
    QSettings setting(configFileName, QSettings::IniFormat);
    setting.sync();
    config.threads_num = setting.value("settings/threads_num",0).toInt();
    config.queue_size = setting.value("settings/queue_size",0).toInt();
    config.servers_nb = setting.value("settings/servers_number",0).toInt();
    config.DBServers_nb = setting.value("settings/servers_DB_number",0).toInt();
    this->config.interval = setting.value("master_settings/interval",0).toInt();
    config.myServer.setIp(setting.value("mysettings/ip","0.0.0.0").toString().toStdString());
    config.myServer.setName(setting.value("mysettings/name","Unknown server").toString().toStdString());
    config.myServer.setPort(setting.value("mysettings/port",0).toInt());
    config.myServer.setPortDB(setting.value("mysettings/portDB",0).toInt());
    config.myServer.setPortClient(setting.value("mysettings/portClient",0).toInt());
    for(int i = 1; i<=config.servers_nb; ++i) {
        SServer srv;
        srv.setName(setting.value("serwer"+QString::number(i)+"/name","Unknown server").toString().toStdString());
        srv.setIp(setting.value("serwer"+QString::number(i)+"/ip","0.0.0.0").toString().toStdString());
        srv.setPort(setting.value("serwer"+QString::number(i)+"/port", 0).toInt());
        srv.setPortDB(setting.value("serwer"+QString::number(i)+"/portDB", 0).toInt());
        srv.setPortClient(setting.value("serwer"+QString::number(i)+"/portClient", 0).toInt());
        config.servers.push_back(srv);
    }
    for(int i = 1; i<=config.DBServers_nb; ++i) {
        SServer srv;
        srv.setName(setting.value("serwerDB"+QString::number(i)+"/name","Unknown server").toString().toStdString());
        srv.setIp(setting.value("serwerDB"+QString::number(i)+"/ip","0.0.0.0").toString().toStdString());
        srv.setPort(setting.value("serwerDB"+QString::number(i)+"/port", 0).toInt());
//        srv.setPortDB(setting.value("serwerDB"+QString::number(i)+"/portDB", 0).toInt());
//        srv.setPortClient(setting.value("serwerDB"+QString::number(i)+"/portClient", 0).toInt());
        config.DBServers.push_back(srv);
    }
}

void ListenModule::log(QString what)
{
    std::cout << what.toStdString() << std::endl;
}

void ListenModule::error(QString err)
{
    std::cout << "ListenModule: Failed to send frame" << err.toStdString() << std::endl;
    onFailedToSend(err);
}

void ListenModule::frame(QTcpSocket *socket, QStringList what)
{
   // std::cout << what.join(" / ").toStdString() << std::endl;
    //czy przekazać też wskaźnik na socket :D ?
    onReceive(what);
//    if(server)
//        server->sendFrame(socket, what << "!");
//    else
//        cipher->sendFrame(socket, what << "!", "klucz");
}

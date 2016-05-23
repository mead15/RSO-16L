#include "sserver.h"

SServer::SServer() : name(""), ip(""), port(0) {}

SServer::SServer(std::string name, std::string ip, int port, int portDB, int portClient)
{
    this->name = name;
    this->ip = ip;
    this->port = port;
    this->portDB = portDB;
    this->portClient = portClient;
}

bool SServer::operator==(const SServer &other)
{
    return (ip == other.ip) && (port == other.port);
}

QDebug operator<<(QDebug qdb, SServer srv) {
    qdb << "Server ( name:" << QString::fromStdString(srv.getName()) << " addressIP:" << QString::fromStdString(srv.getIp()) << " port/portDB/portClient:" << srv.getPort() << srv.getPortDB() << srv.getPortClient()<< ")";
    return qdb;
}

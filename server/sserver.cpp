#include "sserver.h"
#include "iostream"

SServer::SServer(int num, QString ip, int portExt, int portDB, int portClient, QString pubKey, SrvType type):
    num_(num), ip_(ip), portExt_(portExt), portDB_(portDB), portClient_(portClient), pubKey_(pubKey),
    active_(false), type_(type)
{
        std::cout<<portExt<<" "<<portDB<<" "<<portClient<<std::endl<<std::flush;
}

bool SServer::operator==(const SServer &other)
{
    return num_ == other.num_;
}

//QDebug operator<<(QDebug qdb, SServer srv) {
//    qdb << "Server ( name:" << QString::fromStdString(srv.getName()) << " addressIP:" << QString::fromStdString(srv.getIp()) << " port/portDB/portClient:" << srv.getPort() << srv.getPortDB() << srv.getPortClient()<< ")";
//    return qdb;
//}

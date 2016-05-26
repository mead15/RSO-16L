#include "sserver.h"


SServer::SServer(int num, QString ip, int portExt, int portDB, int portClient, QString pubKey, SrvType type):
    num_(num), ip_(ip), portExt_(portExt), portDB_(portDB), portClient_(portClient), pubKey_(pubKey),
    active_(false), type_(type)
{
}

bool SServer::operator==(const SServer &other)
{
    return num_ == other.num_;
}

#ifndef SERVSERVERLISTEN_H
#define SERVSERVERLISTEN_H

#include <QObject>
#include "listenmodule.h"

class ServServerListen : public ListenModule
{
    Q_OBJECT
public:
    enum Role {
        COORDINATOR,
        SLAVE,
        NEW,
        COO_CANDIDATE, //CANDIDATE for COORDINATOR
    };

    ServServerListen(const QString& configFileName, TcpServer* server);
    void start();
    QString getRoleText(Role role);

protected:
    virtual void onFailedToSend(QString err);
    virtual void onReceive(QStringList what);

private:
    Role role;
    std::list<SServer> getServers();
    std::list<SServer> getServersUnderMe();
    SServer getMyServer();
    //SServer& getMyCoordinator() { return getServers().front(); }
    SServer& getMyCoordinator() { return myCoordinator; }

    SServer myCoordinator;
    void setRole(const Role role) { this->role = role; }
    bool isCoordinator() const { return role == Role::COORDINATOR; }
    bool isSlave() const { return role == Role::SLAVE; }
    bool isNew() const { return role == Role::NEW; }
    bool isCooCandidate() const { return role == Role::COO_CANDIDATE; }

};

#endif // SERVSERVERLISTEN_H

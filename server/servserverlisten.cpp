#include "servserverlisten.h"


ServServerListen::ServServerListen(const QString &configFileName, TcpServer *server) : ListenModule(configFileName,server)
{
    this->role = Role::NEW;
}

void ServServerListen::start()
{
    if(getServers().front() == getMyServer()) { // first server - default coordinator
        //jakieś uwierzytelnianie?
        setRole(Role::COORDINATOR);
        myCoordinator = getMyServer();
        std::cout << "I am COORDINATOR" << std::endl;
        // ustaw listę serwerów dla bazy??
    } else {
        setRole(Role::SLAVE);
        myCoordinator = getServers().front();
        std::cout << "I am SLAVE" << std::endl;
    }
}

QString ServServerListen::getRoleText(ServServerListen::Role role)
{
    QString roleT;
    switch (role) {
        case Role::COORDINATOR:
            roleT = "COORDINATOR";
            break;
        case Role::SLAVE:
            roleT = "SLAVE";
            break;
        case Role::NEW:
            roleT = "NEW";
            break;
        case Role::COO_CANDIDATE:
            roleT = "COORDINATOR_CANDIDATE";
            break;
        default:
            roleT = "NOT DEFINED";
            break;
    }

    return roleT;
}

void ServServerListen::onFailedToSend(QString err)
{

}

void ServServerListen::onReceive(QStringList what)
{
    std::cout << "I am " << getRoleText(role).toStdString() << "(Role)" << std::endl;
    std::cout << "FRAME:  " << what.join(" / ").toStdString() << std::endl;
    //getServersUnderMe();
    //qDebug() << "My Coordinator" << myCoordinator;
}

std::list<SServer> ServServerListen::getServers()
{
    std::list<SServer> servers = config.servers;
    servers.sort([](SServer &first, SServer &second) {
        return first.getServerNumber() < second.getServerNumber();
    });
    return servers;
}

std::list<SServer> ServServerListen::getServersUnderMe()
{
    std::list<SServer> servers = getServers();
    int myNr = getMyServer().getServerNumber();

    servers.remove_if([&myNr](SServer &other) {
        return myNr <= other.getServerNumber();
    });
    servers.sort([](SServer &first, SServer &second) {
        return first.getServerNumber() < second.getServerNumber();
    });
//    qDebug() << "ServersUnderMe";
//    for(auto &s : servers)
//    {
//        qDebug() << "Serwer " << s.getServerNumber() <<": ip - " << QString::fromStdString(s.getIp());
//    }
    return servers;
}

SServer ServServerListen::getMyServer()
{
    return config.myServer;
}

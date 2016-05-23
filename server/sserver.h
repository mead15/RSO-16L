#ifndef SSERVER_H
#define SSERVER_H

#include <string>
#include <sstream>
#include <QDebug>

class SServer
{
public:
    SServer();
    SServer(std::string name, std::string ip, int port, int portDB, int portClient);

    std::string getIp() { return ip; }
    void setIp(std::string ip) { this->ip = ip; }
    int getPort() {return port; }
    void setPortDB(int port) { this->portDB = port; }
    int getPortDB() {return portDB; }
    void setPortClient(int port) { this->portClient = port; }
    int getPortClient() {return portClient; }
    void setPort(int port) { this->port = port; }
    std::string getName() { return name; }
    void setName(std::string name) { this->name = name; }

    int getServerNumber() {
        if (name != "") {
            return std::stoi(name.substr(name.size() - 1));
            /*int nr;
            std::stringstream ss(name);
            ss >> nr;
            return nr*/;
        }
        return 0;
    }

    bool operator==(const SServer& other);


private:
    std::string ip;
    int port;
    int portDB;
    int portClient;
    std::string name;
};

QDebug operator<<(QDebug qdb, SServer srv);

#endif // SSERVER_H

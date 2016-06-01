#include "dbserver.h"

dbServer::dbServer(int extPort, int dbPort, int clientPort){
    this->log("init server, exxtPort: " + QString::number(extPort) + " dbPort: " + QString::number(dbPort) + " clientPort: " + QString::number(clientPort));
    lastAskingTime = QTime::currentTime().addSecs( - Configuration::getInstance().interval() - 1);
    lastBeingAskedTime = QTime::currentTime();
    std::cout<<extPort<<" "<<dbPort<<" "<<clientPort<<std::endl;
    extPortListener = new CipherAdapter(extPort);
    dbPortListener = new CipherAdapter(dbPort);
    clientPortListener = new TcpServer(clientPort);

    QObject::connect(extPortListener, SIGNAL(log(QString)), this, SLOT(log(QString)));
    QObject::connect(extPortListener, SIGNAL(frameContent(QTcpSocket*,QStringList)), this, SLOT(frameExtRecived(QTcpSocket*,QStringList)));
    //QObject::connect(extPortListener, SIGNAL(error(QString)), this, SLOT(frameExtRecived(QTcpSocket*,QStringList)));

    QObject::connect(dbPortListener, SIGNAL(log(QString)), this, SLOT(log(QString)));
    QObject::connect(dbPortListener, SIGNAL(frameContent(QTcpSocket*,QStringList)), this, SLOT(frameDBRecived(QTcpSocket*,QStringList)));

    QObject::connect(clientPortListener, SIGNAL(log(QString)), this, SLOT(log(QString)));
    QObject::connect(clientPortListener, SIGNAL(frameContent(QTcpSocket*,QStringList)), this, SLOT(frameClientRecived(QTcpSocket*,QStringList)));

    dbFunctionMap[FrameType::STATUS] = &dbServer::status;
    dbFunctionMap[FrameType::SERVER_STATUS_OK] = &dbServer::statusOK;
    dbFunctionMap[FrameType::ACTIVE_SERVERS_DB] = &dbServer::activeServersDB;

    extFunctionMap[FrameType::GET_ACTIVE_SERVERS_DB] = &dbServer::getActiveServersDB;
}

void dbServer::start(){
    this->log("Start server");
    running = true;
    extPortListener->start();
    dbPortListener->start();
    clientPortListener->start();
    while(running){
        this->log("loop");
        mainLoop();
    }

}

void dbServer::mainLoop(){
    if(this->clientQueue.isEmpty() || this->extQueue.isEmpty() || this->dbQueue.isEmpty()){
        QTime dieTime= QTime::currentTime().addSecs(5);
        while (QTime::currentTime() < dieTime)
            QCoreApplication::processEvents(QEventLoop::AllEvents, 100);
    }
    else{
        if(!this->dbQueue.isEmpty()){
            frameDBAnalyze(dbQueue.first());
            dbQueue.pop_front();
        }
        if(!this->extQueue.isEmpty()){
            frameExtAnalyze(extQueue.first());
            extQueue.pop_front();
        }
        if(!this->clientQueue.isEmpty()){
            frameClientAnalyze(clientQueue.first());
            clientQueue.pop_front();
        }
        masterAction();
    }
}

void dbServer::frameExtRecived(QTcpSocket* socket, QStringList msg){
    log("extFrame: " + msg.join(","));
    Request r;
    r.msg = msg;
    r.socket = socket;
    this->extQueue.append(r);
}

void dbServer::frameDBRecived(QTcpSocket*socket, QStringList msg){
    log("dbFrame: " + msg.join(","));
    Request r;
    r.msg = msg;
    r.socket = socket;
    this->dbQueue.append(r);
}

void dbServer::frameClientRecived(QTcpSocket*socket, QStringList msg){
    log("client Frame: " + msg.join(","));
    std::cout<<"recived from client"<<std::endl;
    LamportRequest r;
    r.msg = msg;
    r.socket = socket;
    // r.time = current time
    // add in good position
    this->clientQueue.append(r);
}

void dbServer::frameExtAnalyze(Request &r){
    int sender = r.msg[0].toInt();
    QString type = r.msg[1];
    (this->*(extFunctionMap[type]))(r, sender);
}

void dbServer::frameDBAnalyze(Request r){
    int sender = r.msg[0].toInt();
    QString type = r.msg[1];
    (this->*(dbFunctionMap[type]))(r, sender);
}

void dbServer::frameClientAnalyze(LamportRequest r){
    QString type = r.msg[0];
    (this->*(dbFunctionMap[type]))(r, 0);
}

void dbServer::frameExtRecivedError(QString error, QString ip){

}

void dbServer::frameDBRecivedError(QString error, QString ip){
    log("error: " + error);
    QMap<int, SServer> servers = Configuration::getInstance().getDBServers();
    for (auto i = servers.begin(); i!=servers.end(); i++){
        if(i.value().getIp()==ip && i.value().isActive()){
            Configuration::getInstance().setServerActive(i.key(), false);
            this->sendDBStateToAll();
            return;
        }
    }
}

void dbServer::frameClientRecivedError(QString error, QString ip){

}


void dbServer::stop(){
    running = false;
}

QString dbServer::logFile="log.txt";

void dbServer::log(QString text)
{
    QFile file(logFile);
    file.open(QIODevice::Append);
    QTextStream out(&file);
    out <<QTime::currentTime().toString()<<": "<< text <<"\n";
    file.flush();
    file.close();
}

void dbServer::masterAction(){
    if(Configuration::getInstance().isMaster()){
        if(lastAskingTime.secsTo(QTime::currentTime()) >= Configuration::getInstance().interval() )
            askForState();
    }
    else{
        if(lastBeingAskedTime.secsTo(QTime::currentTime())> 2 * Configuration::getInstance().interval())
            log("no master!");
            startElection();
    }
}

void dbServer::startElection(){

}

void dbServer::askForState(){
    log("ask for state");
    QMap<int, SServer> servers = Configuration::getInstance().getDBServers();
    for (auto i = servers.begin(); i!=servers.end(); i++){
        dbPortListener->sendFrame(QHostAddress(i.value().getIp()), i.value().getPortDB(), makeFrame(FrameType::STATUS), i.value().getPubKey());
    }
}

QStringList dbServer::getDBState(){
    log("read DB servers state");
    QVector<int> active = Configuration::getInstance().getActiveDBServers();
    QStringList result;
    for (auto i = active.begin(); i!=active.end(); i++){
           result << QString::number(*i);
    }
    return result;
}

void dbServer::sendDBStateToAll(){
    log("propagate DB servers state");
    QStringList state = getDBState();
    QMap<int, SServer> servers = Configuration::getInstance().getDBServers();
    for (auto i = servers.begin(); i!=servers.end(); i++){
        dbPortListener->sendFrame(QHostAddress(i.value().getIp()), i.value().getPortDB(), makeFrame(FrameType::ACTIVE_SERVERS_DB, state), i.value().getPubKey());
    }
}

QStringList dbServer::makeFrame(QString frameType, QStringList data){
    return QStringList() << QString::number(Configuration::getInstance().myNum()) << frameType << data;
}

QStringList dbServer::makeClientFrame(QString frameType, QStringList data){
    return QStringList() << frameType << data;
}

QStringList dbServer::makeFrame(QString frameType){
    return QStringList() << QString::number(Configuration::getInstance().myNum()) << frameType;
}

QStringList dbServer::makeClientFrame(QString frameType){
    return QStringList() << frameType;
}

void dbServer::status(Request& r, int sender){
    dbPortListener->sendFrame(r.socket, makeFrame(FrameType::SERVER_STATUS_OK), Configuration::getInstance().getDBServer(sender).getPubKey());
    if(Configuration::getInstance().isMaster()){
        Configuration::getInstance().setMaster(sender);
    }
}

void dbServer::statusOK(Request& r, int sender){
    log(QString::number(sender) + " is OK");
    if(Configuration::getInstance().isMaster()){
        if(!Configuration::getInstance().getDBServer(sender).isActive()){
            Configuration::getInstance().setServerActive(sender, true);
            sendDBStateToAll();
        }
    }
}

void dbServer::election(Request& r, int sender){

}

void dbServer::coordinator(Request& r, int sender){

}

void dbServer::upload(Request& r, int sender){

}

void dbServer::insert(Request& r, int sender){

}

void dbServer::attach(Request& r, int sender){

}

void dbServer::deletion(Request& r, int sender){

}

void dbServer::unlink(Request& r, int sender){

}

void dbServer::getActiveServersDB(Request& r, int sender){
    log("send active db servers to ext server: " + QString::number(sender));
    QMap<int, SServer> servers = Configuration::getInstance().getDBServers();
    QStringList result;
    for (auto i = servers.begin(); i!=servers.end(); i++){
        if(i.value().isActive())
            result<<QString::number(i.key());
    }
    extPortListener->sendFrame(r.socket, makeFrame(FrameType::ACTIVE_SERVERS_DB, result), Configuration::getInstance().getExtServer(sender).getPubKey());
}

void dbServer::activeServersDB(Request& r, int sender){
    log("recived active servers from: " + QString::number(sender));
    QMap<int, SServer> servers = Configuration::getInstance().getDBServers();
    for (auto i = servers.begin(); i!=servers.end(); i++){
        if(r.msg.contains(QString::number(i.key())))
            Configuration::getInstance().setServerActive(i.key(), true);
        else
            Configuration::getInstance().setServerActive(i.key(), false);
    }

}

void dbServer::getAvailableResults(Request& r, int sender){

}

void dbServer::getResult(Request& r, int sender){

}

void dbServer::getStatistics(Request& r, int sender){

}

void dbServer::uploadClient(Request& r, int sender){

}

void dbServer::insertClient(Request& r, int sender){

}

void dbServer::attachClient(Request& r, int sender){

}

void dbServer::deletionClient(Request& r, int sender){

}

void dbServer::unlinkClient(Request& r, int sender){

}

#include "extserver.h"

extServer::extServer(int extPort, int dbPort, int clientPort){
    this->log("Init ExtServer, extPort: " + QString::number(extPort) + " dbPort: " + QString::number(dbPort) + " clientPort: " + QString::number(clientPort));
    isMasterCandidate = false;
    elecErrorCnt = 0;
    lastAskingTime = QTime::currentTime().addSecs( - Configuration::getInstance().interval() - 1);
    lastBeingAskedTime = QTime::currentTime();
    std::cout<<"myNum:"<<Configuration::getInstance().myNum()<<", ports: "<<extPort<<" "<<dbPort<<" "<<clientPort<<std::endl;
    extPortListener = new CipherAdapter(extPort);
    dbPortListener = new CipherAdapter(dbPort);
    clientPortListener = new TcpServer(clientPort);

    QObject::connect(extPortListener, SIGNAL(log(QString)), this, SLOT(log(QString)));
    QObject::connect(extPortListener, SIGNAL(frameContent(QTcpSocket*,QStringList)), this, SLOT(frameExtReceived(QTcpSocket*,QStringList)));
    QObject::connect(extPortListener, SIGNAL(error(QString, QString)), this, SLOT(frameExtReceivedError(QString,QString)));

    QObject::connect(dbPortListener, SIGNAL(log(QString)), this, SLOT(log(QString)));
    QObject::connect(dbPortListener, SIGNAL(frameContent(QTcpSocket*,QStringList)), this, SLOT(frameDBReceived(QTcpSocket*,QStringList)));
    QObject::connect(dbPortListener, SIGNAL(error(QString, QString)), this, SLOT(frameExtReceivedError(QString,QString)));

    QObject::connect(clientPortListener, SIGNAL(log(QString)), this, SLOT(log(QString)));
    QObject::connect(clientPortListener, SIGNAL(frameContent(QTcpSocket*,QStringList)), this, SLOT(frameClientReceived(QTcpSocket*,QStringList)));
    QObject::connect(clientPortListener, SIGNAL(error(QString, QString)), this, SLOT(frameExtReceivedError(QString,QString)));

    extFunctionMap[FrameType::STATUS] = &extServer::status;
    extFunctionMap[FrameType::SERVER_STATUS_OK] = &extServer::statusOK;

    extFunctionMap[FrameType::ACTIVE_SERVERS_EXT] = &extServer::activeServersExt;

    extFunctionMap[FrameType::ELECTION] = &extServer::election;
    extFunctionMap[FrameType::ELECTION_STOP] = &extServer::electionStop;
    extFunctionMap[FrameType::COORDINATOR] = &extServer::coordinator;
    extFunctionMap[FrameType::ACTIVE_SERVERS_DB] = &extServer::activeServersDB;

    dbFunctionMap[FrameType::GET_ACTIVE_SERVERS_EXT] = &extServer::getActiveServersExt;
    dbFunctionMap[FrameType::ACTIVE_SERVERS_DB] = &extServer::activeServersDB;
    dbFunctionMap[FrameType::RESULTS] = &extServer::results;
    dbFunctionMap[FrameType::RESULT] = &extServer::result;
    dbFunctionMap[FrameType::STATISTICS] = &extServer::statistics;

    clientFunctionMap[FrameType::ACTIVE_SERVERS] = &extServer::activeServers;
    clientFunctionMap[FrameType::GET_AVAILABLE_RESULTS] = &extServer::getAvailableResults;
    clientFunctionMap[FrameType::GET_RESULT] = &extServer::getResult;
    clientFunctionMap[FrameType::GET_STATISTICS] = &extServer::getStatistics;

    extFunctionMap[FrameType::ERROR] = &extServer::errorFrame;
    clientFunctionMap[FrameType::ERROR] = &extServer::errorFrame;
    dbFunctionMap[FrameType::ERROR] = &extServer::errorFrame;
}

void extServer::start(){
    this->log("Start server");
    running = true;
    qsrand((uint)QTime::currentTime().msec());
    extPortListener->start();
    dbPortListener->start();
    clientPortListener->start();
    while(running){
        //this->log("loop");
        mainLoop();
    }
}

QString extServer::logFile="log.txt";

void extServer::mainLoop(){
    if(this->clientQueue.isEmpty() && this->extQueue.isEmpty() && this->dbQueue.isEmpty()){
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
        //masterAction();  //tymczas na zewnątrz
    }
    masterAction();
}

void extServer::frameExtReceived(QTcpSocket* socket, QStringList msg){
    log("extFrame: " + msg.join(","));
    Request r;
    r.msg = msg;
    r.socket = socket;
    this->extQueue.append(r);
}

void extServer::frameDBReceived(QTcpSocket*socket, QStringList msg){
    log("dbFrame: " + msg.join(","));
    Request r;
    r.msg = msg;
    r.socket = socket;
    this->dbQueue.append(r);
}

void extServer::frameClientReceived(QTcpSocket*socket, QStringList msg){
    log("client Frame: " + msg.join(","));
    std::cout<<"Received msg from client"<<std::endl;
    Request r;
    r.msg = msg;
    r.socket = socket;
    this->clientQueue.append(r);
}

void extServer::frameExtAnalyze(Request &r){
    int sender = r.msg[0].toInt();
    QString type = r.msg[1];
    (this->*(extFunctionMap[type]))(r, sender);
}

void extServer::frameDBAnalyze(Request r){
    int sender = r.msg[0].toInt();
    QString type = r.msg[1];
    (this->*(dbFunctionMap[type]))(r, sender);
}

void extServer::frameClientAnalyze(Request r){
    QString type = r.msg[0];
    (this->*(clientFunctionMap[type]))(r, 0);
}

void extServer::frameExtReceivedError(QString ip, QString error){
    log("FrameExt error: " + error);
    //
    if(isMasterCandidate){
        elecErrorCnt++;
    }
    //
    QMap<int, SServer> servers = Configuration::getInstance().getExtServers();
    for (auto i = servers.begin(); i!=servers.end(); i++){
        if(i.value().getIp()==ip && i.value().isActive()){
            Configuration::getInstance().setServerActive(i.key(), false);
            this->sendExtStateToAll();
            return;
        }
    }
}

void extServer::frameDBReceivedError(QString ip, QString error){

}

void extServer::frameClientReceivedError(QString ip, QString error){

}

void extServer::masterAction(){
    std::cout<<"masterAction"<<std::endl;
    std::cout<<Configuration::getInstance().isMaster()<<std::endl;
    //std::cout<<isMasterCandidate<<std::endl;
    if(Configuration::getInstance().isMaster()){
        if(lastAskingTime.secsTo(QTime::currentTime()) >= Configuration::getInstance().interval() )
            askForState();
    }
    else{
        if(lastBeingAskedTime.secsTo(QTime::currentTime())> 2 * Configuration::getInstance().interval()){
            log("ExtServers:: NO master!");
            startElection();
        }
    }
    if(isMasterCandidate){
        QVector<SServer> serversUnder = Configuration::getInstance().getExtServersUnderMe();
        if(serversUnder.size() == elecErrorCnt){
            isMasterCandidate = false;
            Configuration::getInstance().setMaster(Configuration::getInstance().myNum());
            this->sendNewMasterToAll();
            elecErrorCnt = 0;
        }
    }
}

void extServer::startElection(){
    QVector<int> active = Configuration::getInstance().getActiveExtServers();
    if(active.size() == 1){
        Configuration::getInstance().setMaster(Configuration::getInstance().myNum());
        //send frame coordinator (not necessary)
        return;
    }
    QVector<SServer> serversUnder = Configuration::getInstance().getExtServersUnderMe();
    if(serversUnder.isEmpty()){
        Configuration::getInstance().setMaster(Configuration::getInstance().myNum());
        this->sendNewMasterToAll();
        elecErrorCnt = 0;
    } else {
        isMasterCandidate = true;
        for (auto it = serversUnder.begin(); it!=serversUnder.end(); it++){
            SServer srv = *it;
            extPortListener->sendFrame(QHostAddress(srv.getIp()), srv.getPortExt(), makeFrame(FrameType::ELECTION), srv.getPubKey());
        }
    }

}

void extServer::askForState(){
    log("ExtServers:: Check if alive");
    QMap<int, SServer> servers = Configuration::getInstance().getExtServers();
    for (auto i = servers.begin(); i!=servers.end(); i++){
        extPortListener->sendFrame(QHostAddress(i.value().getIp()), i.value().getPortExt(), makeFrame(FrameType::STATUS), i.value().getPubKey());
    }
    lastAskingTime = QTime::currentTime();
}

QStringList extServer::getExtState(){
    log("Read  ExtServers state");
    QVector<int> active = Configuration::getInstance().getActiveExtServers();
    QStringList result;
    for (auto i = active.begin(); i!=active.end(); i++){
           result << QString::number(*i);
    }
    return result;
}

void extServer::sendExtStateToAll(){
    log("Propagate ExtServers state");
    QStringList state = getExtState();
    QMap<int, SServer> servers = Configuration::getInstance().getExtServers();
    for (auto i = servers.begin(); i!=servers.end(); i++){
        extPortListener->sendFrame(QHostAddress(i.value().getIp()), i.value().getPortExt(), makeFrame(FrameType::ACTIVE_SERVERS_EXT, state), i.value().getPubKey());
    }
}

void extServer::sendNewMasterToAll(){
    log("ExtServers:: NEW Master! -> " + QString::number(Configuration::getInstance().myNum()));
    QMap<int, SServer> servers = Configuration::getInstance().getExtServers();
    for (auto i = servers.begin(); i!=servers.end(); i++){
        extPortListener->sendFrame(QHostAddress(i.value().getIp()), i.value().getPortExt(), makeFrame(FrameType::COORDINATOR), i.value().getPubKey());
    }
}

QStringList extServer::makeFrame(QString frameType, QStringList data){
    return QStringList() << QString::number(Configuration::getInstance().myNum()) << frameType << data;
}

QStringList extServer::makeClientFrame(QString frameType, QStringList data){
    return QStringList() << frameType << data;
}

QStringList extServer::makeFrame(QString frameType){
    return QStringList() << QString::number(Configuration::getInstance().myNum()) << frameType;
}

QStringList extServer::makeClientFrame(QString frameType){
    return QStringList() << frameType;
}

void extServer::status(Request& r, int sender){
    lastBeingAskedTime = QTime::currentTime();
    extPortListener->sendFrame(r.socket, makeFrame(FrameType::SERVER_STATUS_OK), Configuration::getInstance().getExtServer(sender).getPubKey());
    if(Configuration::getInstance().isMaster()){
        Configuration::getInstance().setMaster(sender);
    }
}

void extServer::statusOK(Request& r, int sender){
    log(QString::number(sender) + " is OK/Alive");
    if(Configuration::getInstance().isMaster()){
        if(!Configuration::getInstance().getExtServer(sender).isActive()){
            Configuration::getInstance().setServerActive(sender, true);
            sendExtStateToAll();
        }
    }
}

void extServer::election(Request& r, int sender){
    isMasterCandidate = true;//jest w startElection
    extPortListener->sendFrame(r.socket, makeFrame(FrameType::ELECTION_STOP), Configuration::getInstance().getExtServer(sender).getPubKey());
    startElection();
}

void extServer::electionStop(Request& r, int sender){
    isMasterCandidate = false;
}

void extServer::coordinator(Request& r, int sender){
    log("ExtServers:: NEW Master! -> " + QString::number(sender));
    isMasterCandidate = false;
    Configuration::getInstance().setMaster(sender);
}

void extServer::getActiveServersExt(Request& r, int sender){
    log("Send active ExtServers to DBServer: " + QString::number(sender));
    QMap<int, SServer> servers = Configuration::getInstance().getExtServers();
    QStringList result;
    for (auto i = servers.begin(); i!=servers.end(); i++){
        if(i.value().isActive())
            result<<QString::number(i.key());
    }
    dbPortListener->sendFrame(r.socket, makeFrame(FrameType::ACTIVE_SERVERS_EXT, result), Configuration::getInstance().getDBServer(sender).getPubKey());
}

void extServer::activeServersExt(Request& r, int sender){
    log("Received active ExtServers from: " + QString::number(sender));
    QMap<int, SServer> servers = Configuration::getInstance().getExtServers();
    for (auto i = servers.begin(); i!=servers.end(); i++){
        if(r.msg.contains(QString::number(i.key())))
            Configuration::getInstance().setServerActive(i.key(), true);
        else
            Configuration::getInstance().setServerActive(i.key(), false);
    }
}

void extServer::activeServersDB(Request& r, int sender){
    if(Configuration::getInstance().isMaster()) {
        log("Received active DBServers from: (DBServer)" + QString::number(sender));
        log("Propagate DBServers state");
        QStringList result;
        for(int i = 2; i < r.msg.size(); i++){
            result << r.msg.at(i);
        }
        QMap<int, SServer> servers = Configuration::getInstance().getExtServers();
        for (auto i = servers.begin(); i!=servers.end(); i++){
            dbPortListener->sendFrame(QHostAddress(i.value().getIp()), i.value().getPortExt(), makeFrame(FrameType::ACTIVE_SERVERS_EXT, result), i.value().getPubKey());
        }
    } else {
        log("Received active DBServers from: (myMaster)" + QString::number(sender));
    }
    QMap<int, SServer> servers = Configuration::getInstance().getDBServers();
    for (auto i = servers.begin(); i!=servers.end(); i++){
        if(r.msg.contains(QString::number(i.key())))
            Configuration::getInstance().setServerActive(i.key(), true);
        else
            Configuration::getInstance().setServerActive(i.key(), false);
    }
}

void extServer::activeServers(Request &r, int sender) {
    log("Send active ExtServers to Client");
    QMap<int, SServer> servers = Configuration::getInstance().getExtServers();
    QVector<int> active = Configuration::getInstance().getActiveExtServers();
    QStringList result;
    result << QString::number(active.size());
    for (auto i = servers.begin(); i!=servers.end(); i++){
        if(i.value().isActive())
            result << i.value().getIp() << QString::number(i.value().getPortClient());
    }
    clientPortListener->sendFrame(r.socket, makeClientFrame(FrameType::ACTIVE_SERVERS, result));
}

void extServer::getAvailableResults(Request& r, int sender){
    log("Received getAvailableResults from Client");
    QString currTime = QTime::currentTime().toString();
//    std::cout<<"currTime: "<<currTime.toStdString()<<std::endl;
//    QByteArray qba;
//    QByteArray hash = QCryptographicHash::hash(qba.append(currTime),QCryptographicHash::Md5);
    clientSocketMap[currTime] = r.socket;
    //send to random active DBServer
    QVector<int> active = Configuration::getInstance().getActiveDBServers();
    if(!active.isEmpty()){
        int dbServer_nb = randInt(0, active.size()-1);
        QStringList result;
        result << currTime;
        for(int i = 1; i < r.msg.size(); i++){
            result << r.msg.at(i);
        }
        SServer srv = Configuration::getInstance().getDBServer(active.at(dbServer_nb));
        dbPortListener->sendFrame(QHostAddress(srv.getIp()), srv.getPortExt(), makeFrame(FrameType::GET_AVAILABLE_RESULTS, result), srv.getPubKey());
        //clientPortListener->sendFrame(QHostAddress(srv.getIp()), srv.getPortExt(), makeFrame(FrameType::GET_AVAILABLE_RESULTS, result));
    } else{
        clientPortListener->sendFrame(r.socket, makeClientFrame(FrameType::ERROR, QStringList() << "DB_LOST_CONNECTION"));
    }
}

void extServer::getResult(Request& r, int sender){
    log("Received getResult from Client");
    QString currTime = QTime::currentTime().toString();
    std::cout<<"currTime: "<<currTime.toStdString()<<std::endl;
//    QByteArray qba;
//    QByteArray hash = QCryptographicHash::hash(qba.append(currTime),QCryptographicHash::Md5);
    clientSocketMap[currTime] = r.socket;
    //send to random active DBServer
    QVector<int> active = Configuration::getInstance().getActiveDBServers();
    if(!active.isEmpty()){
        int dbServer_nb = randInt(0, active.size()-1);
        QStringList result;
        result << currTime;
        for(int i = 1; i < r.msg.size(); i++){
            result << r.msg.at(i);
        }
        SServer srv = Configuration::getInstance().getDBServer(active.at(dbServer_nb));
        dbPortListener->sendFrame(QHostAddress(srv.getIp()), srv.getPortExt(), makeFrame(FrameType::GET_RESULT, result), srv.getPubKey());
        //clientPortListener->sendFrame(QHostAddress(srv.getIp()), srv.getPortExt(), makeFrame(FrameType::GET_RESULT, result));
    } else{
        clientPortListener->sendFrame(r.socket, makeClientFrame(FrameType::ERROR, QStringList() << "DB_LOST_CONNECTION"));
    }
}

void extServer::getStatistics(Request& r, int sender){
    log("Received getStatistics from Client");
    QString currTime = QTime::currentTime().toString();
//    std::cout<<"currTime: "<<currTime.toStdString()<<std::endl;
//    QByteArray qba;
//    QByteArray hash = QCryptographicHash::hash(qba.append(currTime),QCryptographicHash::Md5);
    clientSocketMap[currTime] = r.socket;
    //send to random active DBServer
    QVector<int> active = Configuration::getInstance().getActiveDBServers();
    if(!active.isEmpty()){
        int dbServer_nb = randInt(0, active.size()-1);
        QStringList result;
        result << currTime;
        for(int i = 1; i < r.msg.size(); i++){
            result << r.msg.at(i);
        }
        SServer srv = Configuration::getInstance().getDBServer(active.at(dbServer_nb));
        dbPortListener->sendFrame(QHostAddress(srv.getIp()), srv.getPortExt(), makeFrame(FrameType::GET_STATISTICS, result), srv.getPubKey());
        //clientPortListener->sendFrame(QHostAddress(srv.getIp()), srv.getPortExt(), makeFrame(FrameType::GET_STATISTICS, result));
    } else{
        clientPortListener->sendFrame(r.socket, makeClientFrame(FrameType::ERROR, QStringList() << "DB_LOST_CONNECTION"));
    }
}

void extServer::results(Request &r, int sender){
    QString mapId = r.msg[2];
    if(clientSocketMap.find(mapId)!=clientSocketMap.end()){
        QStringList result;
        for(int i = 3; i < r.msg.size(); i++){//0->nr_nadawcy, 1->frameType, 2->currTime
            result << r.msg.at(i);
        }
        //dbPortListener
        clientPortListener->sendFrame(clientSocketMap[mapId], makeClientFrame(FrameType::RESULTS, result));
        clientSocketMap.remove(mapId);
        return;
    } else {
        log("Oops! Key not found in clientSocketMap");
    }
}

void extServer::result(Request &r, int sender){
    QString mapId = r.msg[2];
    if(clientSocketMap.find(mapId)!=clientSocketMap.end()){
        QStringList result;
        for(int i = 3; i < r.msg.size(); i++){//0->nr_nadawcy, 1->frameType, 2->currTime
            result << r.msg.at(i);
        }
        //dbPortListener
        clientPortListener->sendFrame(clientSocketMap[mapId], makeClientFrame(FrameType::RESULT, result));
        clientSocketMap.remove(mapId);
        return;
    } else {
        log("Oops! Key not found in clientSocketMap");
    }
}

void extServer::statistics(Request &r, int sender){
    QString mapId = r.msg[2];
    if(clientSocketMap.find(mapId)!=clientSocketMap.end()){
        QStringList result;
        for(int i = 3; i < r.msg.size(); i++){//0->nr_nadawcy, 1->frameType, 2->currTime
            result << r.msg.at(i);
        }
        //dbPortListener
        clientPortListener->sendFrame(clientSocketMap[mapId], makeClientFrame(FrameType::STATISTICS, result));
        clientSocketMap.remove(mapId);
        return;
    } else {
        log("Oops! Key not found in clientSocketMap");
    }
}

void extServer::errorFrame(Request &r, int sender){

}

int extServer::randInt(int low, int high)
{
// Random number between low and high
return qrand() % ((high + 1) - low) + low;
}

void extServer::log(QString text)
{
    QFile file(logFile);
    file.open(QIODevice::Append);
    QTextStream out(&file);
    out <<QTime::currentTime().toString()<<": "<< text <<"\n";
    file.flush();
    file.close();
}

void extServer::stop(){
    running = false;
}

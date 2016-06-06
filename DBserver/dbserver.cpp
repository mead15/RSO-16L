#include "dbserver.h"

dbServer::dbServer(int extPort, int dbPort, int clientPort){
    this->log("init server, exxtPort: " + QString::number(extPort) + " dbPort: " + QString::number(dbPort) + " clientPort: " + QString::number(clientPort));
    lastAskingTime = QTime::currentTime().addSecs( - Configuration::getInstance().interval() - 1);
    lastBeingAskedTime = QTime::currentTime();
    lockalTime =0;
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
    dbFunctionMapLR[FrameType::UPLOAD] = &dbServer::upload;
    dbFunctionMapLR[FrameType::INSERT] = &dbServer::insert;
    dbFunctionMapLR[FrameType::ATTACH] = &dbServer::attach;
    dbFunctionMapLR[FrameType::DELETE] = &dbServer::deletion;
    dbFunctionMapLR[FrameType::UNLINK] = &dbServer::unlink;
    dbFunctionMap[FrameType::OK] = &dbServer::okReceived;
    
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
    if(this->clientQueue.isEmpty() && this->extQueue.isEmpty() && this->dbQueue.isEmpty() && this->dbQueueModify.isEmpty()){
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
        if(!this->dbQueueModify.isEmpty()){
            frameDBAnalyze(dbQueueModify.first());
            dbQueueModify.pop_front();
        }
    }
    masterAction();
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
    if (msg.size()>3 && (QStringList() << FrameType::UPLOAD << FrameType::INSERT << FrameType::ATTACH << FrameType::DELETE << FrameType::UNLINK).contains(msg[1])){
        int timeStamp = msg[2].toInt();
        synchronizeClock(timeStamp);
        LamportRequest r;
        r.msg = QStringList() << msg[0] << msg[1] << msg[3];
        r.socket = socket;
        r.time = timeStamp;
        requestOrder(r);
    }
    else{
        Request r;
        r.msg = msg;
        r.socket = socket;
        this->dbQueue.append(r);
    }
}

void dbServer::frameClientRecived(QTcpSocket*socket, QStringList msg){
    log("client Frame: " + msg.join(","));
    std::cout<<"recived from client"<<std::endl;
    Request r;
    r.msg = msg;
    r.socket = socket;
    this->clientQueue.append(r);
}

void dbServer::frameExtAnalyze(Request &r){
    int sender = r.msg[0].toInt();
    QString type = r.msg[1];
    (this->*(extFunctionMap[type]))(r, sender);
}

void dbServer::frameDBAnalyze(Request & r){
    int sender = r.msg[0].toInt();
    QString type;
    if (r.msg[2] == "OK")
        type = "OK";
    else       
        type = r.msg[1];
    (this->*(dbFunctionMap[type]))(r, sender);
}

void dbServer::frameDBAnalyze(LamportRequest & r){
    int sender = r.msg[0].toInt();
    QString type = r.msg[1];
    (this->*(dbFunctionMapLR[type]))(r, sender);
}

void dbServer::frameClientAnalyze(Request & r){
    LamportRequest rL;
    rL.msg = QStringList() << "0" << r.msg;
    rL.socket = r.socket;
    rL.time = lockalTime;
    lockalTime +=1;
    QMap<int, SServer> servers = Configuration::getInstance().getDBServers();
    clientSocketMap[rL.time] = rL.socket;
    responseNumMap[rL.time] = servers.size() + 1; //ilość koniecznych potwiedzeń zapisu (wszystkie serwery + ja)
    requestOrder(rL);
    for (auto i = servers.begin(); i!=servers.end(); i++){
        dbPortListener->sendFrame(QHostAddress(i.value().getIp()), i.value().getPortDB(), makeFrame(r.msg, rL.time), i.value().getPubKey());
    }
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

void dbServer::synchronizeClock(int timeStamp){
    if (lockalTime <= timeStamp){
        lockalTime = timeStamp + 1;
    }
}

void dbServer::requestOrder(LamportRequest r){
    for (int i = 0; i<dbQueueModify.size(); i++){
        if(this->dbQueueModify[i].time > r.time){
            this->dbQueueModify.insert(i-1, r);
            return;
        }
    }
    this->dbQueueModify.append(r);

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

QStringList dbServer::makeFrame(QStringList data, int timeStamp){
    return QStringList() << QString::number(Configuration::getInstance().myNum()) << QString::number(timeStamp) << data;
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

void dbServer::upload(LamportRequest & r, int sender){

}

void dbServer::insert(LamportRequest & r, int sender){

}

void dbServer::attach(LamportRequest & r, int sender){

}

void dbServer::deletion(LamportRequest & r, int sender){

}

void dbServer::unlink(LamportRequest & r, int sender){

}

void dbServer::okReceived(Request& r, int sender){
   int stamp = r.msg[3].toInt();
   if (responseNumMap.contains(stamp)){
        log("received OK (db operation completed) from: " + QString::number(sender) + "time stamp: " +  QString::number(stamp));
        int toReceive = responseNumMap[stamp];
        toReceive -= 1;
        if (toReceive == 0){
            QString type = r.msg[1];
            log("send client frame: " + type + ", OK");
            clientPortListener->sendFrame(clientSocketMap[stamp],makeClientFrame(type,QStringList() << "OK"));
            responseNumMap.remove(stamp);
            clientSocketMap.remove(stamp);
        }
        else{
            responseNumMap[stamp] = toReceive;
        }
   }
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

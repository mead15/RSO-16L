#include "dbserver.h"

dbServer::dbServer(int extPort, int dbPort, int clientPort){
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

    extFunctionMap[FrameType::STATUS] = &dbServer::status;
    extFunctionMap[FrameType::SERVER_STATUS_OK] = &dbServer::statusOK;
}

void dbServer::start(){
    running = true;
    extPortListener->start();
    dbPortListener->start();
    clientPortListener->start();
    while(running){
        std::cout<<"loop"<<std::endl;
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
    Request r;
    r.msg = msg;
    r.socket = socket;
    this->extQueue.append(r);
}

void dbServer::frameDBRecived(QTcpSocket*socket, QStringList msg){
    Request r;
    r.msg = msg;
    r.socket = socket;
    this->dbQueue.append(r);
}

void dbServer::frameClientRecived(QTcpSocket*socket, QStringList msg){
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
    //find num by ip
    // set as unactive
    // send state if changed
}

void dbServer::frameClientRecivedError(QString error, QString ip){

}


void dbServer::stop(){
    running = false;
}

void dbServer::log(QString what)
{
    std::cout << what.toStdString() << std::endl;
}

void dbServer::masterAction(){
    if(Configuration::getInstance().isMaster()){
        if(lastAskingTime.secsTo(QTime::currentTime()) >= Configuration::getInstance().interval() )
            askForState();
    }
    else{
        if(lastBeingAskedTime.secsTo(QTime::currentTime())> 2 * Configuration::getInstance().interval())
            startElection();
    }
}

void dbServer::startElection(){

}

void dbServer::askForState(){
    QMap<int, SServer> servers = Configuration::getInstance().getDBServers();
    for (auto i = servers.begin(); i!=servers.end(); i++){
        dbPortListener->sendFrame(QHostAddress(i.value().getIp()), i.value().getPortDB(), makeFrame(FrameType::STATUS), i.value().getPubKey());
    }
}

QStringList dbServer::getDBState(){
    QVector<int> active = Configuration::getInstance().getActiveDBServers();
    QStringList result;
    for (auto i = active.begin(); i!=active.end(); i++){
           result << QString::number(*i);
    }
    return result;
}

void dbServer::sendDBStateToAll(){
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

}

void dbServer::activeServersDB(Request& r, int sender){

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

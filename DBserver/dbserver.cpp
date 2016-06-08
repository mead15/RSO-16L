#include "dbserver.h"

dbServer::dbServer(int extPort, int dbPort, int clientPort){
    this->log("init server, exxtPort: " + QString::number(extPort) + " dbPort: " + QString::number(dbPort) + " clientPort: " + QString::number(clientPort));
    lastAskingTime = QTime::currentTime().addSecs( - Configuration::getInstance().interval() - 1);
    lastBeingAskedTime = QTime::currentTime();
    std::cout<<extPort<<" "<<dbPort<<" "<<clientPort<<std::endl;
    extPortListener = new TcpServer(extPort);
    dbPortListener = new TcpServer(dbPort);
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
    dbFunctionMapLR[FrameType::OK] = &dbServer::okReceived;
    dbFunctionMapLR[FrameType::ERROR] = &dbServer::errorReceived;
    
    extFunctionMap[FrameType::GET_ACTIVE_SERVERS_DB] = &dbServer::getActiveServersDB;

    dbh = new DBHandler("localhost", "rso", "postgres", "haslord", 5432);
}

void dbServer::start(){
    this->log("Start server");
    running = true;
    lockalTime =0;
    extPortListener->start();
    dbPortListener->start();
    clientPortListener->start();
    while(running){
        this->log("loop");
        mainLoop();
    }
}

void dbServer::mainLoop(){
    lockalTime += 1;
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
    //if ((QStringList() << FrameType::UPLOAD << FrameType::INSERT << FrameType::ATTACH << FrameType::DELETE << FrameType::UNLINK).contains(msg[2])){

    bool ok;
    msg.at(1).toInt(&ok);
    if (ok){
        int timeStamp = msg[1].toInt();
        synchronizeClock(timeStamp);
        LamportRequest r;
        msg.removeAt(1);
        r.msg = msg;
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
    QString type = r.msg[1];
    (this->*(dbFunctionMap[type]))(r, sender);
}

void dbServer::frameDBAnalyze(LamportRequest & r){
    int sender = r.msg[0].toInt();
    QString type;
    if (r.msg[2] == "OK")
        type = "OK";
    else
        type = r.msg[1];
    (this->*(dbFunctionMapLR[type]))(r, sender);
}

void dbServer::frameClientAnalyze(Request & r){
    LamportRequest rL;
    rL.msg = QStringList() << "0" << r.msg;
    rL.socket = r.socket;
    rL.time = lockalTime;
    QMap<int, SServer> servers = Configuration::getInstance().getDBServers();
    clientSocketMap[rL.time] = rL.socket;
    responseNumMap[rL.time] = servers.size() + 1; //ilość koniecznych potwiedzeń zapisu (wszystkie serwery + ja)
    requestOrder(rL);
    for (auto i = servers.begin(); i!=servers.end(); i++){
        dbPortListener->sendFrame(QHostAddress(i.value().getIp()), i.value().getPortDB(), makeFrame(r.msg, rL.time));
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

QString dbServer::logFile="/log.txt";

void dbServer::log(QString text)
{
    QFile file(QCoreApplication::applicationDirPath() + logFile);
    file.open(QIODevice::Append);
    QTextStream out(&file);
    out <<QTime::currentTime().toString()<<": "<< text <<"\n";
    file.flush();
    file.close();
    std::cout<<text.toStdString()<<std::endl;
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
        dbPortListener->sendFrame(QHostAddress(i.value().getIp()), i.value().getPortDB(), makeFrame(FrameType::STATUS));
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
        dbPortListener->sendFrame(QHostAddress(i.value().getIp()), i.value().getPortDB(), makeFrame(FrameType::ACTIVE_SERVERS_DB, state));
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
    dbPortListener->sendFrame(r.socket, makeFrame(FrameType::SERVER_STATUS_OK));
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
    bool clients = (sender == 0);
    QString tmp = r.msg[2];
    QByteArray archive = QByteArray::fromBase64(tmp.toUtf8());
    QString fname = "/ex_tmp/archive.zip";
    QFile file(fname);
    file.open(QIODevice::WriteOnly);
    file.write(archive);
    file.close();

    QProcess proc;
    proc.start("unzip", QStringList() << fname);
    if (!proc.waitForFinished() && proc.exitCode() != 0){
        sendErrorFrame(r, sender, 666);
    } else {
        file.remove();
        proc.start("psql -d dbname -f /ex_tmp/filename.sql");
        if (!proc.waitForFinished() && proc.exitCode() != 0) {
            sendErrorFrame(r, sender, 666);
        } else {
            proc.start("cp -rf /ex_tmp/data/ /ex_data/");
            if (!proc.waitForFinished() && proc.exitCode() != 0){
                sendErrorFrame(r, sender, 666);
            }
        }
    }
    proc.start("rm -rf /ex_tmp/");
    if (!proc.waitForFinished() && proc.exitCode() != 0) {
        sendErrorFrame(r, sender, 666);
    }
    if(clients)
        checkAllReceived(FrameType::UPLOAD, r.time);
    else
        dbPortListener->sendFrame(r.socket, makeFrame(QStringList() << FrameType::UPLOAD << "OK" << QString::number(r.time), lockalTime));
}

void dbServer::insert(LamportRequest & r, int sender){
    bool clients = (sender == 0);
    QString table = r.msg[2];
    QString error;
    QVector<QStringList> result;
    int index = 3;
    QStringList values;
    for (int i = index; i < r.msg.size(); i++){
       if (r.msg[i] == "NULL" || r.msg[i] == "null")
           values << r.msg[i];
       else
           values << "'"+r.msg[i]+"'";
    }
    if (table == "patient") {
        if(dbh->openDB()){
            if (!dbh->execQuery("INSERT INTO patient(id, pesel, name, last_name, tel, sex, race, birth_date, birth_city, country) VALUES ("+values.join(',')+");",
                            result, error))
                sendErrorFrame(r, sender, 666);
            dbh->closeDB();
        }
    } else if (table == "examination") {
        if(dbh->openDB()){
            if (!dbh->execQuery("INSERT INTO examination(id, patient_id, name, result_path, date) VALUES ("+values.join(',')+");",
                            result, error))
                sendErrorFrame(r,sender, 666);
            dbh->closeDB();
        }
    } else {
        sendErrorFrame(r, sender, 666);
    }
    if(clients)
        checkAllReceived(FrameType::INSERT, r.time);
    else
        dbPortListener->sendFrame(r.socket, makeFrame(QStringList() << FrameType::INSERT << "OK" << QString::number(r.time), lockalTime));
}

void dbServer::attach(LamportRequest & r, int sender){
    bool clients = (sender == 0);
    QString fname = r.msg[2];
    QRegExp rx("*.bmp|*.xml");
    if (rx.exactMatch(fname)){
        QByteArray content = QByteArray::fromBase64(r.msg[2].toUtf8());
        QFile file(fname);
        file.open(QIODevice::WriteOnly);
        file.write(content);
        file.close();
        if(clients)
            checkAllReceived(FrameType::ATTACH, r.time);
        else
            dbPortListener->sendFrame(r.socket, makeFrame(QStringList() << FrameType::ATTACH << "OK" << QString::number(r.time), lockalTime));
    }else {
        sendErrorFrame(r, sender, 666);
    }
}

void dbServer::deletion(LamportRequest & r, int sender){
    bool clients = (sender==0);
    QString table = r.msg[2];
    QString id = clients ? r.msg[3] : r.msg[4];
    QString error;
    QVector<QStringList> result;
    if (dbh->openDB()){
        if (dbh->execQuery("DELETE FROM "+table+"WHERE ID = "+id+";", result, error)){
            if (clients)
                checkAllReceived(FrameType::DELETE, r.time);
            else
                dbPortListener->sendFrame(r.socket, makeFrame(QStringList() << FrameType::DELETE << "OK" << QString::number(r.time), lockalTime));
        } else
            sendErrorFrame(r, sender,666);
    }
}

void dbServer::unlink(LamportRequest & r, int sender){
    bool clients = (sender == 0);
    QString fileName = r.msg[2];
    QFile file(fileName);
    if (file.remove()) {
        // update db?
        if (clients)
            checkAllReceived(FrameType::UNLINK, r.time);
        else
            dbPortListener->sendFrame(r.socket, makeFrame(QStringList() << FrameType::UNLINK << "OK" << QString::number(r.time), lockalTime));
    } else {
        sendErrorFrame(r, sender, 666);
    }
}

void dbServer::okReceived(LamportRequest& r, int sender){
   int stamp = r.msg[3].toInt();
   if (responseNumMap.contains(stamp)){
        log("received OK (db operation completed) from: " + QString::number(sender) + "time stamp: " +  QString::number(stamp));
        checkAllReceived(r.msg[1], stamp);
   }
}

void dbServer::errorReceived(LamportRequest& r, int sender){
    int stamp = r.msg[3].toInt();
    responseNumMap.remove(stamp);
    clientSocketMap.remove(stamp);
    log("send client frame: " + FrameType::ERROR);
    clientPortListener->sendFrame(clientSocketMap[stamp],makeClientFrame(FrameType::ERROR, QStringList() << r.msg[3]));
    QMap<int, SServer> servers = Configuration::getInstance().getDBServers();
    frameDBRecivedError(r.msg[3], servers[sender].getIp());
}

void dbServer::checkAllReceived(QString frameType, int stamp){
    int toReceive = responseNumMap[stamp];
    toReceive -= 1;
    if (toReceive == 0){
        log("send client frame: " + frameType + ", OK");
        clientPortListener->sendFrame(clientSocketMap[stamp],makeClientFrame(frameType,QStringList() << "OK"));
        responseNumMap.remove(stamp);
        clientSocketMap.remove(stamp);
    }
    else{
        responseNumMap[stamp] = toReceive;
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
    extPortListener->sendFrame(r.socket, makeFrame(FrameType::ACTIVE_SERVERS_DB, result));
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
    QString exName = r.msg[3];
    QVector<QStringList> result;
    QString error;
    if (dbh->openDB()){
        if (exName == "*"){
            if (dbh->execQuery("SELECT e.id, e.name, p.country, p.sex, p.race, DATE_PART('year', current_date) - DATE_PART('year', p.birth_date) as age FROM examination e JOIN patient p on e.patient_id = p.id;",
                          result, error)){
                QStringList frameResult;
                frameResult << r.msg[2] << QString(result.size());
                for (int i =0; i < result.size(); i++){
                    frameResult << result.at(i);
                }
                extPortListener->sendFrame(r.socket, makeFrame(FrameType::GET_AVAILABLE_RESULTS, frameResult));
            } else {
                sendErrorFrame(r, sender, 666);
            }
        } else {
            QStringList whereList;
            if (r.msg[3] != "*")
                whereList << "p.country = '"+r.msg[3]+"'";
            if (r.msg[4] != "*")
                whereList << "p.sex = '"+r.msg[4]+"'";
            if (r.msg[5] != "*")
                whereList << "p.race = '"+r.msg[5]+"'";
            if (r.msg[6] != "*")
                whereList << "DATE_PART('year', current_date) - DATE_PART('year', p.birth_date) > "+r.msg[6];
            if (r.msg[7] != "*")
                whereList << "DATE_PART('year', current_date) - DATE_PART('year', p.birth_date) < "+r.msg[7];
            QString where;
            if(whereList.size() > 0)
                where = " WHERE " + whereList.join(" AND ");
            if (dbh->execQuery("SELECT e.id, e.name, p.country, p.sex, p.race, DATE_PART('year', current_date) - DATE_PART('year', birth_date) as age FROM examination e JOIN patient p on e.patient_id = p.id"+where+";",
                            result, error)){
                QStringList frameResult;
                frameResult << r.msg[2] << QString(result.size());
                for (int i =0; i < result.size(); i++){
                    frameResult << result.at(i);
                }
                extPortListener->sendFrame(r.socket, makeFrame(FrameType::GET_AVAILABLE_RESULTS, frameResult));
            } else {
                sendErrorFrame(r, sender, 666);
            }
        }
        dbh->closeDB();
    } else {
        sendErrorFrame(r, sender, 666);
    }
}

void dbServer::getResult(Request& r, int sender){
    QString exId = r.msg[3];
    if(dbh->openDB()){
        QVector<QStringList> result;
        QString error;
        if (dbh->execQuery("SELECT result_path FROM examination WHERE id = "+exId+";", result, error)){
            QFile file(result.at(0).at(0));
            QRegExp rx("*.xml");
            bool isXml = rx.exactMatch(result.at(0).at(0));
            QString size = QString::number(file.size());
            int lastSlash = result.at(0).at(0).lastIndexOf("/");
            QStringList frameResult;
            QByteArray fileContent = file.readAll();
            if (isXml){
                QDomDocument xml;
                xml.setContent(fileContent);
                QDomNodeList nodes = xml.elementsByTagName("last_name");
                for (int i = 0; i < nodes.count(); ++i)
                {
                    QDomNode node = nodes.at(i);
                    QDomElement child = node.firstChildElement("child");
                    if (!child.isNull())
                        node.removeChild(child);
                }
                fileContent = xml.toByteArray();
            }
            frameResult << r.msg[2] <<result.at(0).at(0).left(lastSlash) << size << fileContent.toBase64();
            extPortListener->sendFrame(r.socket, makeFrame(FrameType::RESULT, frameResult));
        }
        dbh->closeDB();
    } else {
        sendErrorFrame(r, sender, 666);
    }
}

void dbServer::getStatistics(Request& r, int sender){
    QStringList whereList;
    if (r.msg[3] != "*")
        whereList << "e.name = '"+r.msg[3]+"'";
    if (r.msg[4] != "*")
        whereList << "e.date > "+r.msg[4];
    if (r.msg[5] != "*")
        whereList << "e.date < "+r.msg[5];
    if (r.msg[6] != "*")
        whereList << "p.country = '"+r.msg[6]+"'";
    if (r.msg[7] != "*")
        whereList << "p.sex = '"+r.msg[7]+"'";
    if (r.msg[8] != "*")
        whereList << "p.race = '"+r.msg[8]+"'";
    if (r.msg[9] != "*")
        whereList << "DATE_PART('year', current_date) - DATE_PART('year', p.birth_date) > "+r.msg[9];
    if (r.msg[10] != "*")
        whereList << "DATE_PART('year', current_date) - DATE_PART('year', p.birth_date) < "+r.msg[10];
    QString where;
    if(whereList.size() > 0)
        where = " WHERE " + whereList.join(" AND ");
    QStringList groups;
    if (r.msg.size()>11){
        for (int i=11; i<r.msg.size();i++)
           groups << r.msg[i];
    }
    QString groupBy("");
    QString selectGroups("");
    if (dbh->openDB()){
        QVector<QStringList> result;
        QString error;
        if(groups.size() > 0){
            groupBy = " GROUP BY "+groups.join(',');
            selectGroups = groups.join(',')+",";
        }
        if (dbh->execQuery("SELECT "+selectGroups+" COUNT(*) FROM examination e JOIN patient p on e.patient_id = p.id"+where+groupBy+";",
                           result, error)){
            QStringList frameResult;
            frameResult << r.msg[2];
            frameResult << QString(result.size());
            for (int i =0; i < result.size(); i++){
                frameResult << result.at(i);
            }
            extPortListener->sendFrame(r.socket, makeFrame(FrameType::GET_STATISTICS, frameResult));
        }
        dbh->closeDB();
    } else {
        sendErrorFrame(r, sender, 666);
    }
}


void dbServer::sendErrorFrame(Request& r, int sender, int errorCode){
        extPortListener->sendFrame(r.socket, makeFrame(FrameType::ERROR, QStringList() << r.msg[2] << QString(errorCode) ));
}

void dbServer::sendErrorFrame(LamportRequest& r, int sender, int errorCode){
    if (sender == 0){
        log("send client frame: " + FrameType::ERROR);
        clientPortListener->sendFrame(clientSocketMap[r.time],makeClientFrame(FrameType::ERROR, QStringList() << QString(errorCode)));
        responseNumMap.remove(r.time);
        clientSocketMap.remove(r.time);
    } else {
        dbPortListener->sendFrame(r.socket, makeFrame(QStringList() << FrameType::ERROR << QString(errorCode) << QString::number(r.time),lockalTime));
    }
}


#include "dbhandler.h"

DBHandler::DBHandler(){

	// @TODO wyciaganie z pliku konfiguracyjnego ?
    db = QSqlDatabase::addDatabase("QPSQL");
    db.setHostName("192.168.0.40");
    db.setDatabaseName("rso");
    db.setUserName("postgres");
    db.setPassword("haslord");
    db.setPort(5432);
}

DBHandler::DBHandler(QString host, QString dbname, QString user, QString password, int port){
    db = QSqlDatabase::addDatabase("QPSQL");
    db.setHostName(host);
    db.setDatabaseName(dbname);
    db.setUserName(user);
    db.setPassword(password);
    db.setPort(port);
}

/*
	param:
		q 		- zapytanie do bazy
		result 	- referencja na wektor QStringList do wpisania wyniku (dotychczasowa zawartosc wektora jest usuwana)
		error 	- referencja na QString do wpisania opisu bledu (dotychczasowa zawartosc jest usuwana)
	return:
		true	- gdy zapytanie wykona sie poprawnie - uzupelniany jest wtedy wektor podawany jako parametr
		false	- gdy zapytanie zwraca blad - uzupelniany jest wtedy QString error podawany jako parametr
*/
bool DBHandler::execQuery(QString q, QVector<QStringList> &result, QString &error){
    bool ret = false;
    if(!db.isOpen()){
        error.clear();
        error = "Database not open";
    } else {
        QSqlQuery query = db.exec(q);
        QSqlError e = query.lastError();
        if (e.type() != 0){
            error.clear();
            error = e.text();
        } else {
            result.clear();
            int i = 0;
            while (query.next()){
                int cols = query.record().count();
                QStringList tmp;
                for (int j = 0; j < cols; j++)
                    tmp.append(query.value(j).toString());
                result.append(tmp);
                i++;
            }
            ret = true;
        }
    }
    return ret;
}

bool DBHandler::openDB(){
    return db.open();
}

void DBHandler::closeDB(){
    db.close();
}

bool DBHandler::transaction(){
    return db.transaction();
}

bool DBHandler::commit(){
    return db.commit();
}

#include <QCoreApplication>
#include <QTextStream>
#include <iostream>

#include "DBHandler/dbhandler.h"


int main(int argc, char *argv[]){
    QTextStream qout(stdout);

    //DBHandler dbh();
    DBHandler dbh("192.168.1.3", "rso", "postgres", "haslord", 5432);

    QVector<QStringList> queryResult;
    QString error;

    if (dbh.openDB()) {
        dbh.transaction();
        if (dbh.execQuery("SELECT COUNT(sex), sex FROM patient WHERE sex = 'm' GROUP BY sex", queryResult, error)){
            for (int i=0; i < queryResult.size(); i++){
                QStringList row = queryResult.at(i);
                for (int j=0; j < row.size(); j++) {
                    qout << row.at(j) << " ";
                }
                qout << "\n";
            }
        } else {
            qout << error;
            return -1;
        }
        dbh.commit();
    } else {
        qout << "Can't open database!";
        return -1;
    }
    dbh.closeDB();
    return 0;
}

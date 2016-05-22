#ifndef DBHANDLER_H
#define DBHANDLER_H

#include <QSqlDatabase>
#include <QSqlQuery>
#include <QStringList>
#include <QVariant>
#include <QDebug>
#include <QSqlError>
#include <QSqlRecord>
#include <QVector>

class DBHandler
{
public:
    DBHandler();
    DBHandler(QString host, QString dbname, QString user, QString password, int port);
    bool fetch(QString q, QVector<QStringList> & result, QString & error);
    bool openDB();
    void closeDB();
    bool transaction();
    bool commit();
private:
    QSqlDatabase db;
};


#endif // DBHANDLER_H


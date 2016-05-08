#ifndef SETTINGS_H
#define SETTINGS_H
#include <QCoreApplication>
#include <QObject>
#include <QSettings>
class Settings : public QSettings
{
    Q_OBJECT
public:
    explicit Settings(QString& path);
private:
    QString settingsFilePath ;
signals:

public slots:
};

#endif // SETTINGS_H

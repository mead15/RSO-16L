#ifndef DAEMON_H
#define DAEMON_H

#include <QObject>

class Daemon : public QObject
{
    Q_OBJECT
public:
    explicit Daemon(QObject *parent = 0);

signals:

public slots:
};

#endif // DAEMON_H
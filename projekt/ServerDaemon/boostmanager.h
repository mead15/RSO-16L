#ifndef BOOSTMANAGER_H
#define BOOSTMANAGER_H

#include <QObject>
#include <qthread.h>
#include <filebooster.h>
#include <callbacks/fileboostcallback.h>
#include <remote/remoteprocess.h>
class BoostManager : public QThread
{
    Q_OBJECT
public:
   explicit BoostManager(int nodeCount, QObject *parent = 0);
   ~BoostManager();
   void run();
   FileBooster** boosters;
   int* status;//0 - initialized, 1 - success, 2 - fail, 3 - not initialized (loopback?)";
               //0 - status poczatkowy (nie ma znaczenia), 1 - pomyslnie wyslany, 2 - blad podczas wysylania (np. niedostepny), 3 - nie ma Fileboostera (localhost)
   RemoteProcess ** processes;
   void sendCommands();

signals:

public slots:

private:
    int nodeCount;
    bool filesSended;

};

#endif // BOOSTMANAGER_H

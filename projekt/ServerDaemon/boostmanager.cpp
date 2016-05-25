#include "boostmanager.h"

BoostManager::BoostManager(int nodeCount,  QObject*parent) : QThread(parent)
{    

    this->nodeCount = nodeCount;
    this->boosters = new FileBooster * [nodeCount];
    this->processes = new RemoteProcess * [nodeCount];
    this->status = new int [nodeCount];
    for(int i =0;i<nodeCount;i++)
    {
        boosters[i] = new FileBooster();
        processes[i] = new RemoteProcess();
    }
    filesSended = false;
}

BoostManager::~BoostManager()
{
    for(int i =0; i< nodeCount;i++)
    {
       delete boosters[i];
       delete processes[i];
    }
    delete [] status;
    delete [] boosters;
    delete [] processes;
}


void BoostManager::run()
{
    qDebug () << "running " << nodeCount;

    if(!filesSended)
    {
        for (int i =0; i< nodeCount; i++)
        {
            if(boosters[i]->hasInitializedParams())
            {
                boosters[i]->upload();
            }
            else
            {
                status[i] = 3;
            }
            //boosters[i]->moveToThread((QThread *)parent());
        }
    }

    filesSended = true;
    exec();
}

void BoostManager::sendCommands()
{
    if(filesSended)
    {
        for(int i = 0; i < nodeCount; i++)
        {
            if(this->status[i] == 1)
            {
                this->processes[i]->run();
            }
        }
    }
}

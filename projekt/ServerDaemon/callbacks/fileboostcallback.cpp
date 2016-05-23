#include "fileboostcallback.h"
#include "QDebug"
#include "remote/remoteprocess.h"
#include "sshconnection.h"
FileBoostCallback::FileBoostCallback()
{
}

void FileBoostCallback::OnSuccess(const QSsh::SshConnectionParameters &params)
{

    qDebug() << "Success";
    static RemoteProcess remoteProcess(params);
    QString action("/home/");
    action.append(params.userName).append("/server_daemon/ServerDaemon slave");
    qDebug() << action;
    remoteProcess.run(action);
}

void FileBoostCallback::OnFailure()
{
    qDebug() << "Failure";
}


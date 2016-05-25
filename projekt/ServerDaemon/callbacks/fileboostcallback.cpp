#include "fileboostcallback.h"
#include "QDebug"
#include "remote/remoteprocess.h"
#include "sshconnection.h"

FileBoostCallback::FileBoostCallback()
{}
FileBoostCallback::FileBoostCallback(int &status)
{
    this->status = &status;
}

void FileBoostCallback::OnSuccess(const QSsh::SshConnectionParameters &params)
{
   *status = 1;
    qDebug() << "Success";
}

void FileBoostCallback::OnFailure(QString error)
{
    *status = 2;
    qDebug() << "Failure: " << error;
}


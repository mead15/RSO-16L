#ifndef FILEBOOSTCALLBACK_H
#define FILEBOOSTCALLBACK_H
#include "sshconnection.h"

class FileBoostCallback
{
public:
    FileBoostCallback();
    FileBoostCallback(int &status);
    void OnSuccess(const QSsh::SshConnectionParameters &params);
    void OnFailure(QString error);
private:
    int* status;
//    FileBoostCallback(const FileBoostCallback& that);
};


#endif // FILEBOOSTCALLBACK_H

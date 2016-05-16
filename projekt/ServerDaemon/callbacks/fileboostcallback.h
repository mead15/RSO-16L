#ifndef FILEBOOSTCALLBACK_H
#define FILEBOOSTCALLBACK_H
#include "sshconnection.h"

class FileBoostCallback
{
public:
    FileBoostCallback();
    void OnSuccess(const QSsh::SshConnectionParameters &params);
    void OnFailure();
private:
    FileBoostCallback(const FileBoostCallback& that);
};


#endif // FILEBOOSTCALLBACK_H

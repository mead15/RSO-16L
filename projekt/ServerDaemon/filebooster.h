
#ifndef FILEBOOSTER_H
#define FILEBOOSTER_H

#include <QObject>

#include "sftpchannel.h"
#include "sshconnection.h"
#include "callbacks/fileboostcallback.h"
/// Very simple example to upload a file using FTPS
class FileBooster : public QObject
{
    Q_OBJECT
public:
    FileBooster(QObject *parent=0);
    explicit FileBooster(QSsh::SshConnectionParameters params, QString local, QString remote, QObject *parent = 0);

    /// Uploads \a localFile to \a username@host:/dest using password \a passwd
    void upload();
    void setCallBack(FileBoostCallback cb);
    static bool ready;
    bool hasInitializedParams();

signals:

private slots:
    void onConnected();
    void onConnectionError(QSsh::SshError);
    void onChannelInitialized();
    void onChannelError(const QString &err);
    void onOpfinished(QSsh::SftpJobId job, const QString & error = QString());


private:
    QSsh::SshConnectionParameters SSHParams;
    FileBoostCallback callback;
    QString m_localFilename;
    QString m_remoteFilename;
    QSsh::SftpChannel::Ptr m_channel;
    QSsh::SshConnection *m_connection;
    QString localPath;
    QString remotePath;
    void parseDestination(const QString &dest);
    bool hasParams;
};

#endif // FILEBOOSTER_H

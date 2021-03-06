/**************************************************************************
**
** This file is part of QSsh
**
** Copyright (c) 2012 LVK
**
** Contact: andres.pagliano@lvklabs.com
**
** GNU Lesser General Public License Usage
**
** This file may be used under the terms of the GNU Lesser General Public
** License version 2.1 as published by the Free Software Foundation and
** appearing in the file LICENSE.LGPL included in the packaging of this file.
** Please review the following information to ensure the GNU Lesser General
** Public License version 2.1 requirements will be met:
** http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
**************************************************************************/

#include "filebooster.h"

#include <QtDebug>
#include <QFileInfo>

FileBooster::FileBooster(QObject *parent): QObject(parent)
{
    hasParams = false;
}


FileBooster::FileBooster(QSsh::SshConnectionParameters params, QString local, QString remote, QObject *parent) :
    QObject(parent), m_connection(0)
{
    localPath = local;
    remotePath = remote;
    SSHParams = params;
    hasParams = true;
}

bool FileBooster::ready = false;

void FileBooster::upload()
{

    QFileInfo info(localPath);
    m_localFilename = info.canonicalFilePath();
    m_remoteFilename = remotePath + "/" + info.fileName();
    m_connection = new QSsh::SshConnection(SSHParams); // TODO free this pointer!
    connect(m_connection, SIGNAL(connected()), SLOT(onConnected()));
    connect(m_connection, SIGNAL(error(QSsh::SshError)), SLOT(onConnectionError(QSsh::SshError)));

    qDebug() << "SecureUploader: Connecting to host" << SSHParams.host;//host;

    m_connection->connectToHost();
}

void FileBooster::onConnected()
{
    qDebug() << "SecureUploader: Connected";
    qDebug() << "SecureUploader: Creating SFTP channel...";

    m_channel = m_connection->createSftpChannel();

    if (m_channel) {
        connect(m_channel.data(), SIGNAL(initialized()),
                SLOT(onChannelInitialized()));
        connect(m_channel.data(), SIGNAL(initializationFailed(QString)),
                SLOT(onChannelError(QString)));
        connect(m_channel.data(), SIGNAL(finished(QSsh::SftpJobId, QString)),
                SLOT(onOpfinished(QSsh::SftpJobId, QString)));

        m_channel->initialize();

    } else {
        qDebug() << "SecureUploader: Error null channel";
    }
}

void FileBooster::onConnectionError(QSsh::SshError err)
{
    qDebug() << "SecureUploader: Connection error" << err;
    callback.OnFailure(QString(err));
}

void FileBooster::onChannelInitialized()
{
    qDebug() << "SecureUploader: Channel Initialized";

    // TODO Choose the overwrite mode: SftpOverwriteExisting, SftpAppendToExisting, SftpSkipExisting
    QSsh::SftpJobId job = m_channel->uploadFile(m_localFilename, m_remoteFilename,
                                                QSsh::SftpOverwriteExisting);

    if (job != QSsh::SftpInvalidJob) {
        qDebug() << "SecureUploader: Starting job #" << job;
    } else {
        qDebug() << "SecureUploader: Invalid Job";
    }
}

void FileBooster::onChannelError(const QString &err)
{
    qDebug() << "SecureUploader: Error: " << err;
    FileBooster::ready = true;
}

void FileBooster::onOpfinished(QSsh::SftpJobId job, const QString &err)
{
    qDebug() << "SecureUploader: Finished job #" << job << ":" << (err.isEmpty() ? "OK" : err);
    if(err.isEmpty())
    {
        callback.OnSuccess(SSHParams);
    }
    FileBooster::ready = true;
}

void FileBooster::setCallBack(FileBoostCallback cb)
{
    callback = cb;
}

bool FileBooster::hasInitializedParams()
{
    return hasParams;
}


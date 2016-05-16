#include <QCoreApplication>
#include <QDebug>
#include <QProcess>
#include <sys/types.h>
#include <sys/stat.h>
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <errno.h>
#include <unistd.h>
#include <syslog.h>
#include <string.h>
#include <settings.h>
#include <filebooster.h>
#include <iostream>>
#include <QFuture>
#include <QtConcurrent>
#include <QNetworkInterface>
#include <callbacks/fileboostcallback.h>
#include "sshconnection.h"
#define DAEMON_NAME "server_daemon"


void showSyntax();
QString getPassword();
extern void upload(const QString &orig, const QString &dest, const QString passwd);

extern void process(QString sPath){
    Settings settings(sPath);

    QString filename="/home/logs.txt";
    QFile file( filename );

    while(true)
    {
        if ( file.open(QIODevice::ReadWrite) )
        {
            QTextStream stream( &file );
            stream << "server_daemon" << endl;
        }
        QStringList s = settings.allKeys();

        syslog(LOG_NOTICE,"xxx");
        foreach (QString str, s) {
            syslog(LOG_NOTICE, "yyy");
            str.append(" - printed arg");
            syslog(LOG_NOTICE, "server_daemon");
        }
        sleep(10);
    }
}

QSsh::SshConnectionParameters getSSHConnectionParams(QString host,QString username,QString password)
{
    QSsh::SshConnectionParameters params;
    params.host = host;
    params.userName = username;
    params.password = password;
    params.authenticationType = QSsh::SshConnectionParameters::AuthenticationByPassword;
    params.timeout = 3000;
    params.port = 22;
    return params;
}

QString getLocalAddress()
{
    QList<QNetworkInterface> interfaces = QNetworkInterface::allInterfaces();
    for(int i=0; i<interfaces.count(); i++)
    {
        QList<QNetworkAddressEntry> entries = interfaces.at(i).addressEntries();
        for(int j=0; j<entries.count(); j++)
        {
            if(entries.at(j).ip().protocol() == QAbstractSocket::IPv4Protocol)
            {
                if (entries.at(j).ip().toString()!="127.0.0.1")
                {
                    return entries.at(j).ip().toString();

                }
                //qDebug() << entries.at(j).netmask().toString();
            }
        }
    }
    return NULL;
}

int main(int argc, char *argv[])
{
    QCoreApplication a (argc,argv);


    QStringList args = a.arguments();
    if (args.count() != 2)
    {

        std::cerr << "argument required" << endl << endl;
        return 1;
    }

    QString arg = args[1];

    if(arg != "master" && arg != "slave")
    {
        std::cerr << "invalid argument" << endl << endl;
        return 1;
    }

    bool propagate = arg == "master";

    QString localHostAddress = getLocalAddress();
    qDebug () << "Localhost: " <<localHostAddress;

    QString sPath = QCoreApplication::applicationDirPath() + "/settings.ini";
    //Settings object, based on QSettings. Contains variaables from .ini file

    Settings settings(sPath);
    QStringList childGroups = settings.childGroups();

    if(propagate)
    {
        for (int i = 0; i< childGroups.count(); i++)
        {
            QString group  = childGroups.at(i);
            //qDebug() << group;
            settings.beginGroup(group);

            QStringList childKeys = settings.childKeys();

            QString ip (settings.value("ip").toString());

            if(ip != localHostAddress)
            {
                QString name (settings.value("name").toString());
                QString password (settings.value("password").toString());
                QString user (settings.value("user").toString());
                QString userCp (user);
                QString dest = user.append("@").append(ip).append(":/home/").append(userCp).append("/rso");
                upload(sPath, dest, password);
            }

            settings.endGroup();
        }
    }

    setlogmask(LOG_UPTO(LOG_NOTICE));
    openlog(DAEMON_NAME, LOG_CONS | LOG_NDELAY | LOG_PERROR | LOG_PID, LOG_USER);

    syslog(LOG_INFO, "Entering Daemon");

    pid_t pid, sid;

   //Fork the Parent Process
    pid = fork();

    if (pid < 0) { exit(EXIT_FAILURE); }

    //We got a good pid, Close the Parent Process
    if (pid > 0) { exit(EXIT_SUCCESS); }

    //Change File Mask
    umask(0);

    //Create a new Signature Id for our child
    sid = setsid();
    if (sid < 0) { exit(EXIT_FAILURE); }

    //Change Directory
    //If we cant find the directory we exit with failure.
    if ((chdir("/")) < 0) { exit(EXIT_FAILURE); }

    //Close Standard File Descriptors
//    close(STDIN_FILENO);
//    close(STDOUT_FILENO);
//    close(STDERR_FILENO);




    //Default path of conf file (app path)



    QFuture<void> future = QtConcurrent::run(process, sPath);



    //----------------
    //Daemon loop
    //----------------
//    while(true){

//        process(settings);    //Run our Process
//        sleep(10);    //Sleep for 10 seconds
//    }






    //Close the log
    closelog ();

    return a.exec();
}




void showSyntax()
{
    std::cerr << "Syntax: " << std::endl;
    std::cerr << "         SecureUploader local_file username@host:/destination_path" << std::endl;
    exit(1);
}


QString getPassword()
{
    std::cout << "Password (Warning password will be echoed): ";
    std::string passwd;
    std::cin >> passwd;

    return QString::fromStdString(passwd).trimmed();
}


extern void upload(const QString &orig, const QString &dest, const QString passwd)
{
    // Parse destination with format "username@host:/destination"

    QStringList l1 = dest.split("@");

    if (l1.size() == 2) {
        QStringList l2 = l1[1].split(":");

        if (l2.size() == 2) {
            QSsh::SshConnectionParameters params = getSSHConnectionParams( l2[0], l1[0], passwd);
            static FileBooster uploader(params);
            FileBoostCallback cb;

            uploader.setCallBack(cb);
            uploader.upload(orig, l2[1]);
        } else {
            qDebug() << "SecureUploader:  Error invalid parameter " ;//<< dest.toStdString() << std::endl;
            showSyntax();
        }
    } else {
        qDebug() << "SecureUploader:  Error invalid parameter ";// << dest.toStdString() << std::endl;
        showSyntax();
    }
}

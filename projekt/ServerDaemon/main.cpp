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
#include <boostmanager.h>
#include <remote/remoteprocess.h>
#define DAEMON_NAME "server_daemon"


void showSyntax();
QString getPassword();




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
void initManager(QString orig, QString dest, QString passwd,FileBooster* (&uploader),RemoteProcess* (&process), int *status)
{
    // Parse destination with format "username@host:/destination"

    QStringList l1 = dest.split("@");

    if (l1.size() == 2) {
        QStringList l2 = l1[1].split(":");

        if (l2.size() == 2) {
            QSsh::SshConnectionParameters params = getSSHConnectionParams( l2[0], l1[0], passwd);
            uploader = new FileBooster(params,orig, l2[1]);

            QString action ("/home/");
            action.append(params.userName).append("/server_daemon/ServerDaemon slave");
            qDebug() << action;
            process = new RemoteProcess(params,action);

            FileBoostCallback cb(*status);
            uploader->setCallBack(cb);

        } else {
            qDebug() << "SecureUploader:  Error invalid parameter " ;//<< dest.toStdString() << std::endl;
            showSyntax();
        }
    } else {
        qDebug() << "SecureUploader:  Error invalid parameter ";// << dest.toStdString() << std::endl;
        showSyntax();
    }
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

        qDebug() << "argument required";
        exit(0);
    }

    QString arg = args[1];

    if(arg != "master" && arg != "slave")
    {
        qDebug() << "invalid argument";
        exit(0);
    }

    bool propagate = arg == "master";

    QString localHostAddress = getLocalAddress();
    //qDebug () << "Localhost: " <<localHostAddress;

    QString sPath = QCoreApplication::applicationDirPath() + "/settings.ini";


    QString logPath = QCoreApplication::applicationDirPath() + "log.txt";
    QLockFile lock (logPath);

    if(!lock.tryLock())
    {
        qDebug() << "App is runnging";
        exit(0);
    }

    Settings settings(sPath);
    QStringList childGroups = settings.childGroups();
    BoostManager manager (childGroups.count(),QThread::currentThread());


//        FileBooster* boosters [childGroups.count()];
//        int status [childGroups.count()]; //0 - initialized, 1 - success, 2 - fail, 3 - not initialized (loopback?)

    for (int i = 0; i< childGroups.count(); i++)
    {
        manager.status[i] = 0;
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
            initManager(sPath, dest, password,manager.boosters[i],manager.processes[i],&manager.status[i]);
        }

        settings.endGroup();
    }

    //loop in main trhead, must be
    for(int i = 0; i<childGroups.count();i ++)
    {
        manager.boosters[i]->moveToThread(&manager);
        //manager.processes[i]->moveToThread(&manager);
    }

    if(propagate)
    {
        manager.start();
        while(true)
        {
            bool breakLoop = true;
            for(int i =0; i < childGroups.count();i++)
            {
                if(manager.status[i] == 0)
                    breakLoop = false;
                qDebug() << "status_" << i <<": " << manager.status[i] << "//0 - initialized, 1 - success, 2 - fail, 3 - not initialized (loopback?)";
                //0 - status poczatkowy (nie ma znaczenia), 1 - pomyslnie wyslany, 2 - blad podczas wysylania (np. niedostepny), 3 - nie ma Fileboostera (localhost)
            }
            if(breakLoop)
                break;
            sleep(10);
        }
        manager.quit();
        while(!manager.isFinished())
        {
            qDebug()<<"Waiting for another thread...";
            sleep(5);
            //
        }


        manager.sendCommands();
    }


//    setlogmask(LOG_UPTO(LOG_NOTICE));
//    openlog(DAEMON_NAME, LOG_CONS | LOG_NDELAY | LOG_PERROR | LOG_PID, LOG_USER);

//    syslog(LOG_INFO, "Entering Daemon");

//    pid_t pid, sid;

//   //Fork the Parent Process
//    pid = fork();

//    if (pid < 0) { exit(EXIT_FAILURE); }

//    //We got a good pid, Close the Parent Process
//    if (pid > 0) { exit(EXIT_SUCCESS); }

//    //Change File Mask
//    umask(0);

//    //Create a new Signature Id for our child
//    sid = setsid();
//    if (sid < 0) { exit(EXIT_FAILURE); }

//    //Change Directory
//    //If we cant find the directory we exit with failure.
//    if ((chdir("/")) < 0) { exit(EXIT_FAILURE); }

    //Close Standard File Descriptors
//    close(STDIN_FILENO);
//    close(STDOUT_FILENO);
//    close(STDERR_FILENO);


//    QFuture<void> future = QtConcurrent::run(process, sPath);
 //   closelog ();

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



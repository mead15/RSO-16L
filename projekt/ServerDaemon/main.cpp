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
#include <securefileuploader.h>
#include <iostream>>
#include <QFuture>
#include <QtConcurrent>
#define DAEMON_NAME "server_daemon"


void showSyntax();
QString getPassword();
extern void upload(const QString &orig, const QString &dest, const QString passwd);

extern void process(QString sPath){
    Settings settings(sPath);
    while(true)
    {
        QStringList s = settings.allKeys();

        syslog(LOG_NOTICE,"xxx");
        foreach (QString str, s) {
            syslog(LOG_NOTICE, "yyy");
            str.append(" - printed arg");
            syslog(LOG_NOTICE, str.toUtf8().constData());
        }
        sleep(10);
    }
}

int main(int argc, char *argv[])
{
    QCoreApplication a (argc,argv);
    qDebug () << "works";
    //Default path of conf file (app path)
    QString sPath = QCoreApplication::applicationDirPath() + "/settings.ini";
    //Settings object, based on QSettings. Contains variaables from .ini file



    //Set our Logging Mask and open the Log
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


    QString orig = sPath;
    QString dest = "osmc@192.168.0.10:/home/osmc/test";
    QString passwd = "osmc";
    upload(orig, dest, passwd);


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
            static SecureFileUploader uploader;
            uploader.upload(orig, l2[1], l2[0], l1[0], passwd);
        } else {
            qDebug() << "SecureUploader:  Error invalid parameter " ;//<< dest.toStdString() << std::endl;
            showSyntax();
        }
    } else {
        qDebug() << "SecureUploader:  Error invalid parameter ";// << dest.toStdString() << std::endl;
        showSyntax();
    }
}

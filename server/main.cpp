#include <QCoreApplication>
#include <QString>
#include <iostream>
#include <configuration.h>
#include <../communication/rsaencrypter.h>
#include "extserver.h"


int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);
    //QString configPath = "settings.ini";
    QString configPath = QCoreApplication::applicationDirPath() + "/settings.ini";
    QString privKeyPath = "privKey.pem";
    QStringList args = a.arguments();
    if(args.count() != 2)
    {
        std::cout << "Argument required!!!" << std::endl;
        exit(0);
    }
    int num = args.at(1).toInt();
    //if(argc>3){
        //int num = atoi(argv[1]);
        //int master = atoi(argv[2]);
        //int num = 3;
        //int master = 1;
        // Load Configuration
        Configuration::getInstance().loadConfig(configPath);
        Configuration::getInstance().setMyNum(num);
        //Configuration::getInstance().setMaster(master);
        Configuration::getInstance().setServerActive(num, true);

        // Set ny private key
        RsaEncrypter::setPrivateKey("-----BEGIN RSA PRIVATE KEY-----\n"
                                    "MIICXAIBAAKBgQCp2w+8HUdECo8V5yuKYrWJmUbLtD6nSyVifN543axXvNSFzQfW\n"
                                    "NOGVkMsCo6W4hpl5eHv1p9Hqdcf/ZYQDWCK726u6hsZA81AblAOOXKaUaxvFC+ZK\n"
                                    "RJf+MtUGnv0v7CrGoblm1mMC/OQI1JfSsYi68EpnaOLepTZw+GLTnusQgwIDAQAB\n"
                                    "AoGBAKDuq3PikblH/9YS11AgwjwC++7ZcltzeZJdGTSPY1El2n6Dip9ML0hUjeSM\n"
                                    "ROIWtac/nsNcJCnvOnUjK/c3NIAaGJcfRPiH/S0Ga6ROiDfFj2UXAmk/v4wRRUzr\n"
                                    "5lsA0jgEt5qcq2Xr/JPQVGB4wUgL/yQK0dDhW0EdrJ707e3BAkEA1aIHbmcVfCP8\n"
                                    "Y/uWuK0lvWxrIWfR5MlHhI8tD9lvkot2kyXiV+jB6/gktwk1QaFsy7dCXn7w03+k\n"
                                    "xrjEGGN+kQJBAMuKf55lDtU9K2Js3YSStTZAXP+Hz7XpoLxmbWFyGvBx806WjgAD\n"
                                    "624irwS+0tBxkERbRcisfb2cXmAx8earT9MCQDZuVCpjBWxd1t66qYpgQ29iAmG+\n"
                                    "jBIY3qn9uOOC6RSTiCCx1FvFqDMxRFmGdRVFxeyZwsVE3qNksF0Zko0MPKECQCEe\n"
                                    "oDV97DP2iCCz5je0R5hUUM2jo8DOC0GcyR+aGZgWcqjPBrwp5x08t43mHxeb4wW8\n"
                                    "dFZ6+trnntO4TMxkA9ECQB+yCPgO1zisJWYuD46KISoesYhwHe5C1BQElQgi9bio\n"
                                    "U39fFo88w1pok23a2CZBEXguSvCvexeB68OggdDXvy0=\n"
                                    "-----END RSA PRIVATE KEY-----\n");
        //RsaEncrypter::loadPrivateKey(privKeyPath);

        //Start ExtServer
        SServer me = Configuration::getInstance().getExtServer(num);
        extServer server(me.getPortExt(), me.getPortDB(), me.getPortClient());
        server.start();

    //}
    //else{
        //std::cout<<"Invalid arguments"<<std::endl;
   // }

    return a.exec();
}


















//#include <QCoreApplication>
//#include <QDebug>
//#include <iostream>
//#include "servserverlisten.h"



//int main(int argc, char *argv[])
//{
//    QCoreApplication a(argc, argv);
//    TcpServer server(2000);
//    ServServerListen sServerListen("settings.ini", &server);
//    sServerListen.start();

//    QObject::connect(&server, SIGNAL(log(QString)), &sServerListen, SLOT(log(QString)));
//    QObject::connect(&server, SIGNAL(error(QString,QString)), &sServerListen, SLOT(error(QString,QString)));
//    QObject::connect(&server, SIGNAL(frameContent(QTcpSocket*,QStringList)), &sServerListen, SLOT(frame(QTcpSocket*,QStringList)));


//    server.start();
//    return a.exec();
//}

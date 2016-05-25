#include <QCoreApplication>
#include <QObject>
#include <stdio.h>
#include "tester.h"
#include "tcpserver.h"
#include "cipheradapter.h"
#include "rsaencrypter.h"

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);
    std::cout<<"Communication test"<<std::endl;
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
   QString pubKey = "-----BEGIN PUBLIC KEY-----\n"
                    "MIGfMA0GCSqGSIb3DQEBAQUAA4GNADCBiQKBgQCp2w+8HUdECo8V5yuKYrWJmUbL\n"
                    "tD6nSyVifN543axXvNSFzQfWNOGVkMsCo6W4hpl5eHv1p9Hqdcf/ZYQDWCK726u6\n"
                    "hsZA81AblAOOXKaUaxvFC+ZKRJf+MtUGnv0v7CrGoblm1mMC/OQI1JfSsYi68Epn\n"
                    "aOLepTZw+GLTnusQgwIDAQAB\n"
                    "-----END PUBLIC KEY-----\n";

   QString encrypted = RsaEncrypter::encrypt("To jest text który ma 128 zanków.To jest text który ma 128 zanków.To jest text aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaab", pubKey);
   std::cout<<encrypted.toStdString()<<std::endl;
   QString decrypted = RsaEncrypter::decrypt(encrypted);
   std::cout<<decrypted.toStdString()<<std::endl;
    //trzeba postawić serwer, podpiąć mu sloty i wystartować
    TcpServer server(2000);
    CipherAdapter sc(3000);
    tester t2(&sc);
    tester t(&server);

    QObject::connect(&server, SIGNAL(log(QString)), &t, SLOT(log(QString)));
    QObject::connect(&server, SIGNAL(frameContent(QTcpSocket*,QStringList)), &t, SLOT(frame(QTcpSocket*,QStringList)));

    QObject::connect(&sc, SIGNAL(log(QString)), &t2, SLOT(log(QString)));
    QObject::connect(&sc, SIGNAL(frameContent(QTcpSocket*,QStringList)), &t2, SLOT(frame(QTcpSocket*,QStringList)));

    server.start();
    sc.start();

    return a.exec();
}

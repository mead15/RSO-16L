#include "cipheradapter.h"

CipherAdapter::CipherAdapter(int port) : server(port)
{
    connect(&server, SIGNAL(log(QString)), this, SLOT(catchLog(QString)));
    connect(&server, SIGNAL(error(QString)), this, SLOT(catchError(QString)));
    connect(&server, SIGNAL(frameContent(QTcpSocket*,QStringList)), this, SLOT(catchFrameContent(QTcpSocket*,QStringList)));
}

void CipherAdapter::start()
{
    server.start();
}

void CipherAdapter::sendFrame(QHostAddress recipient, int port, QStringList content, QString key)
{
    //zaszyfrować content kluczem publicznym odbiorcy do encypted
    // content przerobić na ciąg bajtów, zaszyfrować, zakodować przez base64 i wrzucić do encrypted
    QString encrypted = RsaEncrypter::encrypt(content.join(""), key);
    QStringList frame = QStringList() << encrypted;
    server.sendFrame(recipient, port, frame);
}

void CipherAdapter::sendFrame(QTcpSocket *recipient, QStringList content, QString key)
{
    //zaszyfrować content kluczem publicznym odbiorcy do encypted
    // RsaEncrypter oczekuje QStringa z ramką (wartości oddzielone przecinkiem, ale bez nawiasów),
    // zwróci QStringa z zaszyfrowaną i zakodowaną ramką

    QString encrypted = RsaEncrypter::encrypt(content.join(","), key);
    QStringList frame = QStringList() << encrypted;
    server.sendFrame(recipient, frame);
}

void CipherAdapter::catchFrameContent(QTcpSocket *socket, QStringList content)
{
    //w content siedzi ramka zaszyfrowana przez rsa a potem zakodowana przez base64 (1 element)
    // RsaEncrypter oczekuje ramki zakodowanej, zwróci QStringa z ramką odkodowaną, trzeba ją jeszcze sparsować

    //std::cout << "w catchFrame" << std::endl;

    QString frame(RsaEncrypter::decrypt(content.at(0)));

    bool nadal = true;
    int pos1=0, pos2=0;
    while(nadal)
    {
        pos1 = frame.indexOf("(", pos1);
        pos2 = frame.indexOf(")", pos2);
        int len = pos2-pos1-1;
        if(len > 0)
            emit frameContent(socket, frame.mid(pos1+1, len).split(","));
        if(pos1 == -1 || pos2 == -1)
            nadal = false;
        pos1++;
        pos2++;
    }
}

void CipherAdapter::catchLog(QString content)
{
    emit log(content);
}

void CipherAdapter::catchError(QString content)
{
    emit error(content);
}

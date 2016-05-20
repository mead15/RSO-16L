#include "rsaencrypter.h"
#include <iostream>

QString RsaEncrypter::privateKey;

RsaEncrypter::RsaEncrypter()
{
}

QString RsaEncrypter::encrypt(QString data, QString key)
{
    QByteArray before = QByteArray::fromRawData(data.toStdString().c_str(), sizeof(data.toStdString().c_str()));
    QString ret = before.toBase64();
    std::cout << "enc: " << ret.toStdString() << std::endl;
    return ret;
}

QString RsaEncrypter::decrypt(QString data)
{
    QByteArray before = QByteArray::fromRawData(data.toStdString().c_str(), sizeof(data.toStdString().c_str()));
    QString ret = QByteArray::fromBase64(before);
    std::cout << "dec: " << ret.toStdString() << std::endl;
    return ret;
}

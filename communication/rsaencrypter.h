#ifndef RSAENCRYPTER_H
#define RSAENCRYPTER_H

#include <QString>
#include <QStringList>
#include <QByteArray>
#include <openssl/rsa.h>
#include <openssl/pem.h>
#include <openssl/err.h>
#include <openssl/engine.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <stdio.h>

#define KEY_LENGTH  2048
#define PADDING RSA_PKCS1_PADDING

class RsaEncrypter
{
public:
    RsaEncrypter();
    static QString encrypt(QString data, QString key);
    static QString decrypt(QString data);
    static void setPrivateKey(QString key);
    static void loadPrivateKey(QString &path);
private:
    static RSA* privateKey;
    static RSA* loadPUBLICKeyFromString( const char* publicKeyStr );
    static RSA* loadPRIVATEKeyFromString( const char* privateKeyStr );
};

#endif // RSAENCRYPTER_H


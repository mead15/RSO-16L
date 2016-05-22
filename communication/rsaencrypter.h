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

#define KEY_LENGTH  2048
#define PADDING RSA_PKCS1_PADDING

class RsaEncrypter
{
public:
    RsaEncrypter();
    static QString encrypt(QString data, QString key);
    static QString decrypt(QString data);
    static void setPrivateKey(QString key);
private:
    static RSA* privateKey;
    static RSA* loadPUBLICKeyFromString( const char* publicKeyStr );
    static RSA* loadPRIVATEKeyFromString( const char* privateKeyStr );
};

#endif // RSAENCRYPTER_H

/*
 * QByteArray ma metody toBase64 i fromBase64
 *
 * encrypt przyjmuje QStringa z całą ramką do zaszyfrowania i QStringa z kluczem prywatnym odbiorcy,
 * powinien zwrócić QStringa z ramką zaszyfrowaną przez rsa i potem zakodowaną przez base64
 *
 * decrypt przyjmuje QStringa z wiadomością zaszyfrowaną przez rsa i zakodowaną przez base64,
 * trzeba odkodować base64 a potem odszyfrować rsa własnym kluczem prywatnym i zwrócić QStringa z odszyfrowaną ramką
 *
 * setPrivateKey - wiadomo
 */

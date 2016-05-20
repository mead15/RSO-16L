#ifndef RSAENCRYPTER_H
#define RSAENCRYPTER_H

#include <QString>
#include <QStringList>
#include <QByteArray>

class RsaEncrypter
{
public:
    RsaEncrypter();
    static QString encrypt(QString data, QString key);
    static QString decrypt(QString data);
    static void setPrivateKey(QString key) { privateKey = key; }
private:
    static QString privateKey;
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

#include "rsaencrypter.h"
#include <iostream>

RSA* RsaEncrypter::privateKey;

RsaEncrypter::RsaEncrypter()
{
}

QString RsaEncrypter::encrypt(QString data, QString key)
{
    QByteArray before = data.toLocal8Bit();
    int msgLen = before.size();
    char * msg = before.data();

    QByteArray keybefore = key.toLocal8Bit();
    char * keyChar = keybefore.data();
    RSA* pubKey = loadPUBLICKeyFromString(keyChar);

    int rsaLen = RSA_size( pubKey );
    int maxMsgLen = rsaLen - 20;
    QByteArray resultByte;
    for(int i=0; i<msgLen;){
        char* msgPart = msg + i;
        int len = maxMsgLen;
        if(i+maxMsgLen>=msgLen){
            len = msgLen - i;
        }
        i+=len;
        unsigned char* ed = new unsigned char[rsaLen];
        int resultLen = RSA_public_encrypt( len, (const unsigned char*)msgPart, ed, pubKey, PADDING ) ;
        if( resultLen == -1 )
           std::cout<<"ERROR: RSA_public_encrypt: %s\n"<<ERR_error_string(ERR_get_error(), NULL)<<std::endl;
        resultByte +=QByteArray((char*)ed, resultLen);
    }
    QByteArray resultBase64 = resultByte.toBase64();
    QString result = QString(resultBase64);
    return result;
}

QString RsaEncrypter::decrypt(QString data)
{
    QByteArray before = data.toLocal8Bit();
    QByteArray encodedBase64 = QByteArray::fromBase64(before);
    unsigned char * encryptedData = (unsigned char*)encodedBase64.data();
    int rsaLen = RSA_size( privateKey ) ; // That's how many bytes the decrypted data would be
    int partNum = encodedBase64.size()/rsaLen;
    QString result;
    for(int i=0; i<partNum; i++){
        unsigned char *decryptedBin = new unsigned char[rsaLen];
        int resultLen = RSA_private_decrypt( rsaLen, encryptedData + i*rsaLen, decryptedBin, privateKey, PADDING ) ;
        if( resultLen == -1 )
           std::cout<<"ERROR: RSA_private_decrypt: %s\n"<<ERR_error_string(ERR_get_error(), NULL)<<std::endl;
        result += QString::fromLocal8Bit((char*)decryptedBin, resultLen);
    }

    return result;
}

void RsaEncrypter::setPrivateKey(QString key){
    QByteArray before = key.toLocal8Bit();
    char * keyChar = before.data();
    privateKey = loadPRIVATEKeyFromString(keyChar);
}

RSA* RsaEncrypter::loadPUBLICKeyFromString( const char* publicKeyStr )
{
  // A BIO is an I/O abstraction (Byte I/O?)

  // BIO_new_mem_buf: Create a read-only bio buf with data
  // in string passed. -1 means string is null terminated,
  // so BIO_new_mem_buf can find the dataLen itself.
  // Since BIO_new_mem_buf will be READ ONLY, it's fine that publicKeyStr is const.
  BIO* bio = BIO_new_mem_buf( (void*)publicKeyStr, -1 ) ; // -1: assume string is null terminated

  BIO_set_flags( bio, BIO_FLAGS_BASE64_NO_NL ) ; // NO NL

  // Load the RSA key from the BIO
  RSA* rsaPubKey = PEM_read_bio_RSA_PUBKEY( bio, NULL, NULL, NULL ) ;
  if( !rsaPubKey )
    printf( "ERROR: Could not load PUBLIC KEY!  PEM_read_bio_RSA_PUBKEY FAILED: %s\n", ERR_error_string( ERR_get_error(), NULL ) ) ;

  BIO_free( bio ) ;
  return rsaPubKey ;
}

RSA* RsaEncrypter::loadPRIVATEKeyFromString( const char* privateKeyStr )
{
  BIO *bio = BIO_new_mem_buf( (void*)privateKeyStr, -1 );
  //BIO_set_flags( bio, BIO_FLAGS_BASE64_NO_NL ) ; // NO NL
  RSA* rsaPrivKey = PEM_read_bio_RSAPrivateKey( bio, NULL, NULL, NULL ) ;

  if ( !rsaPrivKey )
    printf("ERROR: Could not load PRIVATE KEY!  PEM_read_bio_RSAPrivateKey FAILED: %s\n", ERR_error_string(ERR_get_error(), NULL));

  BIO_free( bio ) ;
  return rsaPrivKey ;
}

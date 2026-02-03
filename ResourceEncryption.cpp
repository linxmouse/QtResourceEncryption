#include "ResourceEncryption.h"
#include <QCryptographicHash>

QByteArray ResourceEncryption::encrypt(const QByteArray &data, const QString &key)
{
    QByteArray keyHash = generateKey(key);
    return xorEncrypt(data, keyHash);
}

QByteArray ResourceEncryption::decrypt(const QByteArray &encryptedData, const QString &key)
{
    // XOR加密的特性:加密和解密使用相同的操作
    QByteArray keyHash = generateKey(key);
    return xorEncrypt(encryptedData, keyHash);
}

QByteArray ResourceEncryption::generateKey(const QString &key)
{
    // 使用SHA-256生成固定长度的密钥
    return QCryptographicHash::hash(key.toUtf8(), QCryptographicHash::Sha256);
}

QByteArray ResourceEncryption::xorEncrypt(const QByteArray &data, const QByteArray &key)
{
    QByteArray result = data;
    int keyLen = key.size();
    
    for (int i = 0; i < result.size(); ++i) {
        result[i] = result[i] ^ key[i % keyLen];
    }
    
    return result;
}

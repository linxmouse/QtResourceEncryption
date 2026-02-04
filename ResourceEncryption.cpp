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
    // 使用SHA-256生成固定长度的密钥 (32字节，正好对应 AES-256)
    return QCryptographicHash::hash(key.toUtf8(), QCryptographicHash::Sha256);
}

/**
 * @brief 使用 XOR 算法进行加解密
 * 由于 XOR 的特性：(A ^ B) ^ B = A，因此加解密可以使用同一个函数。
 * @param data 原始数据或已加密的数据
 * @param key  加密密钥
 * @return QByteArray 处理后的结果
 */
QByteArray ResourceEncryption::xorEncrypt(const QByteArray &data, const QByteArray &key)
{
    // 复制原始数据到结果缓存
    QByteArray result = data;
    int keyLen = key.size();    
    // 如果密钥为空，则直接返回原数据
    if (keyLen <= 0) return result;
    // 遍历每一个字节进行异或处理
    for (int i = 0; i < result.size(); ++i) {
        // 使用循环密钥（通过取模运算 i % keyLen 循环使用密钥字节）
        result[i] = result[i] ^ key[i % keyLen];
    }  
    return result;
}

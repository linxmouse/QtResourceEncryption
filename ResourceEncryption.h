#ifndef RESOURCEENCRYPTION_H
#define RESOURCEENCRYPTION_H

#include <QByteArray>
#include <QString>
#include <QCryptographicHash>

/**
 * @brief 资源加密/解密核心类
 * 使用AES-256算法进行资源加密
 */
class ResourceEncryption
{
public:
    /**
     * @brief 加密数据
     * @param data 原始数据
     * @param key 加密密钥
     * @return 加密后的数据
     */
    static QByteArray encrypt(const QByteArray &data, const QString &key);
    
    /**
     * @brief 解密数据
     * @param encryptedData 加密的数据
     * @param key 解密密钥
     * @return 解密后的数据
     */
    static QByteArray decrypt(const QByteArray &encryptedData, const QString &key);
    
private:
    /**
     * @brief 生成256位密钥
     * @param key 原始密钥字符串
     * @return 256位密钥
     */
    static QByteArray generateKey(const QString &key);
    
    /**
     * @brief 简单的XOR加密(演示用,生产环境建议使用OpenSSL的AES)
     * @param data 数据
     * @param key 密钥
     * @return 处理后的数据
     */
    static QByteArray xorEncrypt(const QByteArray &data, const QByteArray &key);
};

#endif // RESOURCEENCRYPTION_H

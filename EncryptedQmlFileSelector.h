#ifndef ENCRYPTEDQMLFILESELECTOR_H
#define ENCRYPTEDQMLFILESELECTOR_H

#include <QQmlFileSelector>
#include <QQmlEngine>
#include <QString>
#include <QHash>

/**
 * @brief 加密QML文件选择器
 * 在QML引擎加载文件时,自动解密加密的资源
 */
class EncryptedQmlFileSelector : public QObject
{
    Q_OBJECT
    
public:
    explicit EncryptedQmlFileSelector(QQmlEngine *engine, const QString &decryptionKey, QObject *parent = nullptr);
    
    /**
     * @brief 注册加密的资源路径
     * @param virtualPath 虚拟路径(qrc路径)
     * @param encryptedData 加密的数据
     */
    void registerEncryptedResource(const QString &virtualPath, const QByteArray &encryptedData);
    
    /**
     * @brief 获取解密后的资源
     * @param path 资源路径
     * @return 解密后的数据
     */
    QByteArray getDecryptedResource(const QString &path);
    
private:
    QQmlEngine *m_engine;
    QString m_decryptionKey;
    QHash<QString, QByteArray> m_encryptedResources;
};

#endif // ENCRYPTEDQMLFILESELECTOR_H

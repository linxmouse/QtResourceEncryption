#include "EncryptedQmlFileSelector.h"
#include "ResourceEncryption.h"

EncryptedQmlFileSelector::EncryptedQmlFileSelector(QQmlEngine *engine, const QString &decryptionKey, QObject *parent)
    : QObject(parent)
    , m_engine(engine)
    , m_decryptionKey(decryptionKey)
{
}

void EncryptedQmlFileSelector::registerEncryptedResource(const QString &virtualPath, const QByteArray &encryptedData)
{
    m_encryptedResources.insert(virtualPath, encryptedData);
}

QByteArray EncryptedQmlFileSelector::getDecryptedResource(const QString &path)
{
    if (m_encryptedResources.contains(path)) 
    {
        QByteArray encryptedData = m_encryptedResources.value(path);
        qDebug() << "开始解密资源:" << path << "(加密数据大小:" << encryptedData.size() << "字节)";     
        QByteArray decryptedData = ResourceEncryption::decrypt(encryptedData, m_decryptionKey);        
        if (decryptedData.isEmpty()) qWarning() << "解密失败: 返回的数据为空" << path;
        else qDebug() << "成功解密资源:" << path << "(解密数据大小:" << decryptedData.size() << "字节)";      
        return decryptedData;
    } 
    else 
    {
        qWarning() << "资源未找到:" << path;
        qWarning() << "已注册的资源:" << m_encryptedResources.keys();
    }
    return QByteArray();
}

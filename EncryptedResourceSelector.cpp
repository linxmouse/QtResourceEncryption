#include "EncryptedResourceSelector.h"
#include "ResourceEncryption.h"
#include <QDebug>
#include <QFile>
#include <QIODevice>

EncryptedResourceSelector::EncryptedResourceSelector(
    QQmlEngine *engine, const QString &decryptionKey, QObject *parent)
    : QObject(parent), m_engine(engine), m_decryptionKey(decryptionKey) {}

void EncryptedResourceSelector::setRawMode(bool isRawMode,
                                           const QString &basePath) {
  m_isRawMode = isRawMode;
  m_basePath = basePath;
}

void EncryptedResourceSelector::registerEncryptedResource(
    const QString &virtualPath, const QByteArray &encryptedData) {
  m_encryptedResources.insert(virtualPath, encryptedData);
}

QByteArray
EncryptedResourceSelector::getDecryptedResource(const QString &path) {
  // 如果是原始模式，直接从本地文件系统加载
  if (m_isRawMode) {
    QString fullPath = m_basePath + "/" + path;
    QFile file(fullPath);
    if (file.open(QIODevice::ReadOnly)) {
      return file.readAll();
    }
    qWarning() << "[RawMode] 资源未找到:" << fullPath;
    return QByteArray();
  }

  if (m_encryptedResources.contains(path)) {
    QByteArray encryptedData = m_encryptedResources.value(path);
    QByteArray decryptedData =
        ResourceEncryption::decrypt(encryptedData, m_decryptionKey);
    if (decryptedData.isEmpty())
      qWarning() << "解密失败: 返回的数据为空" << path;
    return decryptedData;
  } else {
    qWarning() << "资源未找到:" << path;
  }
  return QByteArray();
}

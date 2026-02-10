#ifndef ENCRYPTEDRESOURCESELECTOR_H
#define ENCRYPTEDRESOURCESELECTOR_H

#include <QHash>
#include <QQmlEngine>
#include <QString>


/**
 * @brief 加密资源选择器
 * 在加载加密资源（QML、JS、图片等）时，从内存中提供解密后的数据
 */
class EncryptedResourceSelector : public QObject {
  Q_OBJECT

public:
  explicit EncryptedResourceSelector(QQmlEngine *engine,
                                     const QString &decryptionKey,
                                     QObject *parent = nullptr);

  /**
   * @brief 设置是否为原始模式及基础路径
   * @param isRawMode 是否开启原始模式
   * @param basePath 原始文件的存放路径
   */
  void setRawMode(bool isRawMode, const QString &basePath = "");

  /**
   * @brief 注册加密的资源路径
   * @param virtualPath 虚拟路径(qrc路径)
   * @param encryptedData 加密的数据
   */
  void registerEncryptedResource(const QString &virtualPath,
                                 const QByteArray &encryptedData);

  /**
   * @brief 获取解密后的资源
   * @param path 资源路径
   * @return 解密后的数据
   */
  QByteArray getDecryptedResource(const QString &path);

private:
  QQmlEngine *m_engine;
  QString m_decryptionKey;
  bool m_isRawMode = false;
  QString m_basePath;
  QHash<QString, QByteArray> m_encryptedResources;
};

#endif // ENCRYPTEDRESOURCESELECTOR_H

#ifndef ENCRYPTEDNETWORKACCESSMANAGER_H
#define ENCRYPTEDNETWORKACCESSMANAGER_H

#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QString>
#include <QHash>

class EncryptedResourceSelector;

/**
 * @brief 加密资源网络访问管理器
 * 拦截Qt的资源加载请求,对加密资源进行解密
 */
class EncryptedNetworkAccessManager : public QNetworkAccessManager
{
    Q_OBJECT
    
public:
    explicit EncryptedNetworkAccessManager(EncryptedResourceSelector *selector, QObject *parent = nullptr);
    
protected:
    QNetworkReply *createRequest(Operation op, const QNetworkRequest &request, 
                                 QIODevice *outgoingData = nullptr) override;
    
private:
    EncryptedResourceSelector *m_resourceSelector;
};

#include <QQmlNetworkAccessManagerFactory>

/**
 * @brief QQmlNetworkAccessManagerFactory 的实现
 * 用于为 QQmlEngine 提供自定义的 QNetworkAccessManager
 */
class EncryptedNetworkAccessManagerFactory : public QQmlNetworkAccessManagerFactory
{
public:
    explicit EncryptedNetworkAccessManagerFactory(EncryptedResourceSelector *selector)
        : m_resourceSelector(selector) {}

    QNetworkAccessManager *create(QObject *parent) override {
        return new EncryptedNetworkAccessManager(m_resourceSelector, parent);
    }

private:
    EncryptedResourceSelector *m_resourceSelector;
};

/**
 * @brief 自定义网络回复,返回解密后的数据
 */
class EncryptedNetworkReply : public QNetworkReply
{
    Q_OBJECT
    
public:
    explicit EncryptedNetworkReply(const QByteArray &data, const QUrl &url, QObject *parent = nullptr);
    
    void abort() override;
    qint64 bytesAvailable() const override;
    bool isSequential() const override;
    
protected:
    qint64 readData(char *data, qint64 maxlen) override;
    
private:
    QByteArray m_data;
    qint64 m_offset;
};

#endif // ENCRYPTEDNETWORKACCESSMANAGER_H

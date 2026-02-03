#include "EncryptedNetworkAccessManager.h"
#include "EncryptedQmlFileSelector.h"
#include <QDebug>

EncryptedNetworkAccessManager::EncryptedNetworkAccessManager(EncryptedQmlFileSelector *selector, QObject *parent)
    : QNetworkAccessManager(parent)
    , m_selector(selector)
{
}

QNetworkReply *EncryptedNetworkAccessManager::createRequest(Operation op, const QNetworkRequest &request, QIODevice *outgoingData)
{
    const QUrl url = request.url();
    // 协议过滤：非自定义协议直接进入默认处理流程
    if (url.scheme() != "encrypted") return QNetworkAccessManager::createRequest(op, request, outgoingData);   
    // 路径标准化：移除路径前缀的所有 '/' (例如 "/main.qml" -> "main.qml")
    QString resourcePath = url.path();
    while (resourcePath.startsWith('/')) resourcePath.remove(0, 1);
    // 核心请求逻辑
    qDebug() << "[Network] 尝试加载加密资源:" << resourcePath;
    // 尝试获取解密后的数据
    QByteArray data = m_selector->getDecryptedResource(resourcePath);
    // 处理特殊情况：如果没找到对应数据，且是 qmldir 这种元数据请求，返回空内容以防止引擎报错
    if (data.isEmpty() && resourcePath.endsWith("qmldir")) {
        qDebug() << "[Network] 为 qmldir 提供空响应兜底";
        return new EncryptedNetworkReply(QByteArray(), url, this);
    }
    // 返回解密后的回复
    if (!data.isEmpty()) return new EncryptedNetworkReply(data, url, this);
    // 兜底：如果完全没找到资源，按默认逻辑处理（通常会触发 404）
    qWarning() << "[Network] 资源既未加密也未找到:" << resourcePath;
    return QNetworkAccessManager::createRequest(op, request, outgoingData);
}

// EncryptedNetworkReply 实现
EncryptedNetworkReply::EncryptedNetworkReply(const QByteArray &data, const QUrl &url, QObject *parent)
    : QNetworkReply(parent)
    , m_data(data)
    , m_offset(0)
{
    setUrl(url);
    setOperation(QNetworkAccessManager::GetOperation);   
    // 设置成功状态码，有些 Qt 组件会检查这个
    setAttribute(QNetworkRequest::HttpStatusCodeAttribute, 200);
    // 设置头信息
    setHeader(QNetworkRequest::ContentLengthHeader, QVariant(data.size()));
    // 根据文件扩展名设置 Content-Type
    QString path = url.path();
    if (path.endsWith(".qml")) setHeader(QNetworkRequest::ContentTypeHeader, QVariant("text/plain"));
    else if (path.endsWith(".js")) setHeader(QNetworkRequest::ContentTypeHeader, QVariant("application/javascript"));
    else if (path.endsWith(".png")) setHeader(QNetworkRequest::ContentTypeHeader, QVariant("image/png"));
    else if (path.endsWith(".jpg") || path.endsWith(".jpeg")) setHeader(QNetworkRequest::ContentTypeHeader, QVariant("image/jpeg"));
    else setHeader(QNetworkRequest::ContentTypeHeader, QVariant("application/octet-stream"));
    // 打开设备
    open(ReadOnly | Unbuffered);
    // 延迟发送信号
    QMetaObject::invokeMethod(this, [this]() {
        if (m_data.size() > 0) emit readyRead();
        emit finished();
    }, Qt::QueuedConnection);
}

void EncryptedNetworkReply::abort()
{
    // 不需要特殊处理
}

qint64 EncryptedNetworkReply::bytesAvailable() const
{
    return m_data.size() - m_offset + QNetworkReply::bytesAvailable();
}

bool EncryptedNetworkReply::isSequential() const
{
    return true;
}

qint64 EncryptedNetworkReply::readData(char *data, qint64 maxlen)
{
    if (m_offset >= m_data.size()) return 0; // 改为 0: 表示 EOF，不是错误
    qint64 number = qMin(maxlen, (qint64)(m_data.size() - m_offset));
    memcpy(data, m_data.constData() + m_offset, number);
    m_offset += number;
    return number;
}

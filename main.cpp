#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQmlContext>
#include <QFile>
#include <QDebug>
#include <QTextStream>
#include <QDateTime>

#include "ResourceEncryption.h"
#include "EncryptedResourceSelector.h"
#include "EncryptedNetworkAccessManager.h"

// 日志文件
static QFile logFile;

void messageHandler(QtMsgType type, const QMessageLogContext &context, const QString &msg)
{
    QString txt;
    switch (type) {
    case QtDebugMsg:
        txt = QString("[DEBUG] %1").arg(msg);
        break;
    case QtWarningMsg:
        txt = QString("[WARNING] %1").arg(msg);
        break;
    case QtCriticalMsg:
        txt = QString("[CRITICAL] %1").arg(msg);
        break;
    case QtFatalMsg:
        txt = QString("[FATAL] %1").arg(msg);
        abort();
    case QtInfoMsg:
        txt = QString("[INFO] %1").arg(msg);
        break;
    }
    // 输出到控制台
    fprintf(stderr, "%s\n", txt.toLocal8Bit().constData());
    fflush(stderr);
    // 输出到文件
    if (logFile.isOpen()) {
        QTextStream ts(&logFile);
        ts << QDateTime::currentDateTime().toString("hh:mm:ss.zzz ") << txt << "\n";
        ts.flush();
    }
}

/**
 * @brief 加载并注册加密的资源文件
 */
void loadEncryptedResources(EncryptedResourceSelector *selector, const QString &key)
{
    // 示例: 从加密文件中加载资源
    // 实际应用中,这些加密文件可以嵌入到二进制中或从外部加载

    QFile encFile(":/encrypted/main.qml.enc");
    if (encFile.open(QIODevice::ReadOnly))
    {
        QByteArray encryptedData = encFile.readAll();
        selector->registerEncryptedResource("main.qml", encryptedData);
        encFile.close();
        qDebug() << "已注册加密资源: main.qml";
    }

    QFile encFile2(":/encrypted/MyComponent.qml.enc");
    if (encFile2.open(QIODevice::ReadOnly))
    {
        QByteArray encryptedData = encFile2.readAll();
        selector->registerEncryptedResource("MyComponent.qml", encryptedData);
        encFile2.close();
        qDebug() << "已注册加密资源: MyComponent.qml";
    }

    // 新增：测试图片资源 (从本地文件读取加密后的产物)
    QFile imgFile(":/encrypted/CMake-Logo.png.enc");
    if (imgFile.open(QIODevice::ReadOnly))
    {
        QByteArray encryptedData = imgFile.readAll();
        selector->registerEncryptedResource("CMake-Logo.png", encryptedData);
        imgFile.close();
        qDebug() << "已注册加密资源: CMake-Logo.png";
    }
}

int main(int argc, char *argv[])
{
    // 安装日志处理器
    logFile.setFileName("debug.log");
    logFile.open(QIODevice::WriteOnly | QIODevice::Append);
    qInstallMessageHandler(messageHandler);

    QGuiApplication app(argc, argv);

    // 密钥应该通过更安全的方式存储,这里仅作演示
    const QString DECRYPTION_KEY = "MySecretKey123!@#";

    QQmlApplicationEngine engine;

    // 创建加密资源选择器
    EncryptedResourceSelector *selector = new EncryptedResourceSelector(&engine, DECRYPTION_KEY, &app);

    // 加载加密的资源
    loadEncryptedResources(selector, DECRYPTION_KEY);

    // 使用自定义网络访问管理器
    engine.setNetworkAccessManagerFactory(new EncryptedNetworkAccessManagerFactory(selector));

    // 由于自定义协议会被视为网络请求, 加载是异步的。
    const QUrl url(QStringLiteral("encrypted:/main.qml"));
    QObject::connect(&engine, &QQmlApplicationEngine::objectCreated,
                     &app, [url](QObject *obj, const QUrl &objUrl) {
        if (!obj && url == objUrl) {
            qCritical() << "QML加载失败: 无法创建对象" << objUrl;
            QCoreApplication::exit(-1);
        } else {
            qDebug() << "QML对象创建成功:" << objUrl;
        }
    }, Qt::QueuedConnection);
    engine.load(url);

    return app.exec();
}

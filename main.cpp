#include <QDateTime>
#include <QDebug>
#include <QDirIterator>
#include <QFile>
#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQmlContext>
#include <QTextStream>

#include "EncryptedNetworkAccessManager.h"
#include "EncryptedResourceSelector.h"

// 原始资源模式开关
// #define USE_ENCRYPTED_RESOURCES

// 日志文件
static QFile logFile;

void messageHandler(QtMsgType type, const QMessageLogContext &context,
                    const QString &msg) {
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
 * @brief 自动加载并注册所有加密资源
 * 扫描 qrc 中 :/encrypted 路径下的所有 .enc 文件并自动注册
 */
void loadEncryptedResources(EncryptedResourceSelector *selector) {
  // // 从加密文件中加载资源
  // // 这些加密文件可以嵌入到二进制中或从外部加载
  // QFile encFile(":/encrypted/main.qml.enc");
  // if (encFile.open(QIODevice::ReadOnly))
  // {
  //     QByteArray encryptedData = encFile.readAll();
  //     selector->registerEncryptedResource("main.qml", encryptedData);
  //     encFile.close();
  //     qDebug() << "已注册加密资源: main.qml";
  // }
  // QFile encFile2(":/encrypted/MyComponent.qml.enc");
  // if (encFile2.open(QIODevice::ReadOnly))
  // {
  //     QByteArray encryptedData = encFile2.readAll();
  //     selector->registerEncryptedResource("MyComponent.qml", encryptedData);
  //     encFile2.close();
  //     qDebug() << "已注册加密资源: MyComponent.qml";
  // }
  // // 测试图片资源 (从本地文件读取加密后的产物)
  // QFile imgFile(":/encrypted/CMake-Logo.png.enc");
  // if (imgFile.open(QIODevice::ReadOnly))
  // {
  //     QByteArray encryptedData = imgFile.readAll();
  //     selector->registerEncryptedResource("CMake-Logo.png", encryptedData);
  //     imgFile.close();
  //     qDebug() << "已注册加密资源: CMake-Logo.png";
  // }
  // // 加载 qmldir
  // QFile qmldirFile(":/encrypted/qmldir.enc");
  // if (qmldirFile.open(QIODevice::ReadOnly))
  // {
  //     selector->registerEncryptedResource("qmldir", qmldirFile.readAll());
  //     qDebug() << "成功注册加密资源: qmldir";
  // }

  // 使用 QDirIterator 遍历资源系统中的加密文件
  QDirIterator it(":/encrypted", QDirIterator::Subdirectories);
  while (it.hasNext()) {
    it.next();
    QString fullPath = it.filePath(); // 例如 ":/encrypted/main.qml.enc"
    QString fileName = it.fileName(); // 例如 "main.qml.enc"
    // 只处理以 .enc 结尾的文件
    if (fileName.endsWith(".enc")) {
      QFile file(fullPath);
      if (file.open(QIODevice::ReadOnly)) {
        // 自动计算虚拟路径：去掉末尾的 .enc
        QString virtualName = fileName.left(fileName.length() - 4);
        selector->registerEncryptedResource(virtualName, file.readAll());
        file.close();
        qDebug() << "[AutoLoad] 已注册加密资源:" << virtualName << "<-"
                 << fileName;
      }
    }
  }
}

int main(int argc, char *argv[]) {
  // 安装日志处理器
  logFile.setFileName("debug.log");
  logFile.open(QIODevice::WriteOnly | QIODevice::Append);
  qInstallMessageHandler(messageHandler);

  QGuiApplication app(argc, argv);
  QQmlApplicationEngine engine;

  // --- 开发调试开关 ---
  // 注释掉顶部的 #define USE_ENCRYPTED_RESOURCES 即可切换到“原始资源模式”

  QUrl url = QUrl(QStringLiteral("encrypted:/main.qml"));
  const QString DECRYPTION_KEY = "MySecretKey123!@#";

  // 统一创建选择器
  EncryptedResourceSelector *selector =
      new EncryptedResourceSelector(&engine, DECRYPTION_KEY, &app);

#ifdef USE_ENCRYPTED_RESOURCES
  qDebug() << "运行模式: [加密模式]";
  loadEncryptedResources(selector);
#else
  qDebug() << "运行模式: [原始资源模式] - 自定义协议自动映射本地文件";
  // 设置为原始模式，并指向源码根目录
  selector->setRawMode(true, app.applicationDirPath() + "/../..");
#endif

  // 无论哪种模式，都注册自定义网络管理，这样 QML 里的 encrypted:/// 永远有效
  engine.setNetworkAccessManagerFactory(
      new EncryptedNetworkAccessManagerFactory(selector));

  QObject::connect(
      &engine, &QQmlApplicationEngine::objectCreated, &app,
      [url](QObject *obj, const QUrl &objUrl) {
        if (!obj && url == objUrl) {
          qCritical() << "QML加载失败: 无法创建对象" << objUrl;
          QCoreApplication::exit(-1);
        } else {
          qDebug() << "QML对象创建成功:" << objUrl;
        }
      },
      Qt::QueuedConnection);
  engine.load(url);

  return app.exec();
}

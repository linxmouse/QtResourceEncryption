#include <QCoreApplication>
#include <QCommandLineParser>
#include <QDebug>
#include "ResourceEncryptor.h"

int main(int argc, char *argv[])
{
    QCoreApplication app(argc, argv);
    app.setApplicationName("Qt Resource Encryptor");
    app.setApplicationVersion("1.0");
    
    QCommandLineParser parser;
    parser.setApplicationDescription("Qt资源加密工具 - 用于加密/解密QML和其他资源文件");
    parser.addHelpOption();
    parser.addVersionOption();
    
    // 定义命令行选项
    QCommandLineOption modeOption(QStringList() << "m" << "mode",
                                  "操作模式: encrypt(加密) 或 decrypt(解密)",
                                  "mode",
                                  "encrypt");
    parser.addOption(modeOption);
    
    QCommandLineOption inputOption(QStringList() << "i" << "input",
                                   "输入文件或目录路径",
                                   "input");
    parser.addOption(inputOption);
    
    QCommandLineOption outputOption(QStringList() << "o" << "output",
                                    "输出文件或目录路径",
                                    "output");
    parser.addOption(outputOption);
    
    QCommandLineOption keyOption(QStringList() << "k" << "key",
                                 "加密/解密密钥",
                                 "key",
                                 "DefaultKey123");
    parser.addOption(keyOption);
    
    QCommandLineOption extensionsOption(QStringList() << "e" << "extensions",
                                       "要处理的文件扩展名(逗号分隔,例如: .qml,.js,.qrc)",
                                       "extensions",
                                       ".qml,.js");
    parser.addOption(extensionsOption);
    
    QCommandLineOption directoryOption(QStringList() << "d" << "directory",
                                       "处理整个目录");
    parser.addOption(directoryOption);
    
    parser.process(app);
    
    // 获取参数
    QString mode = parser.value(modeOption);
    QString input = parser.value(inputOption);
    QString output = parser.value(outputOption);
    QString key = parser.value(keyOption);
    QString extensions = parser.value(extensionsOption);
    bool isDirectory = parser.isSet(directoryOption);
    
    // 验证参数
    if (input.isEmpty()) {
        qCritical() << "错误: 必须指定输入路径";
        parser.showHelp(1);
        return 1;
    }
    
    if (output.isEmpty()) {
        qCritical() << "错误: 必须指定输出路径";
        parser.showHelp(1);
        return 1;
    }
    
    qDebug() << "操作模式:" << mode;
    qDebug() << "输入路径:" << input;
    qDebug() << "输出路径:" << output;
    qDebug() << "使用密钥:" << (key.length() > 0 ? "***" : "无");
    
    // 执行操作
    if (isDirectory) {
        QStringList extList = extensions.split(',', Qt::SkipEmptyParts);
        
        if (mode == "encrypt") {
            int count = ResourceEncryptor::encryptDirectory(input, output, key, extList);
            qDebug() << "加密完成,处理了" << count << "个文件";
        } else if (mode == "decrypt") {
            qCritical() << "目录解密功能暂未实现";
            return 1;
        } else {
            qCritical() << "未知的操作模式:" << mode;
            return 1;
        }
    } else {
        bool success = false;
        
        if (mode == "encrypt") {
            success = ResourceEncryptor::encryptFile(input, output, key);
        } else if (mode == "decrypt") {
            success = ResourceEncryptor::decryptFile(input, output, key);
        } else {
            qCritical() << "未知的操作模式:" << mode;
            return 1;
        }
        
        if (success) {
            qDebug() << mode << "操作成功完成";
            return 0;
        } else {
            qCritical() << mode << "操作失败";
            return 1;
        }
    }
    
    return 0;
}

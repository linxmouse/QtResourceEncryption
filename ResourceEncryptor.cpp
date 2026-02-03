#include "ResourceEncryptor.h"
#include "ResourceEncryption.h"
#include <QFile>
#include <QDir>
#include <QDirIterator>
#include <QDebug>

bool ResourceEncryptor::encryptFile(const QString &inputPath, const QString &outputPath, const QString &key)
{
    QFile inputFile(inputPath);
    if (!inputFile.open(QIODevice::ReadOnly)) {
        qWarning() << "无法打开输入文件:" << inputPath;
        return false;
    }
    
    QByteArray data = inputFile.readAll();
    inputFile.close();
    
    QByteArray encryptedData = ResourceEncryption::encrypt(data, key);
    
    QFile outputFile(outputPath);
    if (!outputFile.open(QIODevice::WriteOnly)) {
        qWarning() << "无法创建输出文件:" << outputPath;
        return false;
    }
    
    outputFile.write(encryptedData);
    outputFile.close();
    
    qDebug() << "加密成功:" << inputPath << "->" << outputPath;
    return true;
}

bool ResourceEncryptor::decryptFile(const QString &inputPath, const QString &outputPath, const QString &key)
{
    QFile inputFile(inputPath);
    if (!inputFile.open(QIODevice::ReadOnly)) {
        qWarning() << "无法打开输入文件:" << inputPath;
        return false;
    }
    
    QByteArray encryptedData = inputFile.readAll();
    inputFile.close();
    
    QByteArray decryptedData = ResourceEncryption::decrypt(encryptedData, key);
    
    QFile outputFile(outputPath);
    if (!outputFile.open(QIODevice::WriteOnly)) {
        qWarning() << "无法创建输出文件:" << outputPath;
        return false;
    }
    
    outputFile.write(decryptedData);
    outputFile.close();
    
    qDebug() << "解密成功:" << inputPath << "->" << outputPath;
    return true;
}

int ResourceEncryptor::encryptDirectory(const QString &inputDir, const QString &outputDir, 
                                        const QString &key, const QStringList &extensions)
{
    QDir outDir(outputDir);
    if (!outDir.exists()) {
        outDir.mkpath(".");
    }
    
    int count = 0;
    QDirIterator it(inputDir, QDirIterator::Subdirectories);
    
    while (it.hasNext()) {
        QString filePath = it.next();
        QFileInfo fileInfo(filePath);
        
        if (fileInfo.isFile()) {
            QString suffix = "." + fileInfo.suffix();
            if (extensions.contains(suffix, Qt::CaseInsensitive)) {
                QString relativePath = QDir(inputDir).relativeFilePath(filePath);
                QString outputPath = outputDir + "/" + relativePath + ".enc";
                
                // 确保输出目录存在
                QDir().mkpath(QFileInfo(outputPath).absolutePath());
                
                if (encryptFile(filePath, outputPath, key)) {
                    count++;
                }
            }
        }
    }
    
    qDebug() << "总共加密了" << count << "个文件";
    return count;
}

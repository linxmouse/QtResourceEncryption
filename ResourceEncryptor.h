#ifndef RESOURCEENCRYPTOR_H
#define RESOURCEENCRYPTOR_H

#include <QString>
#include <QByteArray>

/**
 * @brief 资源加密工具类
 * 用于在构建时加密资源文件
 */
class ResourceEncryptor
{
public:
    /**
     * @brief 加密文件
     * @param inputPath 输入文件路径
     * @param outputPath 输出文件路径
     * @param key 加密密钥
     * @return 是否成功
     */
    static bool encryptFile(const QString &inputPath, const QString &outputPath, const QString &key);
    
    /**
     * @brief 解密文件
     * @param inputPath 输入文件路径
     * @param outputPath 输出文件路径
     * @param key 解密密钥
     * @return 是否成功
     */
    static bool decryptFile(const QString &inputPath, const QString &outputPath, const QString &key);
    
    /**
     * @brief 加密目录中的所有文件
     * @param inputDir 输入目录
     * @param outputDir 输出目录
     * @param key 加密密钥
     * @param extensions 要加密的文件扩展名(例如: .qml, .js)
     * @return 成功加密的文件数量
     */
    static int encryptDirectory(const QString &inputDir, const QString &outputDir, 
                               const QString &key, const QStringList &extensions);
};

#endif // RESOURCEENCRYPTOR_H

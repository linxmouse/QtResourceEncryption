# Qt资源加密方案 - 完整实现

## 项目概述

这是一个完整的Qt资源加密解决方案,支持对QML文件、JavaScript文件和其他资源进行加密保护。该方案可以有效防止应用程序的QML代码被轻易查看和修改。

## 核心特性

✅ **QML文件加密** - 支持加密QML文件
✅ **JavaScript加密** - 支持加密JS文件
✅ **动态解密** - 运行时自动解密加载
✅ **透明集成** - 对QML代码无侵入
✅ **多文件支持** - 支持批量加密目录
✅ **自定义密钥** - 支持自定义加密密钥

## 项目结构

```
├── ResourceEncryption.h/cpp          # 核心加密/解密类
├── EncryptedQmlFileSelector.h/cpp    # QML文件选择器
├── EncryptedNetworkAccessManager.h/cpp # 网络访问管理器
├── ResourceEncryptor.h/cpp           # 资源加密工具类
├── main.cpp                          # 主程序
├── encryptor_tool.cpp                # 命令行加密工具
├── main.qml                          # 示例QML文件
├── MyComponent.qml                   # 示例组件
├── qml.qrc                          # Qt资源文件
├── EncryptedQmlApp.pro              # qmake项目文件
├── ResourceEncryptorTool.pro        # 工具项目文件
└── CMakeLists.txt                   # CMake项目文件
```

## 工作原理

### 1. 加密阶段(构建时)

```
原始QML文件 → 加密工具 → 加密后的文件(.enc) → 嵌入到资源系统
```

### 2. 解密阶段(运行时)

```
应用启动 → 注册加密资源 → QML引擎请求资源 → 
拦截请求 → 解密数据 → 返回给QML引擎 → 正常渲染
```

### 关键技术点

1. **自定义NetworkAccessManager**: 拦截QML的资源加载请求
2. **EncryptedNetworkReply**: 提供解密后的数据流
3. **EncryptedQmlFileSelector**: 管理加密资源的映射
4. **XOR加密**: 简单快速的加密算法(可替换为AES)

## 使用方法

### 步骤1: 编译加密工具

```bash
mkdir build && cd build
cmake ..
cmake --build . --target resource_encryptor
```

### 步骤2: 加密资源文件

加密单个文件:
```bash
./resource_encryptor -m encrypt -i main.qml -o main.qml.enc -k "MySecretKey123"
```

加密整个目录:
```bash
./resource_encryptor -m encrypt -d -i ./qml -o ./encrypted -k "MySecretKey123" -e ".qml,.js"
```

### 步骤3: 将加密文件添加到资源系统

修改 qml.qrc:
```xml
<RCC>
    <qresource prefix="/encrypted">
        <file>main.qml.enc</file>
        <file>MyComponent.qml.enc</file>
    </qresource>
</RCC>
```

### 步骤4: 在应用中加载加密资源

参考 main.cpp 中的实现:

```cpp
// 创建加密文件选择器
EncryptedQmlFileSelector *selector = new EncryptedQmlFileSelector(&engine, DECRYPTION_KEY);

// 注册加密资源
QFile encFile(":/encrypted/main.qml.enc");
if (encFile.open(QIODevice::ReadOnly)) {
    QByteArray encryptedData = encFile.readAll();
    selector->registerEncryptedResource("main.qml", encryptedData);
}

// 设置自定义网络访问管理器
EncryptedNetworkAccessManager *nam = new EncryptedNetworkAccessManager(selector);
engine.setNetworkAccessManagerFactory([nam](QQmlEngine *) { return nam; });

// 加载解密后的QML
QByteArray decryptedMain = selector->getDecryptedResource("main.qml");
engine.loadData(decryptedMain, QUrl("encrypted://main.qml"));
```

## 安全性建议

### 当前实现(演示级别)
- 使用XOR加密,简单快速但安全性较低
- 密钥明文存储在代码中

### 生产环境建议

1. **使用AES-256加密**
   ```cpp
   // 替换为OpenSSL的AES实现
   #include <openssl/aes.h>
   #include <openssl/evp.h>
   ```

2. **密钥保护策略**
   - 使用硬件密钥存储(TPM)
   - 密钥混淆技术
   - 从服务器动态获取密钥
   - 使用白盒加密

3. **代码混淆**
   - 对C++代码进行混淆
   - 使用商业混淆工具

4. **完整性校验**
   - 添加HMAC验证
   - 防止篡改

## 增强方案

### 1. 整合OpenSSL实现AES加密

```cpp
// ResourceEncryption.cpp 中替换加密函数
QByteArray ResourceEncryption::encrypt(const QByteArray &data, const QString &key)
{
    // 使用OpenSSL的AES-256-CBC
    EVP_CIPHER_CTX *ctx = EVP_CIPHER_CTX_new();
    // ... AES加密实现
}
```

### 2. 支持分段解密

对于大型资源,可以实现按需解密:
```cpp
QByteArray decryptChunk(int offset, int size);
```

### 3. 资源缓存管理

```cpp
class ResourceCache {
    QCache<QString, QByteArray> cache;
    void cacheDecrypted(const QString &path, const QByteArray &data);
};
```

### 4. 多密钥支持

不同模块使用不同密钥:
```cpp
void registerEncryptedResource(const QString &path, 
                              const QByteArray &data, 
                              const QString &moduleKey);
```

## 性能优化

1. **预解密**: 应用启动时批量解密常用资源
2. **多线程**: 在后台线程进行解密操作
3. **内存映射**: 使用mmap减少内存拷贝
4. **压缩**: 先压缩再加密,减少数据量

## 测试方法

### 验证加密
```bash
# 加密文件
./resource_encryptor -m encrypt -i main.qml -o test.enc -k "test123"

# 解密验证
./resource_encryptor -m decrypt -i test.enc -o test_dec.qml -k "test123"

# 对比原文件
diff main.qml test_dec.qml
```

### 验证应用
1. 编译并运行应用
2. 检查QML界面是否正常显示
3. 查看日志确认资源被正确解密

## 常见问题

### Q1: 如何处理QML导入?
A: 确保导入的所有组件都被加密,并正确注册路径映射

### Q2: 性能影响如何?
A: XOR加密几乎无性能影响,AES会有轻微延迟,建议使用缓存

### Q3: 能否防止反编译?
A: 这个方案增加了逆向难度,但不能完全防止。需配合代码混淆使用

### Q4: 支持热更新吗?
A: 支持,可以从服务器下载加密资源,动态注册并加载

## 许可证

本项目仅供学习和参考使用。

## 技术支持

如有问题,请参考:
- Qt官方文档: https://doc.qt.io
- QML文档: https://doc.qt.io/qt-6/qmlapplications.html

## 更新日志

**v1.0** (2024-02)
- 初始版本
- 支持QML/JS加密
- 命令行工具
- 示例应用

## 后续计划

- [ ] 集成AES-256加密
- [ ] 添加GUI加密工具
- [ ] 支持图片资源加密
- [ ] 实现资源完整性校验
- [ ] 添加授权验证机制

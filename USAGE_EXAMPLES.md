# Qt资源加密方案 - 使用示例

## 快速开始

### 1. 准备工作

确保已安装Qt开发环境:
- Qt 5.15+ 或 Qt 6.x
- QtCreator (可选)
- C++17编译器

### 2. 构建项目

#### Linux/Mac:
```bash
chmod +x build.sh
./build.sh
```

#### Windows:
```cmd
build.bat
```

#### 手动构建:
```bash
# 构建加密工具
cd build
qmake ../ResourceEncryptorTool.pro
make

# 加密资源
./resource_encryptor -m encrypt -i ../main.qml -o ../encrypted/main.qml.enc -k "YourKey"

# 构建应用
qmake ../EncryptedQmlApp.pro
make
```

## 使用场景

### 场景1: 加密整个QML应用

```bash
# 1. 准备QML源文件目录
mkdir qml_source
# 将所有QML文件放入 qml_source/

# 2. 批量加密
./resource_encryptor -d -m encrypt \
  -i qml_source \
  -o encrypted_resources \
  -k "ProductionKey2024" \
  -e ".qml,.js"

# 3. 更新qml.qrc,添加加密后的文件

# 4. 在main.cpp中注册所有加密资源
```

### 场景2: 选择性加密重要文件

只加密核心业务逻辑文件:

```cpp
// main.cpp
void loadEncryptedResources(EncryptedQmlFileSelector *selector, const QString &key)
{
    // 只加密包含业务逻辑的核心文件
    registerEncryptedFile(selector, "BusinessLogic.qml", key);
    registerEncryptedFile(selector, "SecureComponent.qml", key);
    registerEncryptedFile(selector, "LicenseValidator.js", key);
    
    // UI文件可以不加密,节省性能
}
```

### 场景3: 多模块不同密钥

为不同模块使用不同的加密密钥:

```cpp
// 扩展EncryptedQmlFileSelector支持多密钥
class MultiKeySelector : public EncryptedQmlFileSelector {
public:
    void registerEncryptedResource(const QString &path, 
                                   const QByteArray &data,
                                   const QString &moduleKey) {
        // 存储模块密钥映射
        m_moduleKeys[path] = moduleKey;
        m_encryptedResources[path] = data;
    }
    
private:
    QHash<QString, QString> m_moduleKeys;
};

// 使用
MultiKeySelector *selector = new MultiKeySelector(&engine, defaultKey);
selector->registerEncryptedResource("core/Business.qml", data1, "CoreKey123");
selector->registerEncryptedResource("ui/Login.qml", data2, "UIKey456");
```

## 高级用法

### 1. 集成到构建系统

#### qmake示例:

```qmake
# EncryptedQmlApp.pro

# 定义加密密钥
ENCRYPTION_KEY = "MyProductKey2024"

# 在构建前加密资源
encrypt_resources.commands = \
    $$PWD/build/resource_encryptor \
    -d -m encrypt \
    -i $$PWD/qml \
    -o $$PWD/encrypted \
    -k $$ENCRYPTION_KEY \
    -e ".qml,.js"

QMAKE_EXTRA_TARGETS += encrypt_resources
PRE_TARGETDEPS += encrypt_resources
```

#### CMake示例:

```cmake
# CMakeLists.txt

# 添加自定义命令加密资源
add_custom_command(
    OUTPUT ${CMAKE_SOURCE_DIR}/encrypted/main.qml.enc
    COMMAND resource_encryptor 
        -m encrypt 
        -i ${CMAKE_SOURCE_DIR}/main.qml 
        -o ${CMAKE_SOURCE_DIR}/encrypted/main.qml.enc
        -k "MyKey"
    DEPENDS ${CMAKE_SOURCE_DIR}/main.qml
    COMMENT "Encrypting QML resources..."
)

add_custom_target(EncryptResources 
    DEPENDS ${CMAKE_SOURCE_DIR}/encrypted/main.qml.enc
)

add_dependencies(EncryptedQmlApp EncryptResources)
```

### 2. 动态密钥生成

基于硬件信息生成密钥:

```cpp
#include <QSysInfo>
#include <QCryptographicHash>

QString generateHardwareKey() {
    QString machineId = QSysInfo::machineUniqueId();
    QString productType = QSysInfo::productType();
    
    QString combined = machineId + productType + "SaltValue";
    
    return QString(QCryptographicHash::hash(
        combined.toUtf8(), 
        QCryptographicHash::Sha256
    ).toHex());
}

// 使用
QString dynamicKey = generateHardwareKey();
EncryptedQmlFileSelector *selector = 
    new EncryptedQmlFileSelector(&engine, dynamicKey);
```

### 3. 网络授权集成

从服务器获取解密密钥:

```cpp
class LicenseManager : public QObject {
    Q_OBJECT
public:
    void requestDecryptionKey(const QString &licenseCode) {
        QNetworkRequest request(QUrl("https://api.example.com/getKey"));
        request.setHeader(QNetworkRequest::ContentTypeHeader, 
                         "application/json");
        
        QJsonObject json;
        json["license"] = licenseCode;
        json["hwid"] = getHardwareId();
        
        QNetworkReply *reply = manager->post(request, 
                                            QJsonDocument(json).toJson());
        connect(reply, &QNetworkReply::finished, this, [=]() {
            if (reply->error() == QNetworkReply::NoError) {
                QJsonDocument doc = QJsonDocument::fromJson(
                    reply->readAll());
                QString key = doc.object()["key"].toString();
                emit keyReceived(key);
            }
            reply->deleteLater();
        });
    }

signals:
    void keyReceived(const QString &key);
    
private:
    QNetworkAccessManager *manager;
};

// 在main.cpp中使用
LicenseManager *license = new LicenseManager();
connect(license, &LicenseManager::keyReceived, [&](const QString &key) {
    EncryptedQmlFileSelector *selector = 
        new EncryptedQmlFileSelector(&engine, key);
    loadEncryptedResources(selector, key);
    startApplication();
});
license->requestDecryptionKey(userLicenseCode);
```

### 4. 资源完整性校验

添加SHA256校验防止篡改:

```cpp
class SecureResourceLoader {
public:
    struct EncryptedResource {
        QByteArray data;
        QString sha256Hash;
    };
    
    bool loadAndVerify(const QString &path, const QString &key) {
        EncryptedResource res = loadResource(path);
        
        // 验证完整性
        QString computedHash = QString(QCryptographicHash::hash(
            res.data, QCryptographicHash::Sha256).toHex());
        
        if (computedHash != res.sha256Hash) {
            qWarning() << "资源完整性校验失败:" << path;
            return false;
        }
        
        // 解密
        QByteArray decrypted = ResourceEncryption::decrypt(res.data, key);
        registerResource(path, decrypted);
        return true;
    }
};
```

### 5. 性能优化 - 预解密缓存

```cpp
class OptimizedResourceManager {
public:
    OptimizedResourceManager(const QString &key) : m_key(key) {
        // 在后台线程预解密常用资源
        QtConcurrent::run([this]() {
            preDecryptCommonResources();
        });
    }
    
    void preDecryptCommonResources() {
        QStringList commonFiles = {
            "main.qml",
            "HomePage.qml",
            "NavigationBar.qml"
        };
        
        for (const QString &file : commonFiles) {
            QByteArray encrypted = loadEncryptedFile(file);
            QByteArray decrypted = ResourceEncryption::decrypt(
                encrypted, m_key);
            
            QMutexLocker locker(&m_cacheMutex);
            m_cache.insert(file, decrypted);
        }
    }
    
    QByteArray getResource(const QString &path) {
        QMutexLocker locker(&m_cacheMutex);
        if (m_cache.contains(path)) {
            return m_cache.value(path);
        }
        
        // 未缓存,即时解密
        locker.unlock();
        QByteArray encrypted = loadEncryptedFile(path);
        return ResourceEncryption::decrypt(encrypted, m_key);
    }
    
private:
    QString m_key;
    QCache<QString, QByteArray> m_cache;
    QMutex m_cacheMutex;
};
```

## 最佳实践

### 1. 密钥管理

❌ **不要这样做**:
```cpp
const QString KEY = "hardcoded_password_123";  // 明文硬编码
```

✅ **推荐做法**:
```cpp
// 混淆密钥
const char KEY_PARTS[] = {0x4D, 0x79, 0x53, 0x65, ...};
QString reconstructKey() {
    return QString::fromLatin1(KEY_PARTS, sizeof(KEY_PARTS));
}

// 或使用环境变量
QString key = qgetenv("APP_DECRYPT_KEY");

// 或从硬件生成
QString key = generateHardwareBasedKey();
```

### 2. 分层加密策略

```
├── 核心业务逻辑 (强加密 + 校验)
├── UI组件 (轻量加密)
└── 资源文件 (可选加密)
```

### 3. 错误处理

```cpp
try {
    QByteArray decrypted = selector->getDecryptedResource(path);
    if (decrypted.isEmpty()) {
        // 回退到默认资源
        engine.load(QUrl("qrc:/fallback/default.qml"));
    }
} catch (const std::exception &e) {
    qCritical() << "解密失败:" << e.what();
    // 记录日志并优雅退出
}
```

### 4. 调试模式

```cpp
#ifdef QT_DEBUG
    // 调试模式下跳过加密,方便开发
    engine.load(QUrl("qrc:/main.qml"));
#else
    // 发布模式使用加密
    loadEncryptedResources(selector, key);
#endif
```

## 故障排查

### 问题1: QML加载失败

**症状**: 黑屏或提示找不到QML文件

**检查**:
1. 确认加密文件已正确添加到qrc
2. 检查解密密钥是否正确
3. 验证文件路径映射

**调试**:
```cpp
qDebug() << "Available resources:";
QDirIterator it(":", QDirIterator::Subdirectories);
while (it.hasNext()) {
    qDebug() << it.next();
}
```

### 问题2: 资源解密后乱码

**原因**: 密钥不匹配或文件损坏

**解决**:
```bash
# 验证加密/解密往返
./resource_encryptor -m encrypt -i test.qml -o test.enc -k "key123"
./resource_encryptor -m decrypt -i test.enc -o test_dec.qml -k "key123"
diff test.qml test_dec.qml  # 应该无差异
```

### 问题3: 性能下降

**优化措施**:
1. 启用资源缓存
2. 预解密常用文件
3. 使用更快的加密算法
4. 考虑只加密关键文件

## 扩展阅读

- Qt QML引擎原理
- 加密算法选择 (AES vs XOR vs ChaCha20)
- 代码混淆技术
- 软件授权管理

## 示例项目

完整的示例应用包含:
- ✅ 登录界面加密
- ✅ 业务逻辑加密
- ✅ 配置文件加密
- ✅ 网络授权集成
- ✅ 完整性校验

可作为实际项目的模板使用。

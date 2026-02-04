# Qt 资源加密方案 (Qt Resource Encryption)

本项目提供了一套完整的 Qt/QML 资源加密解决方案，支持对 QML、JavaScript、图片等多种资源进行加密，并通过自定义网络协议 (`encrypted:///`) 在运行时透明解密加载，有效保护您的代码资产。

## 核心特性

- **协议级拦截**: 使用 `QNetworkAccessManager` 拦截自定义 `encrypted:` 协议，完全不干扰 `qrc:` 等内置协议。
- **纯内存处理**: 资源在内存中实时解密并提供给引擎，不产生临时文件，安全性高。
- **多资源支持**: 透明支持 QML、JS、PNG/JPG 图像等所有通过 URL 加载的资源。
- **高性能**: 采用高效的加密算法（演示版为 XOR，可扩展为 AES），并支持资源 MIME 类型自动识别。
- **异步兼容**: 完美支持自定义协议的异步加载特性，适配 `QQmlApplicationEngine` 的 `objectCreated` 信号。

## 项目结构

```text
├── ResourceEncryption.h/cpp          # 加密/解密算法实现 (核心)
├── EncryptedResourceSelector.h/cpp    # 资源注册中心，管理解密后的内存数据
├── EncryptedNetworkAccessManager.h/cpp # 自定义 NetworkAccessManager 及 Reply 实现
├── ResourceEncryptor.h/cpp           # 批量处理文件/目录的工具类
├── encryptor_tool.cpp                # 命令行加密工具入口
├── main.cpp                          # 示例：如何初始化与集成
├── main.qml                          # 示例：通过自定义协议引用资源
└── CMakeLists.txt                    # 项目构建配置 (支持 Qt 6)
```

## 快速开始

### 1. 编译项目
使用 CMake 构建项目：
```bash
mkdir build && cd build
cmake ..
cmake --build . --config Debug
```

### 2. 使用加密工具
您可以将原始资源加密为 `.enc` 文件：
```bash
# 加密单个文件
resource_encryptor.exe -m encrypt -i main.qml -o main.qml.enc -k "YourKey123"

# 批量加密目录
resource_encryptor.exe -m encrypt -d -i ./qml_src -o ./encrypted -k "YourKey123" -e ".qml,.js,.png"
```

### 3. 程序集成

在 `main.cpp` 中按以下顺序集成：

```cpp
// 1. 创建资源选择器
auto selector = new EncryptedResourceSelector(&engine, "YourKey123");

// 2. 注册加密后的资源 (数据可以来自 qrc 里的 .enc 文件)
QFile file(":/encrypted/main.qml.enc");
if (file.open(QIODevice::ReadOnly)) {
    selector->registerEncryptedResource("main.qml", file.readAll());
}

// 3. 安装自定义网络工厂
engine.setNetworkAccessManagerFactory(new EncryptedNetworkAccessManagerFactory(selector));

// 4. 使用 encrypted:/// 协议加载 (主意：必须包含此协议前缀)
engine.load(QUrl("encrypted:///main.qml"));
```

## 关键代码说明

### 自定义协议处理
我们在 `EncryptedNetworkAccessManager` 中只处理 `url.scheme() == "encrypted"` 的请求。
- 路径 `encrypted:///test.png` 会被映射到注册名为 `test.png` 的内存数据。
- 所有的相对路径引用（例如 QML 中的 `Image { source: "test.png" }`）在 `encrypted:///main.qml` 环境下会自动补充为以 `encrypted:` 开头的请求，从而实现透明加载。

### Content-Type 识别
`EncryptedNetworkReply` 会根据请求的文件后缀自动设置 `Content-Type`（如 `text/plain` 或 `image/png`），确保 QML 引擎能正确识别数据类型。

## 安全性建议

1. **更换算法**: 生产环境建议将 `ResourceEncryption` 中的 XOR 算法替换为标准的 `AES-256-CBC` 或 `GCM`。
2. **密钥混淆**: 不要将密钥明文写在代码中，建议使用简单的混淆、从服务器拉取或使用环境变量。
3. **启用断言**: 在发布版本中可以添加资源完整性校验（如散列值匹配）。

## 许可证
本项目仅供学习与交流使用。

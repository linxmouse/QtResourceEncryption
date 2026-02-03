# Qt资源加密方案 - 项目结构

## 文件清单

### 核心源代码
- `ResourceEncryption.h/cpp` - 加密/解密核心实现
- `EncryptedQmlFileSelector.h/cpp` - QML文件选择器
- `EncryptedNetworkAccessManager.h/cpp` - 网络访问管理器拦截
- `ResourceEncryptor.h/cpp` - 资源加密工具类

### 应用程序
- `main.cpp` - 主应用程序入口
- `encryptor_tool.cpp` - 命令行加密工具

### QML文件
- `main.qml` - 主界面示例
- `MyComponent.qml` - 组件示例

### 项目配置
- `EncryptedQmlApp.pro` - 主应用qmake项目文件
- `ResourceEncryptorTool.pro` - 加密工具qmake项目文件
- `CMakeLists.txt` - CMake项目文件
- `qml.qrc` - Qt资源文件

### 构建脚本
- `build.sh` - Linux/Mac构建脚本
- `build.bat` - Windows构建脚本

### 文档
- `README.md` - 项目说明文档
- `USAGE_EXAMPLES.md` - 使用示例和最佳实践
- `PROJECT_STRUCTURE.md` - 本文件

## 目录结构建议

```
qt-resource-encryption/
├── src/                      # 源代码
│   ├── core/                # 核心加密模块
│   │   ├── ResourceEncryption.h
│   │   ├── ResourceEncryption.cpp
│   │   ├── ResourceEncryptor.h
│   │   └── ResourceEncryptor.cpp
│   ├── qml/                 # QML支持
│   │   ├── EncryptedQmlFileSelector.h
│   │   ├── EncryptedQmlFileSelector.cpp
│   │   ├── EncryptedNetworkAccessManager.h
│   │   └── EncryptedNetworkAccessManager.cpp
│   └── tools/               # 工具
│       └── encryptor_tool.cpp
├── qml/                     # QML源文件
│   ├── main.qml
│   └── components/
│       └── MyComponent.qml
├── encrypted/               # 加密后的资源(构建时生成)
├── resources/               # 其他资源
│   └── qml.qrc
├── build/                   # 构建目录(构建时生成)
├── docs/                    # 文档
│   ├── README.md
│   └── USAGE_EXAMPLES.md
├── scripts/                 # 构建脚本
│   ├── build.sh
│   └── build.bat
├── main.cpp                 # 应用入口
├── EncryptedQmlApp.pro     # qmake项目
├── ResourceEncryptorTool.pro
└── CMakeLists.txt          # CMake项目
```

## 编译顺序

1. 构建加密工具 (`resource_encryptor`)
2. 使用工具加密QML资源
3. 构建主应用程序 (`EncryptedQmlApp`)

## 依赖关系

```
EncryptedQmlApp
  ├── ResourceEncryption (加密核心)
  ├── EncryptedQmlFileSelector (资源管理)
  └── EncryptedNetworkAccessManager (资源拦截)

resource_encryptor
  ├── ResourceEncryption (加密核心)
  └── ResourceEncryptor (工具类)
```

## Qt模块依赖

- Qt Core (必需)
- Qt GUI (主应用)
- Qt QML (主应用)
- Qt Quick (主应用)
- Qt Network (主应用)

## 最小Qt版本

- Qt 5.15 或更高
- Qt 6.x (推荐)

# Qt版本兼容性说明

## 问题描述

在Qt 6中编译时遇到以下错误:

```
error C2664: "void QQmlEngine::setNetworkAccessManagerFactory(QQmlNetworkAccessManagerFactory *)": 
无法将参数 1 从"main::<lambda_1>"转换为"QQmlNetworkAccessManagerFactory *"
```

## 原因分析

Qt 6移除了对lambda函数作为NetworkAccessManagerFactory的支持。在Qt 5中可以这样写:

```cpp
// Qt 5 可以,Qt 6 不行
engine.setNetworkAccessManagerFactory([nam](QQmlEngine *) -> QNetworkAccessManager* {
    return nam;
});
```

但在Qt 6中,`setNetworkAccessManagerFactory`只接受`QQmlNetworkAccessManagerFactory*`指针。

## 解决方案

### 方案1: 使用工厂类(推荐 - Qt 5和Qt 6都兼容)

创建一个继承自`QQmlNetworkAccessManagerFactory`的工厂类:

```cpp
class EncryptedNetworkAccessManagerFactory : public QQmlNetworkAccessManagerFactory
{
public:
    explicit EncryptedNetworkAccessManagerFactory(EncryptedQmlFileSelector *selector)
        : m_selector(selector)
    {
    }
    
    QNetworkAccessManager *create(QObject *parent) override
    {
        return new EncryptedNetworkAccessManager(m_selector, parent);
    }
    
private:
    EncryptedQmlFileSelector *m_selector;
};

// 使用
EncryptedNetworkAccessManagerFactory *factory = 
    new EncryptedNetworkAccessManagerFactory(selector);
engine.setNetworkAccessManagerFactory(factory);
```

### 方案2: 条件编译

如果需要同时支持Qt 5和Qt 6:

```cpp
#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
    // Qt 5: 可以使用lambda
    engine.setNetworkAccessManagerFactory([nam](QQmlEngine *) {
        return nam;
    });
#else
    // Qt 6: 必须使用工厂类
    EncryptedNetworkAccessManagerFactory *factory = 
        new EncryptedNetworkAccessManagerFactory(selector);
    engine.setNetworkAccessManagerFactory(factory);
#endif
```

但是**推荐直接使用方案1**,因为它在两个版本中都能工作。

## 已修复的文件

项目中提供了两个版本的main.cpp:

1. **main.cpp** - 使用工厂类,Qt 5和Qt 6都兼容(推荐)
2. **main_compatible.cpp** - 带条件编译的版本(参考)

## 其他Qt 6兼容性注意事项

### 1. 高DPI支持

Qt 6中默认启用高DPI,无需设置:

```cpp
#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
    QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
#endif
```

### 2. QML模块导入

Qt 6中某些模块路径发生变化:

```qml
// Qt 5
import QtQuick 2.15
import QtQuick.Controls 2.15

// Qt 6
import QtQuick
import QtQuick.Controls
```

### 3. CMake配置

Qt 6使用新的CMake API:

```cmake
# Qt 5
find_package(Qt5 COMPONENTS Core Gui Qml Quick REQUIRED)
target_link_libraries(app Qt5::Core Qt5::Gui Qt5::Qml Qt5::Quick)

# Qt 6
find_package(Qt6 COMPONENTS Core Gui Qml Quick REQUIRED)
target_link_libraries(app Qt6::Core Qt6::Gui Qt6::Qml Qt6::Quick)
```

项目中的CMakeLists.txt已经使用Qt 6的方式,如果需要Qt 5,请相应修改。

## 编译测试

### Windows (MSVC)

```cmd
mkdir build
cd build
cmake .. -G "Visual Studio 17 2022" -A x64
cmake --build . --config Release
```

### Linux/Mac

```bash
mkdir build
cd build
cmake ..
cmake --build .
```

### Qt Creator

直接打开`.pro`文件,选择合适的Qt版本套件即可。

## 运行测试

编译成功后:

1. 确保有加密的资源文件或使用普通QML测试
2. 运行应用程序
3. 查看控制台输出确认资源加载状态

## 常见问题

### Q: 还是编译失败怎么办?

A: 请检查:
1. Qt版本是否正确(建议Qt 6.2+)
2. 是否包含了QQmlNetworkAccessManagerFactory头文件
3. 项目配置中是否正确链接了Qt Network模块

### Q: 运行时提示找不到QML文件?

A: 因为加密资源示例需要先运行加密工具。可以:
1. 注释掉加密资源加载部分
2. 直接加载普通QML: `engine.load(QUrl("qrc:/main.qml"));`
3. 先按README说明加密资源文件

## 完整编译步骤

```bash
# 1. 构建加密工具
cd build
qmake ../ResourceEncryptorTool.pro
make

# 2. 加密QML文件(可选)
./resource_encryptor -m encrypt -i ../main.qml -o ../encrypted/main.qml.enc -k "MySecretKey123!@#"

# 3. 构建主应用
qmake ../EncryptedQmlApp.pro
make

# 4. 运行
./EncryptedQmlApp
```

## 更新日志

**2024-02-03**
- 修复Qt 6兼容性问题
- 使用QQmlNetworkAccessManagerFactory替代lambda
- 添加Qt 5/6兼容性说明

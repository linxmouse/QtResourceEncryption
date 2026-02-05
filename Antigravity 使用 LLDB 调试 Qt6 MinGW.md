# Antigravity 使用 LLDB 调试 Qt6 MinGW

### 开发环境

- **编辑器**: Antigravity
- **调试器扩展**: CodeLLDB (`vadimcn.vscode-lldb-1.12.1`)
- **Qt 版本**: Qt 6.10.2
- **编译器**: MinGW 13.1.0 64-bit
- **操作系统**: Windows

### 关键路径

```bash
Qt SDK:           D:\Qt\6.10.2\mingw_64\
Qt Creator:       D:\Qt\Tools\QtCreator\
MinGW:            D:\Qt\Tools\mingw1310_64\
CodeLLDB Python:  c:\Users\YourUserName\.antigravity\extensions\vadimcn.vscode-lldb-1.12.1\lldb\Lib\site-packages
```

## 核心解决方案

### 问题背景

在 Antigravity 中使用 LLDB 调试 Qt 程序时，默认无法正确显示 `QString`、`QList`、`QMap` 等 Qt 类型的内容。

### 解决方案概述

通过两个关键步骤实现完美的 Qt 类型调试支持：

1. **安装 pygdbmi** - Qt 官方 lldbbridge.py 脚本的依赖
2. **配置 lldbbridge.py** - 使用 Qt Creator 自带的官方调试脚本

这种方案的优势：

- ✅ **完整的 Qt 类型支持** - 所有 Qt 类型开箱即用
- ✅ **官方维护** - Qt 官方维护，跟随 Qt 版本更新
- ✅ **零维护成本** - 不需要手动编写和维护格式化脚本
- ✅ **最佳兼容性** - 与 Qt Creator 使用相同的调试体验

------

## 详细安装步骤

### launch.json 初始配置

在 `.vscode/launch.json` 中配置：

```json
{
    // Use IntelliSense to learn about possible attributes.
    // Hover to view descriptions of existing attributes.
    // For more information, visit: https://go.microsoft.com/fwlink/?linkid=830387
    "version": "0.2.0",
    "configurations": [
        {
            "name": "Debug Qt6MinGW",
            "type": "lldb",
            "request": "launch",
            "program": "${command:cmake.launchTargetPath}",
            "args": [],
            "cwd": "${workspaceFolder}",        
            // 环境变量 - 添加 Qt 和 MinGW 路径
            "env": {
                "PATH": "${env:PATH};D:\\Qt\\6.10.2\\mingw_64\\bin;D:\\Qt\\Tools\\mingw1310_64\\bin"
            }
        }
    ]
}
```

### 步骤 1: 定位 CodeLLDB 的 Python 环境

CodeLLDB 使用内置的 Python 环境，路径为：

```bash
c:\Users\Administrator\.antigravity\extensions\vadimcn.vscode-lldb-1.12.1\lldb\Lib\site-packages
```

> **注意**：这个路径会因为：用户名不同、计算机名不同、CodeLLDB 版本不同而有所变化。

#### 如何找到你的 Python site-packages 路径？

启动lldb调试器后，在DebugConsole窗口执行命令获取：

```python
script sys.executable
'c:\\Users\\Administrator\\.antigravity\\extensions\\vadimcn.vscode-lldb-1.12.1\\adapter\\codelldb.exe'
script sys.version
'3.12.7 (main, Oct 16 2024, 00:21:24) [MSC v.1929 64 bit (AMD64)]'
script sys.path
['c:/Users/Administrator/.antigravity/extensions/vadimcn.vscode-lldb-1.12.1/lldb/bin', 'c:/Users/Administrator/.antigravity/extensions/vadimcn.vscode-lldb-1.12.1', 'D:\\Qt\\Tools\\QtCreator\\share\\qtcreator\\debugger', 'D:/Qt/Tools/QtCreator/share/qtcreator/debugger', 'c:/Users/Administrator/.antigravity/extensions/vadimcn.vscode-lldb-1.12.1/adapter/scripts', 'c:/Users/Administrator/.antigravity/extensions/vadimcn.vscode-lldb-1.12.1/lldb/lib/lldb-python', 'c:\\Users\\Administrator\\.antigravity\\extensions\\vadimcn.vscode-lldb-1.12.1\\lldb\\bin\\python312.zip', 'c:\\Users\\Administrator\\.antigravity\\extensions\\vadimcn.vscode-lldb-1.12.1\\lldb\\DLLs', 'c:\\Users\\Administrator\\.antigravity\\extensions\\vadimcn.vscode-lldb-1.12.1\\lldb\\Lib', 'c:\\Users\\Administrator\\.antigravity\\extensions\\vadimcn.vscode-lldb-1.12.1\\adapter', 'c:\\Users\\Administrator\\.antigravity\\extensions\\vadimcn.vscode-lldb-1.12.1\\lldb', 'c:\\Users\\Administrator\\.antigravity\\extensions\\vadimcn.vscode-lldb-1.12.1\\lldb\\Lib\\site-packages', '.']
```

### 步骤 2: 安装 pygdbmi 模块

#### 在Antigravity的Terminal执行安装命令

```bash
pip install pygdbmi --target="c:\Users\Administrator\.antigravity\extensions\vadimcn.vscode-lldb-1.12.1\lldb\Lib\site-packages" --no-deps
```

**重要参数说明：**

- `--target` - 指定安装到 CodeLLDB 的 Python 环境
- `--no-deps` - 不安装依赖包（pygdbmi 没有必需依赖）

## 配置调试器

### 项目结构

```
your-qt-project/
├── .vscode/
│   └── launch.json          # 调试配置
├── CMakeLists.txt
├── main.cpp
└── ...
```

### launch.json 完整配置

在 `.vscode/launch.json` 中配置：

```json
{
    // Use IntelliSense to learn about possible attributes.
    // Hover to view descriptions of existing attributes.
    // For more information, visit: https://go.microsoft.com/fwlink/?linkid=830387
    "version": "0.2.0",
    "configurations": [
        {
            "name": "Debug Qt6MinGW",
            "type": "lldb",
            "request": "launch",
            "program": "${command:cmake.launchTargetPath}",
            "args": [],
            "cwd": "${workspaceFolder}",        
            // 环境变量 - 添加 Qt 和 MinGW 路径
            "env": {
                "PATH": "${env:PATH};D:\\Qt\\6.10.2\\mingw_64\\bin;D:\\Qt\\Tools\\mingw1310_64\\bin"
            },
            // 初始化命令 - 加载 Qt 官方调试脚本
            "initCommands": [
                "command script import D:\\Qt\\Tools\\QtCreator\\share\\qtcreator\\debugger\\lldbbridge.py"
            ]
        }
    ]
}
```

### 配置说明

#### 1. program

```jsonc
"program": "${command:cmake.launchTargetPath}"
```

- 自动获取 CMake 构建的目标可执行文件路径
- 确保 CMake Tools 扩展已安装并配置

#### 2. env - 环境变量

```jsonc
"env": {
    "PATH": "${env:PATH};D:\\Qt\\6.10.2\\mingw_64\\bin;D:\\Qt\\Tools\\mingw1310_64\\bin"
}
```

- 添加 Qt 的 DLL 路径
- 添加 MinGW 的运行时库路径
- 确保程序能找到所需的动态链接库

#### 3. initCommands - Qt 调试脚本

```jsonc
"initCommands": [
    "command script import D:\\Qt\\Tools\\QtCreator\\share\\qtcreator\\debugger\\lldbbridge.py"
]
```

**路径格式注意事项：**

- Windows 路径可以使用 **双反斜杠** `\\`
- 或使用 **正斜杠** `/` （推荐，更简洁）
- 不要使用单反斜杠 `\` （会被转义）

**验证脚本路径是否存在：**

```bash
dir "D:\Qt\Tools\QtCreator\share\qtcreator\debugger\lldbbridge.py"
```

### CMakeLists.txt 示例

```cmake
cmake_minimum_required(VERSION 3.16)
project(MyQtApp VERSION 1.0 LANGUAGES CXX)

# C++ 标准
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Qt 路径（根据实际情况修改）
set(CMAKE_PREFIX_PATH "D:/Qt/6.10.2/mingw_64")

# 查找 Qt 组件
find_package(Qt6 REQUIRED COMPONENTS 
    Core 
    Widgets
    # 根据需要添加其他组件：Gui, Network, Sql 等
)

# 自动处理 Qt 元对象编译
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)

# 源文件
add_executable(${PROJECT_NAME}
    main.cpp
    # 添加其他源文件
)

# 链接 Qt 库
target_link_libraries(${PROJECT_NAME}
    Qt6::Core
    Qt6::Widgets
)

# Debug 模式配置
if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    # 启用调试信息
    if(MSVC)
        target_compile_options(${PROJECT_NAME} PRIVATE /Zi)
    else()
        target_compile_options(${PROJECT_NAME} PRIVATE -g)
    endif()
endif()

# Windows 配置（可选）
if(WIN32)
    # 设置为 GUI 应用（不显示控制台）
    # set_target_properties(${PROJECT_NAME} PROPERTIES WIN32_EXECUTABLE TRUE)
endif()
```

### 测试代码

创建一个测试文件 `main.cpp`：

```cpp
#include <QApplication>
#include <QMainWindow>
#include <QString>
#include <QList>
#include <QMap>
#include <QVariant>
#include <QDebug>

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    
    // 测试各种 Qt 类型
    QString str1 = "Hello Qt6!";
    QString str2 = "你好，世界！";
    QString empty = "";
    
    QByteArray bytes = "Test ByteArray";
    
    QList<int> numbers = {1, 2, 3, 4, 5};
    QList<QString> strings = {"Apple", "Banana", "Cherry"};
    
    QMap<QString, int> ages;
    ages["Alice"] = 30;
    ages["Bob"] = 25;
    ages["Charlie"] = 35;
    
    QVariant variant1 = 42;
    QVariant variant2 = "String in QVariant";
    QVariant variant3 = 3.14;
    
    // 在这里设置断点进行测试
    qDebug() << "Testing Qt types debugging...";
    qDebug() << str1;
    qDebug() << "Numbers:" << numbers;
    qDebug() << "Ages:" << ages;
    
    // 简单窗口（可选）
    QMainWindow window;
    window.setWindowTitle("Qt Debug Test");
    window.resize(400, 300);
    window.show();
    
    return app.exec();
}
```

### 调试控制台命令

在调试时，可以在 Debug Console 中使用 LLDB 命令：

```lldb
# 打印 QString 变量
p str1

# 查看 QList 内容
p numbers

# 查看变量类型
ptype str1

# 查看已加载的 Qt 格式化器
type summary list QString
type summary list QList

# 查看所有 Qt 相关的格式化器
type category list
```

### 为什么需要 pygdbmi？

#### Qt 官方脚本的依赖关系

`lldbbridge.py` 是 Qt Creator 提供的 LLDB 调试辅助脚本，它的作用是：

1. **注册 Qt 类型格式化器**
   - QString、QByteArray 等基础类型
   - QList、QVector、QMap 等容器类型
   - QVariant、QObject 等核心类型
2. **提供类型提示和可视化**
   - 展开容器内容
   - 显示对象层次结构
   - 格式化显示复杂类型
3. **依赖 pygdbmi 模块**
   - `pygdbmi` 是 GDB Machine Interface 的 Python 实现
   - 虽然名字带 "gdb"，但在 LLDB 中用于解析调试器输出
   - 提供了统一的调试器通信接口

### 完整的测试项目模板

#### 项目结构

```
qt-debug-test/
├── .vscode/
│   └── launch.json
├── CMakeLists.txt
├── main.cpp
└── README.md
```

#### main.cpp

```cpp
#include <QApplication>
#include <QMainWindow>
#include <QPushButton>
#include <QString>
#include <QList>
#include <QMap>
#include <QVariant>
#include <QDebug>

class TestWindow : public QMainWindow {
    Q_OBJECT
public:
    TestWindow() {
        setWindowTitle("Qt Debug Test");
        resize(400, 300);
        
        auto button = new QPushButton("Click to Test", this);
        button->setGeometry(100, 100, 200, 50);
        connect(button, &QPushButton::clicked, this, &TestWindow::onButtonClicked);
    }
    
private slots:
    void onButtonClicked() {
        QString message = "Button clicked!";
        QList<int> numbers = {1, 2, 3, 4, 5};
        
        qDebug() << message;  // 在这里设置断点
        qDebug() << "Numbers:" << numbers;
    }
};

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    
    // 测试各种类型
    QString str = "Hello Qt6!";
    QList<QString> list = {"Apple", "Banana", "Cherry"};
    QMap<QString, int> map = {{"A", 1}, {"B", 2}, {"C", 3}};
    
    qDebug() << str;  // 设置断点测试
    
    TestWindow window;
    window.show();
    
    return app.exec();
}
```

### LLDB 常用命令

```lldb
# 变量相关
p variable_name              # 打印变量值
po object                    # 打印对象描述
ptype variable_name          # 打印变量类型
frame variable               # 显示当前帧所有变量

# 断点相关
b main.cpp:42               # 在 main.cpp 第 42 行设置断点
b MyClass::myMethod         # 在方法上设置断点
breakpoint list             # 列出所有断点
breakpoint delete 1         # 删除断点 1
breakpoint disable 2        # 禁用断点 2

# 执行控制
c (continue)                # 继续执行
n (next)                    # 单步跳过
s (step)                    # 单步进入
finish                      # 执行到当前函数返回

# 线程相关
thread list                 # 列出所有线程
thread select 2             # 切换到线程 2
thread backtrace            # 显示当前线程调用栈

# 内存相关
x/10xb address             # 以十六进制查看 10 字节内存
memory read address        # 读取内存内容
```
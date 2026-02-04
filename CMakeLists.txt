cmake_minimum_required(VERSION 3.16)

project(QtEncryptedResources VERSION 1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

find_package(Qt6 REQUIRED COMPONENTS Core Gui Qml Quick Network)

# 主应用程序
add_executable(EncryptedQmlApp
    main.cpp
    ResourceEncryption.h
    ResourceEncryption.cpp
    EncryptedResourceSelector.h
    EncryptedResourceSelector.cpp
    EncryptedNetworkAccessManager.h
    EncryptedNetworkAccessManager.cpp
    ResourceEncryptor.h
    ResourceEncryptor.cpp
    qml.qrc
)

target_link_libraries(EncryptedQmlApp PRIVATE
    Qt6::Core
    Qt6::Gui
    Qt6::Qml
    Qt6::Quick
    Qt6::Network
)

# 加密工具
add_executable(resource_encryptor
    encryptor_tool.cpp
    ResourceEncryption.h
    ResourceEncryption.cpp
    ResourceEncryptor.h
    ResourceEncryptor.cpp
)

target_link_libraries(resource_encryptor PRIVATE
    Qt6::Core
)

set_target_properties(EncryptedQmlApp PROPERTIES
    WIN32_EXECUTABLE TRUE
    MACOSX_BUNDLE TRUE
)

# 设置输出目录Debug/Release
set_target_properties(EncryptedQmlApp PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY_DEBUG ${CMAKE_BINARY_DIR}/Debug
    RUNTIME_OUTPUT_DIRECTORY_RELEASE ${CMAKE_BINARY_DIR}/Release
)
set_target_properties(resource_encryptor PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY_DEBUG ${CMAKE_BINARY_DIR}/Debug
    RUNTIME_OUTPUT_DIRECTORY_RELEASE ${CMAKE_BINARY_DIR}/Release
)

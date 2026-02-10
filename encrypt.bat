cd build
cd Debug

./resource_encryptor.exe -m encrypt -i ../../main.qml -o ../../main.qml.enc -k "MySecretKey123!@#"
if %errorlevel% equ 0 (
    echo [成功] main.qml 加密成功
) else (
    echo [错误] main.qml 加密失败
)

./resource_encryptor.exe -m encrypt -i ../../MyComponent.qml -o ../../MyComponent.qml.enc -k "MySecretKey123!@#"
if %errorlevel% equ 0 (
    echo [成功] MyComponent.qml 加密成功
) else (
    echo [错误] MyComponent.qml 加密失败
)

./resource_encryptor.exe -m encrypt -i ../../qmldir -o ../../qmldir.enc -k "MySecretKey123!@#"
if %errorlevel% equ 0 (
    echo [成功] qmldir 加密成功
) else (
    echo [错误] qmldir 加密失败
)

./resource_encryptor.exe -m encrypt -i ..\..\test_image.png -o ../../test_image.png.enc -k "MySecretKey123!@#"
if %errorlevel% equ 0 (
    echo [成功] test_image.png 加密成功
) else (
    echo [错误] test_image.png 加密失败
)

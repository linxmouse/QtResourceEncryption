import QtQuick
import QtQuick.Window
import QtQuick.Controls

Window {
    width: 640
    height: 550
    visible: true
    title: qsTr("加密QML资源演示")

    Rectangle {
        anchors.fill: parent
        gradient: Gradient {
            GradientStop {
                position: 0.0
                color: "#2c3e50"
            }
            GradientStop {
                position: 1.0
                color: "#34495e"
            }
        }

        Column {
            anchors.centerIn: parent
            spacing: 20

            Text {
                text: "🔒 Qt资源加密方案"
                font.pixelSize: 32
                font.bold: true
                color: "white"
                anchors.horizontalCenter: parent.horizontalCenter
            }

            Text {
                text: "此QML文件已被成功解密并加载"
                font.pixelSize: 18
                color: "#ecf0f1"
                anchors.horizontalCenter: parent.horizontalCenter
            }

            Rectangle {
                width: 400
                height: 200
                color: "#ecf0f1"
                radius: 10
                anchors.horizontalCenter: parent.horizontalCenter

                Column {
                    anchors.centerIn: parent
                    spacing: 15

                    Text {
                        text: "✓ 支持QML加密"
                        font.pixelSize: 16
                        color: "#27ae60"
                    }

                    Image {
                        source: "encrypted:///CMake-Logo.png"
                        width: 290
                        height: 82
                        fillMode: Image.PreserveAspectFit
                        anchors.horizontalCenter: parent.horizontalCenter
                    }

                    Text {
                        text: "✓ 支持图片资源加密"
                        font.pixelSize: 16
                        color: "#27ae60"
                    }
                }
            }

            Button {
                text: "测试按钮"
                anchors.horizontalCenter: parent.horizontalCenter
                onClicked: {
                    console.log("加密资源中的按钮被点击!");
                    testText.text = "按钮点击成功! 时间: " + new Date().toLocaleTimeString();
                }
            }

            Text {
                id: testText
                text: "点击上方按钮测试功能"
                font.pixelSize: 14
                color: "#bdc3c7"
                anchors.horizontalCenter: parent.horizontalCenter
            }

            MyComponent {
                anchors.horizontalCenter: parent.horizontalCenter
            }
        }
    }
}

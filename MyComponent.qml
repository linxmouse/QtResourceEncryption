import QtQuick

Rectangle {
    id: root
    width: 300
    height: 100
    color: "#3498db"
    radius: 10

    property string displayText: "这是一个加密的组件"

    Text {
        anchors.centerIn: parent
        text: root.displayText
        font.pixelSize: 18
        font.bold: true
        color: "white"
    }

    MouseArea {
        anchors.fill: parent
        onClicked: {
            console.log("加密组件被点击");
        }
    }
}

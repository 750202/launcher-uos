import QtQuick 2.15

Item {
    id: root

    // 属性
    property alias icon: iconText.text
    property alias message: messageText.text
    property bool visible: true

    // 样式
    opacity: root.visible ? 1.0 : 0.0

    Column {
        anchors.centerIn: parent
        spacing: 12

        Text {
            id: iconText
            anchors.horizontalCenter: parent.horizontalCenter
            text: "🔍"
            font.pixelSize: 48
            opacity: 0.6
        }

        Text {
            id: messageText
            anchors.horizontalCenter: parent.horizontalCenter
            text: "输入关键词开始搜索"
            font.pixelSize: 14
            color: "#6B7280"
            opacity: 0.6
        }
    }

    Behavior on opacity {
        NumberAnimation { duration: 200 }
    }
}

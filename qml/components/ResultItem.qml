import QtQuick 2.15

Rectangle {
    id: root

    // 属性
    property alias icon: iconText.text
    property alias title: titleText.text
    property alias subtitle: subtitleText.text

    // 样式
    width: parent.width
    height: 60
    radius: 12
    color: mouseArea.containsMouse ? "rgba(59, 130, 246, 0.1)" : "rgba(255, 255, 255, 0.4)"

    Row {
        anchors.fill: parent
        anchors.leftMargin: 12
        anchors.rightMargin: 12
        spacing: 12

        Rectangle {
            width: 40
            height: 40
            radius: 8
            color: "rgba(59, 130, 246, 0.2)"

            Text {
                id: iconText
                anchors.centerIn: parent
                text: "📦"
                font.pixelSize: 20
            }
        }

        Column {
            anchors.verticalCenter: parent.verticalCenter
            spacing: 4

            Text {
                id: titleText
                text: "标题"
                font.pixelSize: 16
                font.weight: Font.Medium
                color: "#111827"
            }

            Text {
                id: subtitleText
                text: "副标题"
                font.pixelSize: 12
                color: "#6B7280"
            }
        }

        Item { width: 1; Layout.fillWidth: true }

        Text {
            text: "→"
            font.pixelSize: 16
            color: "#6B7280"
            anchors.verticalCenter: parent.verticalCenter
        }
    }

    MouseArea {
        id: mouseArea
        anchors.fill: parent
        hoverEnabled: true
    }

    Behavior on color {
        ColorAnimation { duration: 150 }
    }
}

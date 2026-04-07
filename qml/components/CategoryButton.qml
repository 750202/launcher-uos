import QtQuick 2.15

Rectangle {
    id: root

    // 属性
    property alias label: label.text
    property bool active: false

    // 样式
    width: 56
    height: 56
    radius: 28
    color: root.active ? "rgba(59, 130, 246, 0.2)" : "rgba(255, 255, 255, 0.3)"
    border.width: 2
    border.color: root.active ? "rgba(59, 130, 246, 0.5)" : "rgba(200, 200, 200, 0.3)"

    // 标签
    Text {
        id: label
        anchors.centerIn: parent
        font.pixelSize: 14
        font.weight: Font.Medium
        color: "#111827"
    }

    // 鼠标交互
    MouseArea {
        anchors.fill: parent
        hoverEnabled: true

        onHoveredChanged: {
            if (containsMouse && !root.active) {
                root.opacity = 0.8
            } else {
                root.opacity = 1.0
            }
        }
    }

    // 行为
    Behavior on opacity {
        NumberAnimation { duration: 150 }
    }
}

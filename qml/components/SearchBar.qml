import QtQuick 2.15
import QtQuick.Controls 2.15

Rectangle {
    id: root

    // 公共属性
    property alias placeholderText: textField.placeholderText
    property alias text: textField.text

    // 样式
    color: "rgba(255, 255, 255, 0.5)"
    radius: 18
    border.width: 2
    border.color: "rgba(59, 130, 246, 0.2)"

    Row {
        anchors.fill: parent
        anchors.leftMargin: 20
        anchors.rightMargin: 20
        spacing: 12

        Text {
            id: searchIcon
            text: "🔍"
            font.pixelSize: 24
            anchors.verticalCenter: parent.verticalCenter
            opacity: 0.6
        }

        TextField {
            id: textField
            width: parent.width - 72
            height: parent.height
            placeholderText: "聚焦搜索"
            background: Rectangle { color: "transparent" }
            color: "#111827"
            font.pixelSize: 18
            font.weight: Font.SemiBold

            anchors.verticalCenter: parent.verticalCenter
        }
    }
}

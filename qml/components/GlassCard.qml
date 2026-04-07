import QtQuick 2.15

// GlassCard 占位符组件
// 阶段 4 将实现完整的毛玻璃效果

Rectangle {
    id: root

    // 公共属性
    property alias contentItem: contentLoader.sourceComponent
    property real borderRadius: 20
    property real borderWidth: 2

    // 基础样式
    radius: root.borderRadius
    color: "rgba(207, 207, 207, 0.9)"
    border.width: root.borderWidth
    border.color: "rgba(59, 130, 246, 0.15)"

    // 内容加载器
    Loader {
        id: contentLoader
        anchors.fill: parent
        anchors.margins: root.borderWidth
        active: root.contentItem != null
        asynchronous: true
        sourceComponent: root.contentItem
    }
}

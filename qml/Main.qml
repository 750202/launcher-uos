import QtQuick 2.15
import QtQuick.Controls 2.15
import Launcher 1.0

Item {
    id: root
    width: 700
    height: 600

    property string currentMode: ""
    property bool showResults: currentMode !== ""
    property color itemBgColor: Qt.rgba(255/255, 255/255, 255/255, 0.3)
    property color itemHoverColor: Qt.rgba(59/255, 130/255, 246/255, 0.25)
    property color borderColor: Qt.rgba(200/255, 200/255, 200/255, 0.3)
    property color borderHoverColor: Qt.rgba(59/255, 130/255, 246/255, 0.6)
    property int btnSize: 56
    property int btnSpacing: 12

    // 背景缩放入场动画
    property real bgScale: showResults ? 1.0 : 0.95
    transform: Scale {
        origin.x: root.width / 2
        origin.y: root.height / 2
        xScale: root.bgScale
        yScale: root.bgScale
    }
    Behavior on bgScale { NumberAnimation { duration: 200; easing.type: Easing.OutBack } }

    Rectangle {
        id: background
        anchors.fill: parent
        color: showResults ? Qt.rgba(239/255, 239/255, 239/255, 0.95) : "transparent"
        radius: 20
        border.color: showResults ? Qt.rgba(59/255, 130/255, 246/255, 0.3) : "transparent"
        border.width: showResults ? 2 : 0
        opacity: showResults ? 1 : 0
        Behavior on color { ColorAnimation { duration: 200 } }
        Behavior on opacity { NumberAnimation { duration: 200 } }
    }

    Column {
        anchors.fill: parent
        anchors.margins: 20
        spacing: 16

        Row {
            id: searchRow
            width: parent.width
            height: 60
            spacing: btnSpacing
            clip: false

            Rectangle {
                id: searchBox
                width: currentMode === "" ? parent.width - (btnSize * 2 + btnSpacing * 2) : parent.width
                height: 60
                anchors.verticalCenter: parent.verticalCenter
                color: itemBgColor
                radius: 30
                border.width: searchInput.activeFocus ? 2 : 1
                border.color: searchInput.activeFocus ? Qt.rgba(59/255, 130/255, 246/255, 0.5) : borderColor
                Behavior on width { NumberAnimation { duration: 200; easing.type: Easing.OutCubic } }

                Row {
                    anchors.fill: parent
                    anchors.leftMargin: 20
                    anchors.rightMargin: 20
                    spacing: 12

                    Text { text: "🔍"; font.pixelSize: 24; opacity: 0.6; anchors.verticalCenter: parent.verticalCenter }

                    TextField {
                        id: searchInput
                        width: parent.width - 72 - (text.length > 0 ? 40 : 0)
                        height: parent.height
                        placeholderText: currentMode === "apps" ? "搜索应用..." : currentMode === "files" ? "搜索文件..." : "聚焦搜索..."
                        background: Rectangle { color: "transparent" }
                        color: "#111827"
                        font.pixelSize: 18
                        font.bold: true
                        anchors.verticalCenter: parent.verticalCenter
                        selectByMouse: true
                        onTextChanged: {
                            if (currentMode === "" && text.length > 0) currentMode = "global"
                            else if (text.length === 0 && currentMode === "global") currentMode = ""
                            // 更新 ViewModel 查询（带防抖）
                            searchViewModel.query = text
                        }
                    }

                    Text {
                        visible: searchInput.text.length > 0
                        text: "✕"
                        font.pixelSize: 18
                        color: clearArea.containsMouse ? "#3B82F6" : "#9CA3AF"
                        anchors.verticalCenter: parent.verticalCenter
                        MouseArea {
                            id: clearArea
                            anchors.fill: parent
                            hoverEnabled: true
                            cursorShape: Qt.PointingHandCursor
                            onClicked: {
                                searchInput.text = ""
                                currentMode = ""
                                searchViewModel.clear()
                                searchInput.forceActiveFocus()
                            }
                        }
                    }
                }
            }

            // 应用按钮 - 水滴融合效果
            Rectangle {
                id: appBtn
                width: btnSize
                height: btnSize
                radius: btnSize / 2
                anchors.verticalCenter: parent.verticalCenter
                visible: opacity > 0.01
                opacity: currentMode === "" ? 1 : 0
                x: currentMode === "" ? 0 : -30
                color: appArea.containsMouse ? itemHoverColor : itemBgColor
                border.width: 2
                border.color: appArea.containsMouse ? borderHoverColor : borderColor
                Behavior on opacity { NumberAnimation { duration: 200; easing.type: Easing.OutCubic } }
                Behavior on x { NumberAnimation { duration: 200; easing.type: Easing.OutCubic } }
                Image { anchors.centerIn: parent; source: "qrc:/resources/icons/app.png"; sourceSize.width: 28; sourceSize.height: 28 }
                MouseArea {
                    id: appArea
                    anchors.fill: parent
                    hoverEnabled: true
                    cursorShape: Qt.PointingHandCursor
                    onClicked: {
                        currentMode = "apps"
                        searchViewModel.category = "apps"
                        searchViewModel.search()
                        searchInput.forceActiveFocus()
                    }
                }
                Behavior on color { ColorAnimation { duration: 150 } }
            }

            // 文件按钮 - 水滴融合效果
            Rectangle {
                id: fileBtn
                width: btnSize
                height: btnSize
                radius: btnSize / 2
                anchors.verticalCenter: parent.verticalCenter
                visible: opacity > 0.01
                opacity: currentMode === "" ? 1 : 0
                x: currentMode === "" ? 0 : -60
                color: fileArea.containsMouse ? itemHoverColor : itemBgColor
                border.width: 2
                border.color: fileArea.containsMouse ? borderHoverColor : borderColor
                Behavior on opacity { NumberAnimation { duration: 200; easing.type: Easing.OutCubic } }
                Behavior on x { NumberAnimation { duration: 200; easing.type: Easing.OutCubic } }
                Image { anchors.centerIn: parent; source: "qrc:/resources/icons/file.png"; sourceSize.width: 28; sourceSize.height: 28 }
                MouseArea {
                    id: fileArea
                    anchors.fill: parent
                    hoverEnabled: true
                    cursorShape: Qt.PointingHandCursor
                    onClicked: {
                        currentMode = "files"
                        searchViewModel.category = "files"
                        searchViewModel.search()
                        searchInput.forceActiveFocus()
                    }
                }
                Behavior on color { ColorAnimation { duration: 150 } }
            }
        }

        Rectangle {
            id: resultsContainer
            width: parent.width
            height: showResults ? parent.height - 60 - 32 : 0
            visible: height > 0
            color: Qt.rgba(255/255, 255/255, 255/255, 0.5)
            radius: 18
            border.width: 2
            border.color: Qt.rgba(59/255, 130/255, 246/255, 0.15)
            clip: true
            Behavior on height { NumberAnimation { duration: 200; easing.type: Easing.OutCubic } }

            ScrollView {
                anchors.fill: parent
                anchors.margins: 8
                clip: true
                ScrollBar.horizontal.policy: ScrollBar.AlwaysOff
                ScrollBar.vertical.policy: ScrollBar.AsNeeded

                ListView {
                    id: resultList
                    model: searchViewModel.model
                    spacing: 8
                    delegate: Rectangle {
                        id: resultItem
                        width: resultList.width - 16
                        height: 60
                        radius: 12
                        color: delArea.containsMouse ? Qt.rgba(59/255, 130/255, 246/255, 0.1) : Qt.rgba(255/255, 255/255, 255/255, 0.6)
                        opacity: 0
                        Component.onCompleted: fadeInTimer.start()
                        Timer {
                            id: fadeInTimer
                            interval: index * 30
                            onTriggered: fadeInAnim.start()
                        }
                        NumberAnimation {
                            id: fadeInAnim
                            target: resultItem
                            property: "opacity"
                            from: 0; to: 1
                            duration: 150
                            easing.type: Easing.OutCubic
                        }
                        Row {
                            anchors.fill: parent
                            anchors.leftMargin: 12
                            anchors.rightMargin: 12
                            spacing: 12
                            Rectangle {
                                width: 40; height: 40; radius: 8
                                color: model.itemCategory === "app" ? Qt.rgba(59/255, 130/255, 246/255, 0.2) : Qt.rgba(34/255, 197/255, 94/255, 0.2)
                                anchors.verticalCenter: parent.verticalCenter
                                Text {
                                    anchors.centerIn: parent
                                    text: model.itemCategory === "app" ? "📱" : "📄"
                                    font.pixelSize: 20
                                }
                            }
                            Column {
                                width: parent.width - 120
                                anchors.verticalCenter: parent.verticalCenter
                                spacing: 4
                                Text { text: model.name; font.pixelSize: 16; color: "#111827"; elide: Text.ElideRight; width: parent.width }
                                Text { text: model.subtitle; font.pixelSize: 12; color: "#6B7280"; elide: Text.ElideRight; width: parent.width }
                            }
                            Rectangle {
                                visible: currentMode === "global"
                                width: typeLbl.implicitWidth + 12; height: 20; radius: 4
                                color: model.itemCategory === "app" ? Qt.rgba(59/255, 130/255, 246/255, 0.15) : Qt.rgba(34/255, 197/255, 94/255, 0.15)
                                anchors.verticalCenter: parent.verticalCenter
                                Text {
                                    id: typeLbl
                                    anchors.centerIn: parent
                                    text: model.typeLabel
                                    font.pixelSize: 10
                                    color: model.itemCategory === "app" ? "#3B82F6" : "#22C55E"
                                }
                            }
                            Text { text: "→"; font.pixelSize: 16; color: "#6B7280"; anchors.verticalCenter: parent.verticalCenter }
                        }
                        MouseArea {
                            id: delArea
                            anchors.fill: parent
                            hoverEnabled: true
                            cursorShape: Qt.PointingHandCursor
                            onClicked: {
                                // 点击结果项：启动应用或打开文件
                                if (model.itemCategory === "app") {
                                    searchViewModel.launchApp(model.itemId)
                                } else {
                                    searchViewModel.openFile(model.itemId)
                                }
                            }
                        }
                        Behavior on color { ColorAnimation { duration: 100 } }
                    }
                }
            }

            Column {
                anchors.centerIn: parent
                spacing: 12
                visible: searchViewModel.model.count === 0 && showResults && !searchViewModel.searching
                Text { text: "未找到匹配结果"; font.pixelSize: 16; color: "#6B7280"; anchors.horizontalCenter: parent.horizontalCenter }
                Text { text: "ESC 关闭窗口"; font.pixelSize: 12; color: "#9CA3AF"; anchors.horizontalCenter: parent.horizontalCenter }
            }

            // 搜索中指示器
            BusyIndicator {
                anchors.centerIn: parent
                running: searchViewModel.searching
                visible: searchViewModel.searching
            }
        }
    }

    // 连接 ViewModel 信号
    Connections {
        target: searchViewModel
        function onCategoryChanged() {
            // 同步 currentMode
            if (searchViewModel.category === "apps") currentMode = "apps"
            else if (searchViewModel.category === "files") currentMode = "files"
            else if (searchViewModel.category === "all") currentMode = "global"
        }
    }

    focus: true
    Keys.onEscapePressed: {
        if (searchInput.text.length > 0) {
            searchInput.text = ""
            if (currentMode === "global") currentMode = ""
            searchViewModel.clear()
        } else if (currentMode !== "") {
            currentMode = ""
            searchViewModel.category = "all"
            searchViewModel.clear()
        } else {
            Qt.quit()
        }
    }

    Component.onCompleted: Qt.callLater(function() { searchInput.forceActiveFocus() })
}

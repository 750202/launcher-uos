# Qt 5 毛玻璃效果实现指南

用于 Launcher 项目中的毛玻璃背景效果

---

## 📋 概述

目标效果：类似 CSS `backdrop-filter: blur()` 的毛玻璃模糊效果

**挑战**：Qt 5 没有直接的毛玻璃 API，需要使用 ShaderEffect + GLSL

---

## 🔧 技术方案

### 方案 1: ShaderEffect (推荐)

**复杂度**: ⭐⭐⭐⭐
**性能**: ⭐⭐⭐⭐
**效果**: ⭐⭐⭐⭐⭐

#### 基础实现

```qml
import QtQuick 2.15
import QtGraphicalEffects 1.15

Rectangle {
    id: root
    width: 400
    height: 60
    color: "rgba(207, 207, 207, 0.9)"  // 基础背景色

    // 毛玻璃效果
    layer.enabled: true
    layer.effect: ShaderEffect {
        fragmentShader: "
            #version 440
            layout(location = 0) in vec2 qt_TexCoord0;
            layout(location = 0) out vec4 fragColor;
            layout(std140, binding = 0) uniform buf {
                mat4 qt_Matrix;
                float qt_Opacity;
                float blurRadius;
                float delta;
                vec2 resolution;
            };
            uniform sampler2D source;

            void main() {
                vec2 uv = qt_TexCoord0;
                vec4 color = vec4(0.0);
                float total = 0.0;
                float radius = blurRadius;

                // 高斯模糊采样
                for (float x = -radius; x <= radius; x += 1.0) {
                    for (float y = -radius; y <= radius; y += 1.0) {
                        vec2 offset = vec2(x, y) * delta / resolution;
                        color += texture(source, uv + offset);
                        total += 1.0;
                    }
                }

                fragColor = color / total * qt_Opacity;
            }
        "

        property real blurRadius: 20
        property real delta: 2.0
        property vec2 resolution: Qt.vector2d(width, height)
    }
}
```

#### 优化实现 (降采样)

```qml
layer.enabled: true
layer.effect: ShaderEffectSource {
    sourceItem: parent.parent
    anchors.fill: parent

    // 第一次模糊 (降采样)
    blur: 0.6
    blurMax: 64
    transparentBorder: true
}
```

### 方案 2: FastBlur (QtGraphicalEffects)

**复杂度**: ⭐⭐
**性能**: ⭐⭐⭐⭐
**效果**: ⭐⭐⭐

```qml
import QtGraphicalEffects 1.15

Rectangle {
    color: "rgba(207, 207, 207, 0.9)"

    layer.enabled: true
    layer.effect: FastBlur {
        radius: 20
        transparentBorder: true
    }
}
```

### 方案 3: 渐变透明度 (最简单)

**复杂度**: ⭐
**性能**: ⭐⭐⭐⭐⭐
**效果**: ⭐⭐

```qml
Rectangle {
    color: "rgba(207, 207, 207, 0.9)"

    // 模拟毛玻璃的半透明 + 渐变
    gradient: Gradient {
        GradientStop { position: 0.0; color: Qt.rgba(0,0,0,0.1) }
        GradientStop { position: 0.5; color: Qt.rgba(0,0,0,0.075) }
        GradientStop { position: 1.0; color: Qt.rgba(0,0,0,0.05) }
    }
}
```

---

## 🔨 完整组件实现

### GlassCard 组件

```qml
// GlassCard.qml
import QtQuick 2.15

Rectangle {
    id: root

    // 公共属性
    property alias contentItem: contentLoader.sourceComponent
    property real borderRadius: 20
    property real borderWidth: 2
    property bool blurEnabled: true
    property real blurRadius: 20

    // 内部内容容器
    Item {
        id: contentContainer
        anchors.fill: parent
        anchors.margins: root.borderWidth
    }

    // 毛玻璃背景
    Rectangle {
        id: blurRect
        anchors.fill: parent
        anchors.margins: root.borderWidth
        radius: root.borderRadius - root.borderWidth
        color: "rgba(207, 207, 207, 0.9)"

        layer.enabled: root.blurEnabled
        layer.effect: ShaderEffect {
            id: blurEffect
            property real radius: root.blurRadius

            fragmentShader: "
                #version 440
                layout(location = 0) in vec2 qt_TexCoord0;
                layout(location = 0) out vec4 fragColor;
                layout(std140, binding = 0) uniform buf {
                    mat4 qt_Matrix;
                    float qt_Opacity;
                    float radius;
                    float delta;
                    vec2 resolution;
                };
                uniform sampler2D source;

                void main() {
                    vec2 uv = qt_TexCoord0;
                    vec4 color = vec4(0.0);
                    float total = 0.0;

                    for (float x = -radius; x <= radius; x += 1.0) {
                        for (float y = -radius; y <= radius; y += 1.0) {
                            vec2 offset = vec2(x, y) * delta / resolution;
                            color += texture(source, uv + offset);
                            total += 1.0;
                        }
                    }

                    fragColor = color / total * qt_Opacity;
                }
            "

            property real delta: 2.2
            property vec2 resolution: Qt.vector2d(width, height)
        }
    }

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
```

### 双层容器渐变边框 + 毛玻璃

```qml
// GlassSearchBar.qml
import QtQuick 2.15
import QtQuick.Controls 2.15

Item {
    id: root
    width: 600
    height: 60

    // 外层容器（边框）
    Item {
        anchors.fill: parent
        anchors.margins: -2

        Rectangle {
            anchors.fill: parent
            radius: 22
            color: "transparent"

            gradient: Gradient {
                orientation: Gradient.Horizontal
                GradientStop { position: 0.0; color: "#3b82f64d" }
                GradientStop { position: 0.5; color: "#a855f74d" }
                GradientStop { position: 1.0; color: "#60a5fa4d" }
            }
        }
    }

    // 内层容器（内容 + 毛玻璃）
    Rectangle {
        anchors.fill: parent
        radius: 20
        color: "rgba(207, 207, 207, 0.9)"

        // 毛玻璃效果
        layer.enabled: true
        layer.effect: ShaderEffect {
            fragmentShader: "qrc:/shaders/blur.frag.qsb"
            property real blurRadius: 20
        }

        // 内容
        Row {
            anchors.fill: parent
            spacing: 12
            anchors.leftMargin: 20
            anchors.topMargin: 12
            anchors.bottomMargin: 12

            Text {
                text: "🔍"
                font.pixelSize: 24
                opacity: 0.6
                anchors.verticalCenter: parent.verticalCenter
            }

            TextField {
                width: parent.width - 72
                placeholderText: "聚焦搜索"
                background: Rectangle { color: "transparent" }
                color: "#111827"
                font.pixelSize: 18
                font.weight: Font.SemiBold
                anchors.verticalCenter: parent.verticalCenter
            }
        }
    }
}
```

---

## 📝 着色器资源文件

创建 `resources/shaders/blur.frag.qsb`:

```qml
// blur.frag.qsb -> 编译为 blur.frag.qsb

#version 440
layout(location = 0) in vec2 qt_TexCoord0;
layout(location = 0) out vec4 fragColor;
layout(std140, binding = 0) uniform buf {
    mat4 qt_Matrix;
    float qt_Opacity;
    float blurRadius;
    float delta;
    vec2 resolution;
};
uniform sampler2D source;

void main() {
    vec2 uv = qt_TexCoord0;
    vec4 color = vec4(0.0);
    float total = 0.0;

    for (float x = -blurRadius; x <= blurRadius; x += 1.0) {
        for (float y = -blurRadius; y <= blurRadius; y += 1.0) {
            vec2 offset = vec2(x, y) * delta / resolution;
            color += texture(source, uv + offset);
            total += 1.0;
        }
    }

    fragColor = color / total * qt_Opacity;
}
```

---

## 🔄 性能优化技巧

### 1. 使用降采样

```qml
layer.effect: ShaderEffectSource {
    sourceItem: parent.parent
    anchors.fill: parent
    width: parent.width / 2
    height: parent.height / 2
}
```

### 2. 降低模糊半径

```qml
property real blurRadius: 16  // 不是 20
```

### 3. 使用 FastBlur 替代 ShaderEffect

```qml
layer.effect: FastBlur {
    radius: 16
    transparentBorder: true
}
```

### 4. 按需启用

```qml
property bool blurEnabled: true

layer.enabled: root.blurEnabled  // 只在需要时启用
```

---

## ⚙️ CMakeLists.txt 配置

```cmake
# 添加着色器资源
qt_add_resources(launcher
    PREFIX "/shaders"
    FILES
        resources/shaders/blur.frag.qsb
)

# 需要的 Qt 模块
find_package(Qt5 COMPONENTS Charts REQUIRED)
# QtGraphicalEffects 在 Qt 5.15 中不需要额外链接
```

---

## 🐛 调试技巧

### 1. 检查 ShaderEffect 是否生效

```qml
layer.enabled: true
layer.effect: ShaderEffect {
    // 添加调试输出
    Component.onCompleted: {
        console.log("ShaderEffect loaded")
    }
}
```

### 2. 检查性能

```qml
// 添加性能监控
NumberAnimation on blurRadius {
    from: 10
    to: 30
    duration: 2000
    onRunningChanged: {
        if (!running && duration > 500) {
            console.warn("Blur animation slow!")
        }
    }
}
```

### 3. 回退方案

```qml
property bool useShaderEffect: true

layer.enabled: root.useShaderEffect
layer.effect: root.useShaderEffect ? shaderEffect : null

// 兜底方案
Rectangle {
    visible: !root.useShaderEffect
    color: "rgba(207, 207, 207, 0.9)"
}
```

---

## 📚 参考资料

- [Qt 5 ShaderEffect 官方文档](https://doc.qt.io/qt-5/qml-shadereffect.html)
- [QtGraphicalEffects 官方文档](https://doc.qt.io/qt-5/qtgraphicaleffects-index.html)
- [GLSL 着色器语言](https://www.khronos.org/opengl/wiki/OpenGL_Shading_Language)
- [QML Material Blur 教程](https://forum.qt.io/topic/81886/blur-behind-transparent-item-correct-method)

---

## 🎯 Launcher 中的使用

### 搜索框

```qml
GlassSearchBar {
    width: searchContainer.width
    height: 60
    blurEnabled: true
    blurRadius: 16  // 中等模糊
}
```

### 结果卡片

```qml
GlassCard {
    width: resultsContainer.width
    height: Math.min(resultsContainer.height, 500)
    blurEnabled: true
    blurRadius: 12  // 较弱模糊

    contentItem: Component {
        // 结果列表
        ListView { ... }
    }
}
```

### 圆形按钮

```qml
CategoryButton {
    active: true
    blurEnabled: false  // 按钮不需要毛玻璃
}
```

---

**版本**: 1.0
**日期**: 2026-04-02
**状态**: ✅ 已完成

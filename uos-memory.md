# UOS Qt 开发修复经验

本文档记录在 UOS V20 / Deepin 25 平台上开发 Qt (Qt Quick / QML) + CMake + Debian 打包项目的常见问题及解决方案。

---

## 📌 项目背景

**项目**: Launcher - 聚焦搜索启动器
**技术栈**: Qt 5.12+, Qt Quick (QML), CMake, SQLite
**目标平台**: UOS V20 专业版 / Deepin 25 (amd64)

---

## 🔧 常见问题 & 解决方案

### 1. QML ListElement ID 规则

**问题**: `IDs must start with a letter or underscore`

```qml
// ❌ 错误：ID 不能以数字开头
ListElement { id: "1"; name: "Obsidian"; ... }
ListElement { id: "2"; name: "VS Code"; ... }

// ✅ 正确：ID 必须以字母或下划线开头
ListElement { id: "app1"; name: "Obsidian"; ... }
ListElement { id: "app2"; name: "VS Code"; ... }
ListElement { id: "_item1"; name: "VS Code"; ... }
```

**原因**: QML 的 ID 命名规则要求必须以字母[a-zA-Z]或下划线开头，不能以数字开头。

---

### 2. Resources.qrc 引用不存在的文件

**问题**: `make: *** no rule to make target 'xxx.qsb'`

```xml
<!-- ❌ 错误：引用了不存在的文件 -->
<file>qml/resources/shaders/blur.frag.qsb</file>

<!-- ✅ 正确：只引用存在的文件 -->
<!-- 省略不存在的文件引用，或先创建文件 -->
```

**解决方案**:
- 从 resources.qrc 中移除不存在的文件引用
- 或先创建需要的文件再添加到资源系统

---

### 3. 静态实例定义错误

**问题**: `'ClassName* ClassName::s_instance' is not a static data member`

**错误代码**:
```cpp
// ❌ 头文件中缺少静态成员声明
class Settings {
public:
    static Settings* instance();
    // ... 缺少 static Settings* s_instance;
};

// ✅ 头文件必须有静态声明
class Settings {
public:
    static Settings* instance();
    static Settings* s_instance;  // 必须在类的 private 部分声明
};
```

**完整模式**:
```cpp
// Settings.h
class Settings {
public:
    static Settings* instance();
    void setValue(const QString& key, const QVariant& value);
    // ...
    
private:
    static Settings* s_instance;  // ← 必须声明
    Settings();
    // ...
};

// Settings.cpp
Settings* Settings::s_instance = nullptr;  // ← 必须定义
Settings* Settings::instance() {
    if (!s_instance) {
        s_instance = new Settings();
    }
    return s_instance;
}
```

---

### 4. QSqlError 前向声明问题

**问题**: `invalid use of incomplete type 'class QSqlError'`

**错误代码**:
```cpp
// ❌ 缺少完整类型定义
#include <QSqlQuery>
// query.lastError() 需要完整的 QSqlError 类型
qWarning() << query.lastError().text();
```

**解决方案**:
```cpp
// ✅ 添加头文件
#include <QSqlQuery>
#include <QSqlError>  // ← 添加
qWarning() << query.lastError().text();
```

---

### 5. qApp 和 activateWindow Qt 版本兼容

**问题**: `'qApp' was not declared in this scope` 或 `'class QQuickView' has no member named 'activateWindow'`

**Qt 5 不支持的方式**:
```cpp
// ❌ Qt 5 没有 qApp 宏
qApp->installNativeEventFilter(this);

// ❌ QQuickView 没有 activateWindow
view.activateWindow();
```

**Qt 5/6 兼容方式**:
```cpp
// ✅ 使用 QGuiApplication::instance()
#include <QGuiApplication>
if (QGuiApplication::instance()) {
    QGuiApplication::instance()->installNativeEventFilter(this);
}

// ✅ 使用 requestActivate() 替代 activateWindow
#include <QWindow>
view.requestActivate();  // Qt 5.15+ 可用
```

---

### 6. 数据库目录不存在

**问题**: `unable to open database file Error opening database`

**原因**: SQLite 要求父目录必须存在才能创建数据库文件

**解决方案**:
```cpp
// Database::init()
bool Database::init()
{
    // ✅ 创建数据库目录
    QString dbDir = QFileInfo(m_dbPath).absolutePath();
    QDir().mkpath(dbDir);

    m_db.setDatabaseName(m_dbPath);
    if (!m_db.open()) {
        qCritical() << "Failed to open database:" << m_db.lastError().text();
        return false;
    }
    // ...
}
```

---

### 7. QML Layout 属性错误

**问题**: `Non-existent attached object Layout.fillWidth`

**原因**: 使用了 Item 但没有导入 QtQuick.Layouts

**解决方案**:
```qml
// ❌ 缺少导入
import QtQuick 2.15
import QtQuick.Controls 2.15

Item {
    Layout.fillWidth: true  // 错误
}

// ✅ 导入 Layouts 模块
import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15  // ← 添加

Item {
    Layout.fillWidth: true  // 正确
}
```

或使用 Item 的 anchors：
```qml
Item {
   anchors.fill: parent
    anchors.leftMargin: 10
    anchors.rightMargin: 10
}
```

---

### 8. property alias 信号绑定错误

**问题**: `Invalid alias target location: textChanged`

**错误代码**:
```qml
TextField {
    id: searchInput
}

// ❌ 不能 alias 到信号
property alias searchTextChanged: searchInput.textChanged
```

**解决方案**:
```qml
// ✅ 在 onTextChanged 中调用函数
TextField {
    id: searchInput
    onTextChanged: filterResults()
}

function filterResults() {
    // 过滤逻辑
}
```

---

### 9. 单例模式线程安全

**推荐模式**:

```cpp
// Singleton.h
class Singleton {
public:
    static Singleton* instance();
    
    // 禁止拷贝和赋值
    Singleton(const Singleton&) = delete;
    Singleton& operator=(const Singleton&) = delete;
    
private:
    Singleton() = default;
    ~Singleton() = default;
    
    static Singleton* s_instance;
    static QMutex s_mutex;
};

// Singleton.cpp
Singleton* Singleton::s_instance = nullptr;
QMutex Singleton::s_mutex;

Singleton* Singleton::instance() {
    QMutexLocker locker(&s_mutex);
    if (!s_instance) {
        s_instance = new Singleton();
    }
    return s_instance;
}
```

---

### 10. QQuickView 窗口配置顺序

**问题**: macOS 上窗口不显示

**原因**: 某些窗口属性必须在设置 source 之前配置

**正确顺序**:
```cpp
QQuickView view;

// 1. 先设置窗口标志
view.setFlags(
    Qt::Window |
    Qt::FramelessWindowHint |
    Qt::WindowStaysOnTopHint
);

// 2. 设置透明度
view.setColor(QColor(Qt::transparent));

// 3. 然后设置 QML 源
view.setSource(QUrl(QStringLiteral("qrc:/qml/Main.qml")));

// 4. 最后设置位置和显示
QScreen* screen = app.primaryScreen();
if (screen) {
    QRect geometry = screen->availableGeometry();
    int x = (geometry.width() - 700) / 2;
    int y = geometry.top() + int(geometry.height() * 0.15);
    view.setGeometry(x, y, 700, 600);
}

view.show();
```

---

## 🛠️ 开发工具链配置

### Qt 5 安装 (UOS / Deepin)

```bash
# 安装 Qt 5 基础包
sudo apt install qt5-qmake qtbase5-dev qtdeclarative5-dev

# 安装 QML 模块
sudo apt install qml-module-qtquick-controls2
sudo apt install qml-module-qtquick-window2
sudo apt install qml-module-qtquick-layouts

# 验证安装
qmake --version
```

### CMake 配置

```cmake
# 检测 Qt 5/6
find_package(Qt5 COMPONENTS Core Quick Qml Sql QUIET)

if (Qt5_FOUND)
    message(STATUS "Using Qt 5")
    set(QT_VERSION_MAJOR 5)
else()
    find_package(Qt6 COMPONENTS Core Quick Qml Sql QUIET)
    if (Qt6_FOUND)
        message(STATUS "Using Qt 6")
        set(QT_VERSION_MAJOR 6)
    else()
        message(FATAL_ERROR "Qt not found")
    endif()
endif()

# 链接
if (QT_VERSION_MAJOR EQUAL 5)
    target_link_libraries(${PROJECT_NAME} Qt5::Core Qt5::Quick Qt5::Qml Qt5::Sql)
else()
    target_link_libraries(${PROJECT_NAME} Qt6::Core Qt6::Quick Qt6::Qml Qt6::Sql)
endif()
```

---

## 📦 Debian 打包注意事项

### debian/control

```
Package: launcher
Architecture: amd64
Depends: libqt5core5a (${shlibs:Depends}),
         libqt5quick5 (${shlibs:Depends}),
         libqt5qml5 (${shlibs:Depends}),
         libqt5sql5 (${shlibs:Depends}),
         qml-module-qtquick-controls2,
         qml-module-qtquick-window2,
         qml-module-qtquick-layouts
```

### postinst

```bash
#!/bin/bash
set -e

# 更新桌面数据库
if [ -x /usr/bin/update-desktop-database ]; then
    /usr/bin/update-desktop-database /usr/share/applications > /dev/null
fi

# 更新图标缓存
if [ -x /usr/bin/gtk-update-icon-cache ]; then
    /usr/bin/gtk-update-icon-cache -q /usr/share/icons/hicolor
fi
```

---

## 🔍 调试技巧

### 启用 Qt 调试输出

```bash
# 查看插件加载
export QT_DEBUG_PLUGINS=1
./launcher

# 查看 QML 调试
export QML_IMPORT_TRACE=1
./launcher

# 查看详细日志
QT_LOGGING_RULES="*.debug=true;qt.qml.*.debug=true" ./launcher
```

### 检查 QML 错误

```cpp
// main.cpp - 在 QML 加载后检查状态
view.setSource(QUrl(QStringLiteral("qrc:/qml/Main.qml")));
if (view.status() == QQuickView::Error) {
    // Qt 5 需要特定头文件才能访问 errors()
    qCritical() << "Failed to load QML";
}
```

### 单元测试数据路径

```cpp
// 单例模式可能导致测试问题
// 解决方案：
// 1. 添加 reset() 方法重置单例
// 2. 使用依赖注入
// 3. 测试前删除数据库文件
```

---

## 📝 编码规范

### C++ 编码

- **单例**: 使用函数式风格而非类式风格 `ClassName::instance()`
- **头文件包含**: 按字母顺序或功能分组
- **QString**: 使用 QLatin1String 处理 ASCII 字符串，避免隐式转换
- **资源释放**: 使用 RAII，析构函数清理资源

### QML 编码

- **ID 命名**: 必须以字母或下划线开头，使用小驼峰 `itemId` 或全小写
- **组件大小**: 组件超过 200 行考虑拆分为子组件
- **绑定**: 避免深层嵌套绑定，使用属性别名简化

---

## 🔄 常用命令

```bash
# 构建
mkdir build && cd build
cmake ..
make -j$(nproc)

# 清理
make clean
rm -rf build CMakeCache.txt

# 安装依赖检查
ldd ./launcher

# 检查 Qt 模块
ldd ./launcher | grep Qt

# 运行
./launcher

# 调试
gdb ./launcher
```

---

## 📚 参考资料

- [Qt 5.15 官方文档](https://doc.qt.io/qt-5.15/)
- [QML 和 C++ 集成](https://doc.qt.io/qt-5/qtqml-cppintegration-exposecppattributes.html)
- [CMake Qt 集成](https://doc.qt.io/qt-5/cmake-manual.html)
- [Debian 打包指南](https://www.debian.org/doc/manuals/debmake-doc/)

---

**最后更新**: 2026-04-02
**版本**: 1.0
**适用项目**: Launcher - 聚焦搜索启动器

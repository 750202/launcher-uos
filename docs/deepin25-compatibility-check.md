# Deepin 25 兼容性检查报告

**检查日期**: 2026-04-09
**目标平台**: Deepin 25 (Qt6)

---

## ✅ 已确认兼容

### 1. Qt 版本选择
- **CMakeLists.txt**: 优先 Qt6，回退 Qt5 ✅
- **GitHub Actions**: Ubuntu 24.04 + Qt6 ✅

### 2. QML 导入版本
- 所有 QML 文件使用 `QtQuick 2.15` ✅
- 兼容 Qt5 和 Qt6 ✅

### 3. 窗口配置
- **Main.qml**: 使用 `ApplicationWindow` ✅
- 窗口属性：无边框、置顶、透明背景 ✅

### 4. 维护脚本
- **postinst/prerm**: 使用 `|| true` 忽略非关键错误 ✅

---

## ⚠️ 已修复问题

### 1. QString::split() 参数兼容性
**问题**: Qt5 使用 `QString::SkipEmptyParts`，Qt6 使用 `Qt::SkipEmptyParts`

**文件**: `src/data/ApplicationRepository.cpp:193`

**修复**: 添加版本判断
```cpp
#if QT_VERSION >= QT_VERSION_CHECK(5, 14, 0)
    app.setCategories(value.split(';', Qt::SkipEmptyParts));
#else
    app.setCategories(value.split(';', QString::SkipEmptyParts));
#endif
```

### 2. QProcess::startDetached() 参数兼容性
**问题**: Qt5 的 `startDetached()` 需要 program 参数，Qt6 从对象属性读取

**文件**: `src/data/ApplicationRepository.cpp:101`

**修复**: 添加版本判断
```cpp
#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
    process.startDetached();
#else
    process.startDetached(exec);
#endif
```

---

## 📋 检查清单

| 检查项 | 状态 | 说明 |
|--------|------|------|
| Qt 版本优先级 | ✅ | Qt6 > Qt5 |
| C++ 标准 | ✅ | C++11 |
| QML 版本 | ✅ | 2.15 (兼容 Qt5/6) |
| 窗口根对象 | ✅ | ApplicationWindow |
| 窗口属性 | ✅ | 无边框/置顶/透明 |
| QString 兼容性 | ✅ | 已修复 |
| QProcess 兼容性 | ✅ | 已修复 |
| Debian 依赖 | ✅ | Qt6 包 |
| 维护脚本 | ✅ | 错误忽略 |

---

## 🔍 运行时诊断命令

在 Deepin 25 上执行：

```bash
# 检查依赖
ldd /usr/bin/Launcher | grep "not found"

# 检查 Qt 版本
qmake --version

# 检查 QML 模块
qml6-module-qtquick-controls --version

# 手动运行查看错误
Launcher 2>&1 | tee launcher.log

# 检查 desktop 文件
desktop-file-validate /usr/share/applications/launcher.desktop
```

---

## 📝 待办事项

- [ ] 测试安装后是否能正常启动
- [ ] 测试搜索功能是否正常
- [ ] 测试应用启动功能
- [ ] 测试文件搜索功能

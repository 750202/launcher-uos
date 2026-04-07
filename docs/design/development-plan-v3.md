# Launcher 开发计划 v3.0

基于 Figma 原型设计 - Qt 5 兼容版本（无剪贴板）

---

## 📋 项目信息

**项目名称**: Launcher - 聚焦搜索启动器
**目标平台**: Deepin 25 / Qt 5 兼容
**参考设计**: Figma 原型 + 参考项目（去掉剪贴板）
**开发周期**: 12-17 天

---

## 🔄 与 v2 计划的差异

| 项目 | v2 计划 | v3 计划 (当前) | 变更 |
|------|---------|----------------|------|
| 剪贴板模块 | 1-2天 | ❌ 删除 | -2天 |
| UI 组件模块 | 2-3天 | 3-4天 | +1天 |
| 总工期 | 12-17天 | 12-17天 | -1天 |

---

## 一、开发阶段总览

| 阶段 | 模块名称 | 预计工期 | 依赖 | 可交付物 |
|------|----------|----------|------|----------|
| 阶段1 | 基础设施层 | 1-2天 | 无 | 数据库、配置、单例 |
| 阶段2 | 应用搜索模块 | 2-3天 | 阶段1 | 应用加载、拼音匹配 |
| 阶段3 | 文件索引模块 | 3-4天 | 阶段1 | 文件索引、文件查询 |
| 阶段4 | 搜索编排模块 | 2-3天 | 阶段2+3 | 统一搜索、QML接口 |
| 阶段5 | UI 组件模块 | 3-4天 | 阶段4 | 毛玻璃、渐变边框、动画 |
| 阶段6 | 打包与集成 | 1天 | 阶段5 | Debian包、桌面图标 |

---

## 二、模块详细定义

### 模块 1: 基础设施层 (1-2天)

**功能**: 提供项目的基础服务

#### 1.1 Database - 数据库封装

**文件**: `src/data/Database.h/.cpp`

**接口方法**:
```cpp
static Database* instance();
bool open(const QString& path = "~/.local/share/launcher/launcher.db");
void close();
QSqlQuery query(const QString& sql);
bool execute(const QString& sql);
bool beginTransaction();
bool commit();
bool rollback();
```

**数据库表**:
- `settings` - 配置键值对
- `history` - 启动历史
- `favorites` - 收藏夹

#### 1.2 Settings - 配置管理

**文件**: `src/data/Settings.h/.cpp`

**接口方法**:
```cpp
QString theme() const / setTheme(const QString& theme)
QString hotkey() const / setHotkey(const QString& hotkey)
QStringList searchPaths() const / setSearchPaths(...)
bool rememberWindowPosition() const / setRememberWindowPosition(...)
QPoint lastWindowPosition() const / setLastWindowPosition(...)
```

**配置项**:
- 主题: light/dark/auto
- 快捷键: 默认 Ctrl+Q
- 窗口位置记忆: 默认 true
- 搜索路径: 默认 [QDir::homePath()]

#### 1.3 SingleInstance - 单例窗口管理

**文件**: `src/app/SingleInstance.h/.cpp`

**接口方法**:
```cpp
bool listen(const QString& name = "Launcher")
bool isRunning() const
void activateWindow()
```

**功能**: QSharedMemory + QLocalSocket 检测多个实例

---

### 模块 2: 应用搜索模块 (2-3天)

#### 2.1 Application - 应用数据模型

**文件**: `src/domain/Models.h`

```cpp
struct Application {
    QString id;           // .desktop 文件名
    QString name;         // 中文名称
    QString nameEn;       // 英文名称
    QString icon;         // 图标路径
    QString exec;         // 启动命令
    QString category;     // 分类
    QString description;  // 描述
    QString genericName;  // 通用名称
};
```

#### 2.2 ApplicationRepository - 应用仓库

**文件**: `src/data/ApplicationRepository.h/.cpp`

**接口方法**:
```cpp
QList<Application> loadApplications();
QList<Application> loadApplicationsFromPath(const QString& path);
Application findById(const QString& id);
QList<Application> searchByName(const QString& name, int limit = 20);
bool launch(const Application& app, const QStringList& args = {});
```

**功能**: 从 `/usr/share/applications` 加载应用信息

#### 2.3 PinyinMatcher - 拼音匹配器

**文件**: `src/services/PinyinMatcher.h/.cpp`

**接口方法**:
```cpp
static bool match(const QString& text, const QString& query);
static bool matchInitials(const QString& text, const QString& initials);
static int matchScore(const QString& text, const QString& query);
```

**支持**: 中文直接匹配、拼音首字母、拼音全拼

#### 2.4 AppSearchEngine - 应用搜索引擎

**文件**: `src/services/AppSearchEngine.h/.cpp`

**接口方法**:
```cpp
QList<Application> search(const QString& query, int limit = 10);
QList<Application> getRecommendedApps(int limit = 5);
```

**功能**: 结合应用仓库和拼音匹配

---

### 模块 3: 文件索引模块 (3-4天)

#### 3.1 FileIndex - 文件数据模型

**文件**: `src/domain/Models.h`

```cpp
struct FileIndex {
    QString id;
    QString path;
    QString name;
    QString extension;
    QString type;  // document/image/video/music/other
    qint64 size;
    qint64 modifiedTime;
    QString mimeType;
    bool isRecent;
};
```

#### 3.2 FileIndexer - 文件索引器

**文件**: `src/data/FileIndexer.h/.cpp`

**接口方法**:
```cpp
Q_INVOKABLE void startIndexing(const QStringList& paths);
Q_INVOKABLE void stopIndexing();

// 属性
bool isIndexing() const;
int progress() const;
int total() const;
QString currentPath() const;

// 信号
void progressChanged();
void finished();
void fileIndexed(const FileIndex& file);
```

**功能**: 递归扫描目录，提取文件信息

#### 3.3 FileRepository - 文件查询

**文件**: `src/data/FileRepository.h/.cpp`

**接口方法**:
```cpp
QList<FileIndex> searchByName(const QString& name, int limit = 10);
QList<FileIndex> searchByType(const QString& type, int limit = 10);
QList<FileIndex> searchRecent(int limit = 10);
QList<FileIndex> search(const QString& query,
                        const QStringList& types = {},
                        qint64 minSize = 0,
                        qint64 maxSize = LLONG_MAX,
                        int limit = 10);
bool openFile(const FileIndex& file);
```

---

### 模块 4: 搜索编排模块 (2-3天)

#### 4.1 SearchResult - 搜索结果

**文件**: `src/domain/Models.h`

```cpp
enum SearchResultType { App, File, Image, Video, Music, Document };

struct SearchResult {
    SearchResultType type;
    QString id;
    QString title;
    QString subtitle;
    QString icon;
    QString details;
    QVariant data;
    int score;
};
```

#### 4.2 SearchModel - 搜索结果模型

**文件**: `src/viewmodel/SearchModel.h/.cpp`

**接口方法**:
```cpp
// QAbstractListModel
int rowCount(const QModelIndex&) const override;
QVariant data(const QModelIndex&, int role) const override;

// 模型操作
void setResults(const QList<SearchResult>& results);
void filterByType(SearchResultType type);
```

**角色**: TypeRole, IdRole, TitleRole, SubtitleRole, IconRole, DetailsRole, DataRole

#### 4.3 SearchEngine - 搜索引擎

**文件**: `src/services/SearchEngine.h/.cpp`

**接口方法**:
```cpp
Q_INVOKABLE void search(const QString& query);
Q_INVOKABLE void clear();
Q_INVOKABLE void openItem(int index);
Q_INVOKABLE void filterByApplication();
Q_INVOKABLE void filterByFile();
```

#### 4.4 MainViewModel - 主视图模型

**文件**: `src/viewmodel/MainViewModel.h/.cpp`

**接口方法**:
```cpp
// 主题
bool isDarkTheme() const;
QString theme() const / setTheme(...)

// 窗口
QPoint windowPosition() const / setWindowPosition(...);
Q_INVOKABLE void showWindow();
Q_INVOKABLE void hideWindow();
```

#### 4.5 SearchViewModel - 搜索视图模型

**文件**: `src/viewmodel/SearchViewModel.h/.cpp`

**接口方法**:
```cpp
SearchEngine* engine() const;
QString query() const / setQuery(...);
QString category() const / setCategory(...);  // all/apps/files
Q_INVOKABLE void openSelectedItem();
```

---

### 模块 5: UI 组件模块 (3-4天) **重点**

**功能**: 实现 QML 界面和效果

#### 5.1 核心 QML 文件

| 文件 | 功能 | 技术重点 |
|------|------|----------|
| Main.qml | 主窗口 | 无边框、置顶 |
| GlassCard.qml | 毛玻璃卡片 | ✅ Qt 5 ShaderEffect |
| SearchBar.qml | 搜索框 | ✅ 渐变边框 + 毛玻璃 |
| CategoryButton.qml | 分类按钮 | 圆形、悬停动画 |
| SearchResults.qml | 结果列表 | 毛玻璃 + 渐变边框 |
| ResultItem.qml | 结果项 | 悬停/选中效果 |

#### 5.2 Qt 5 毛玻璃实现

**文件**: `qml/components/GlassCard.qml`

**技术**: ShaderEffect + GLSL 着色器（详见 `Qt5-毛玻璃实现指南.md`）

#### 5.3 渐变边框实现

**技术**: 双层容器结构
- 外层: 渐变背景 + 负边距（边框）
- 内层: 内容 + 圆角

#### 5.4 动画效果

| 效果 | 实现 | 时长 |
|------|------|------|
| 入场淡入 | OpacityAnimator | 200ms |
| 悬停缩放 | ScaleAnimator | 150ms |
| 结果项滑入 | SequentialAnimation | 30ms 间隔 |

---

### 模块 6: 打包与集成 (1天)

#### 6.1 CMakeLists.txt

**构建配置**: Qt 5 自动检测

```cmake
find_package(Qt5 COMPONENTS Core Quick Qml Sql REQUIRED)
```

#### 6.2 Debian 打包

**文件**: `debian/control`, `debian/rules`, `debian/postinst`

**依赖**: qt5-core, qt5-quick, qt5-sql 等

#### 6.3 桌面文件

**文件**: `resources/data/launcher.desktop`

**快捷键**: 指定为 Ctrl+Q

---

## 三、开发阶段详细计划

### 阶段 1: 基础设施层 (1-2天)

#### Day 1 上午
- [ ] 创建项目目录结构
- [ ] 配置 CMakeLists.txt
- [ ] 创建 Database 类
- [ ] 测试数据库基本操作

#### Day 1 下午
- [ ] 创建 Settings 类
- [ ] 测试配置读写
- [ ] 创建 SingleInstance 类
- [ ] 测试单例窗口

#### Day 2
- [ ] 集成 Qt GraphicalEffects
- [ ] 着色器资源准备
- [ ] 创建 QML 资源文件
- [ ] 更新 CMakeLists.txt

---

### 阶段 2: 应用搜索模块 (2-3天)

#### Day 1
- [ ] 创建 Application 模型
- [ ] 实现 ApplicationRepository::loadApplications()
- [ ] 测试加载系统应用

#### Day 2
- [ ] 实现 ApplicationRepository::searchByName()
- [ ] 实现 ApplicationRepository::launch()
- [ ] 测试应用启动

#### Day 3
- [ ] 实现 PinyinMatcher
- [ ] 实现 AppSearchEngine
- [ ] 测试拼音搜索（全拼和首字母）

---

### 阶段 3: 文件索引模块 (3-4天)

#### Day 1
- [ ] 创建 FileIndex 模型
- [ ] 实现 FileIndexer::indexDirectory()
- [ ] 测试单目录索引

#### Day 2
- [ ] 实现文件类型判断
- [ ] 支持常用文件格式
- [ ] 测试多目录索引

#### Day 3
- [ ] 实现 FileRepository 各类查询方法
- [ ] 实现索引进度信号
- [ ] 测试异步索引

#### Day 4（可选）
- [ ] 优化索引性能
- [ ] 增量索引实现（可选）

---

### 阶段 4: 搜索编排模块 (2-3天)

#### Day 1
- [ ] 创建 SearchResult 模型
- [ ] 实现 SearchModel
- [ ] 实现 SearchEngine

#### Day 2
- [ ] 实现 MainViewModel
- [ ] 实现 SearchViewModel
- [ ] 暴露给 QML 的接口

#### Day 3
- [ ] 集成应用搜索和文件搜索
- [ ] 测试统一搜索结果
- [ ] 完善错误处理

---

### 阶段 5: UI 组件模块 (3-4天) **重点**

#### Day 1: 毛玻璃 + 渐变边框
- [ ] 创建 GlassCard 组件（Qt 5 ShaderEffect）
- [ ] 创建 SearchBar（渐变边框）
- [ ] 创建 SearchResults（毛玻璃卡片）
- [ ] 测试效果

#### Day 2: 分类按钮 + 结果项
- [ ] 创建 CategoryButton（圆形、动画）
- [ ] 创建 ResultItem（悬停、选中）
- [ ] 创建 EmptyState
- [ ] 测试交互

#### Day 3: 主窗口
- [ ] 创建 Main.qml（无边框、置顶）
- [ ] 集成所有组件
- [ ] 实现快捷键
- [ ] 测试完整流程

#### Day 4（可选）
- [ ] 性能优化
- [ ] 动画调优
- [ ] 多显示器测试

**阶段 5 重点技术**:
- ✅ Qt 5 ShaderEffect 毛玻璃
- ✅ 双层容器渐变边框
- ✅ Behavior 动画
- ✅圆形分类按钮

---

### 阶段 6: 打包与集成 (1天)

#### 全天
- [ ] 创建 debian/ 目录和配置文件
- [ ] 创建桌面文件 launcher.desktop
- [ ] 创建应用图标（多尺寸）
- [ ] 配置快捷键
- [ ] 构建 Debian 包
- [ ] 安装测试

---

## 四、技术栈总结

| 组件 | 技术 | 版本 |
|------|------|------|
| C++ 框架 | Qt | 5.15 (兼容 Qt 6) |
| UI 层 | Qt Quick | 2.15 |
| UI 组件 | Qt Quick Controls | 2.15 |
| 毛玻璃 | Qt GraphicalEffects | 1.15 |
| 模糊效果 | ShaderEffect | GLSL |
| 构建工具 | CMake | 3.10+ |
| 数据库 | SQLite (Qt) | - |

---

## 五、非功能需求

| 需求项 | 指标 |
|--------|------|
| 毛玻璃性能 | 60fps 流畅 |
| 搜索响应 | < 300ms |
| 文件索引 | 10000 文件 < 5min |
| 内存占用 | < 200MB |
| 数据库大小 | 100000 文件 < 50MB |

---

## 六、验收标准

### 阶段 1 验收
- [ ] Database 正常打开/关闭
- [ ] Settings 读写配置
- [ ] SingleInstance 单例检测

### 阶段 2 验收
- [ ] 加载所有应用
- [ ] 中文搜索正常
- [ ] 拼音搜索正常
- [ ] 应用启动成功

### 阶段 3 验收
- [ ] 完整索引目录
- [ ] 按类型查询文件
- [ ] 文件打开成功

### 阶段 4 验收
- [ ] 统一搜索正常
- [ ] 分类筛选正常
- [ ] QML 接口可用

### 阶段 5 验收 ⭐ 重点
- [ ] 毛玻璃效果正确呈现
- [ ] 渐变边框显示正确
- [ ] 动画流畅无卡顿
- [ ] 所有交互正常

### 阶段 6 验收
- [ ] 成功构建 .deb 包
- [ ] 安装后显示图标
- [ ] 快捷键正常工作

---

## 七、风险与应对

| 风险 | 影响 | 应对措施 |
|------|------|----------|
| Qt 5 毛玻璃性能问题 | 高 | 使用 FastBlur 或禁用 |
| 文件索引时间过长 | 中 | 后台索引 + 进度显示 |
| 拼音匹配不准确 | 中 | 提供常见映射 |
| 快捷键冲突 | 低 | 提供配置选项 |

---

## 八、相关文档

| 文档 | 路径 |
|------|------|
| 功能需求 v3 | `docs/requirements/功能需求-v3.md` |
| 接口文档 v3 | `docs/interface/接口文档-v3.md` |
| Qt 5 毛玻璃指南 | `docs/guide/Qt5-毛玻璃实现指南.md` |
| 设计变更对比 | `docs/设计变更对比-最终版.md` |
| 参考项目 | `qt-apps/Lancher-reference/` |

---

**版本**: 3.0
**日期**: 2026-04-02
**状态**: ✅ 已确认（无剪贴板 + Qt 5 毛玻璃）

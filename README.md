# Launcher - 聚焦搜索启动器

![Development Status](https://img.shields.io/badge/status-phase_1_complete-blue)
![License](https://img.shields.io/badge/license-MIT-green)
![Platform](https://img.shields.io/badge/platform-deepin_25-orange)

一个类似于 macOS Spotlight 的快速搜索启动器，专为 Deepin 25 设计。

---

## 🎯 项目状态

**当前进度**: ✅ 节点 1 完成 - 基础设施 + 模拟数据

- GitHub: https://github.com/750202/launcher
- 开发阶段: 阶段 1/4

### 已完成功能（节点 1）

#### 后端 (C++)
- ✅ Database - SQLite 数据库和表结构
- ✅ Settings - 应用配置读写
- ✅ SingleInstance - 单例窗口管理
- ✅ HotkeyManager - 快捷键管理（占位符）

#### 前端 (QML)
- ✅ Main.qml - 主窗口界面
- ✅ 搜索框 - 实时搜索
- ✅ 分类按钮 - 全部/应用/文件切换
- ✅ 结果列表 - 模拟数据渲染
- ✅ 交互效果 - 悬停/点击动画

#### 开发基础设施
- ✅ CMakeLists.txt - Qt 5/6 兼容
- ✅ .gitignore - Git 配置
- ✅ 完整文档结构

### 计划中功能（后续节点）

**节点 2**: 搜索功能 + 真实数据
- 🔲 Application 模型和 Repository
- 🔲 文件索引模块
- 🔲 拼音匹配引擎
- 🔲 前后端数据联调

**节点 3**: 高级功能 + 优化
- 🔲 毛玻璃效果
- 🔲 动画完善
- 🔲 性能优化

**节点 4**: 打包与集成
- 🔲 Debian 打包
- 🔲 快捷键完整实现
- 🔲 系统集成

---

## 🚀 快速开始

### 系统要求

| 平台 | 最低要求 | 推荐版本 |
|------|----------|----------|
| **Deepin 25 / Debian** | Qt 5.12 | Qt 5.15 |
| **macOS** | Qt 5.12 | Qt 5.15 (Homebrew) |
| **Ubuntu** | Qt 5.12 | Qt 5.15 |

### 安装 Qt（如果未安装）

**macOS**:
```bash
brew install qt@5
export Qt5_DIR="/usr/local/opt/qt@5/lib/cmake/Qt5"
```

**Ubuntu/Debian**:
```bash
sudo apt install qt5-qmake qtbase5-dev qtdeclarative5-dev
```

### 构建项目

```bash
# 克隆仓库
git clone https://github.com/750202/launcher.git
cd launcher

# 构建
mkdir build && cd build
cmake ..
make -j$(nproc)

# 运行
./launcher
```

### 验证清单

- [ ] 窗口居中显示
- [ ] 分类按钮可以切换
- [ ] 搜索实时过滤（输入 "chrome"）
- [ ] 鼠标悬停效果正常
- [ ] 按 ESC 关闭窗口
- [ ] 单实例模式生效

---

## 📂 项目结构

```
Launcher/
├── src/                          # C++ 源代码
│   ├── data/                     # 数据层
│   │   ├── Database.cpp/.h       # 数据库管理
│   │   └── Settings.cpp/.h       # 配置管理
│   ├── app/                      # 应用层
│   │   ├── SingleInstance.cpp/.h # 单例窗口
│   │   └── HotkeyManager.cpp/.h  # 快捷键
│   ├── domain/                   # 领域模型 (待开发)
│   ├── services/                 # 业务服务 (待开发)
│   └── viewmodel/                # 视图模型 (待开发)
│
├── qml/                          # QML 界面
│   ├── Main.qml                  # 主窗口
│   └── components/               # UI 组件
│       ├── CategoryButton.qml
│       ├── SearchBar.qml
│       ├── SearchResults.qml
│       ├── ResultItem.qml
│       ├── GlassCard.qml
│       └── EmptyState.qml
│
├── docs/                         # 文档
│   ├── 节点1验证指南.md
│   ├── 详细开发计划.md
│   ├── design/                   # 设计文档
│   ├── requirements/             # 需求文档
│   └── interface/                # 接口文档
│
├── CMakeLists.txt                # 构建配置
├── README.md                     # 本文件
└── .gitignore
```

---

## 🛠️ 技术栈

| 组件 | 技术 | 说明 |
|------|------|------|
| C++ 框架 | Qt 5 / Qt 6 | 自动检测版本 |
| UI 层 | Qt Quick (QML) | 声明式界面 |
| UI 组件 | Qt Quick Controls 2 | 原生控件 |
| 构建工具 | CMake | 跨平台构建 |
| 数据库 | SQLite (Qt SQL) | 本地数据存储 |
| 目标平台 | Deepin 25 | Debian based |

---

## 📋 开发路线图

```
阶段 1: 基础设施 + 模拟数据  ✅ 完成
  ├─ Database + Settings
  ├─ SingleInstance + HotkeyManager
  └─ QML 主界面 + 模拟数据

阶段 2: 搜索功能 + 真实数据  ⏭️ 进行中
  ├─ Application 模型/Repository
  ├─ 文件索引模块
  ├─ 拼音匹配引擎
  └─ 前后端联调

阶段 3: 高级功能 + 优化      🔜 计划中
  ├─ 毛玻璃效果 (Shader)
  ├─ 动画完善
  └─ 性能优化

阶段 4: 打包与集成          🔜 计划中
  ├─ Debian .deb 包
  ├─ 快捷键完整实现
  └─ 系统集成
```

---

## 🤝 贡献

欢迎提交 Issue 和 Pull Request！

### 开发流程

1. fork 本仓库
2. 创建特性分支 (`git checkout -b feature/AmazingFeature`)
3. 提交更改 (`git commit -m 'feat: Add some AmazingFeature'`)
4. 推送到分支 (`git push origin feature/AmazingFeature`)
5. 开启 Pull Request

---

## 📝 开发文档

- [节点 1 验证指南](docs/节点1验证指南.md)
- [详细开发计划](docs/详细开发计划.md)
- [功能需求 v3](docs/requirements/功能需求-v3.md)

---

## 📄 许可证

MIT License

---

## 👥 作者

- **75020** - 项目维护者
- **Claude Code** - 代码生成助手

---

**最后更新**: 2026-04-02 | **版本**: 1.0.0-alpha (节点 1)

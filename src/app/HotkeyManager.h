#ifndef HOTKEYMANAGER_H
#define HOTKEYMANAGER_H

#include <QObject>
#include <QKeySequence>
#include <QAbstractNativeEventFilter>

/**
 * @brief 全局快捷键管理器
 *
 * 管理全局快捷键（如 Ctrl+Q 唤起窗口）
 *
 * 注意: Qt 5 没有内置的全局热键支持
 * Linux 平台需要 X11 特定代码或使用 libqxt
 * 当前实现为占位符，可在阶段 4 完善
 */
class HotkeyManager : public QObject, public QAbstractNativeEventFilter
{
    Q_OBJECT

public:
    // 单例实例
    static HotkeyManager* instance();

    // 初始化
    bool init();

    // 事件过滤器
#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
    bool nativeEventFilter(const QByteArray& eventType, void* message, qintptr* result) override;
#else
    bool nativeEventFilter(const QByteArray& eventType, void* message, long* result) override;
#endif

    // 快捷键触发信号
    Q_SIGNAL void hotkeyTriggered();

    // 快捷键配置
    void setHotkey(const QKeySequence& sequence);
    QKeySequence hotkey() const { return m_hotkeySequence; }

    // 启用/禁用
    void setEnabled(bool enabled);
    bool isEnabled() const { return m_enabled; }

    // 默认快捷键
    static const QKeySequence DEFAULT_HOTKEY;

private:
    explicit HotkeyManager(QObject* parent = nullptr);
    ~HotkeyManager() = default;

    // 禁止拷贝
    HotkeyManager(const HotkeyManager&) = delete;
    HotkeyManager& operator=(const HotkeyManager&) = delete;

private:
    static HotkeyManager* s_instance;
    QKeySequence m_hotkeySequence;
    bool m_enabled;
};

#endif // HOTKEYMANAGER_H

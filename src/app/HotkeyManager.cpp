#include "HotkeyManager.h"
#include <QGuiApplication>
#include <QDebug>

// 默认快捷键: Ctrl+Q
const QKeySequence HotkeyManager::DEFAULT_HOTKEY = QKeySequence("Ctrl+Q");

// 静态实例
HotkeyManager* HotkeyManager::s_instance = nullptr;

HotkeyManager* HotkeyManager::instance()
{
    if (!s_instance) {
        s_instance = new HotkeyManager();
    }
    return s_instance;
}

HotkeyManager::HotkeyManager(QObject* parent)
    : QObject(parent)
    , m_hotkeySequence(DEFAULT_HOTKEY)
    , m_enabled(false)
{
}

bool HotkeyManager::init()
{
    // 注册过滤器
    if (QGuiApplication::instance()) {
        QGuiApplication::instance()->installNativeEventFilter(this);
    }

    qDebug() << "HotkeyManager initialized with hotkey:" << m_hotkeySequence.toString();

    // 注意: 真正的全局快捷键捕获需要平台特定代码
    // Linux 平台可以考虑:
    // 1. 使用 XInput2 API (X11)
    // 2. 使用 uTouch (Wayland)
    // 3. 使用 libqxt-qt5

    // 当前实现作为占位符
    qDebug() << "Global hotkey capture is not fully implemented in this version";
    qDebug() << "Hotkey will be captured only when window has focus";

    return true;
}

#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
bool HotkeyManager::nativeEventFilter(const QByteArray& eventType, void* message, qintptr* result)
#else
bool HotkeyManager::nativeEventFilter(const QByteArray& eventType, void* message, long* result)
#endif
{
    Q_UNUSED(result)

    // Linux 平台处理
#ifdef Q_OS_LINUX
    if (!m_enabled) {
        return false;
    }

    // X11 平台
    if (eventType == "xcb_generic_event_t") {
        // TODO: 实现 X11 事件处理
        // 这是简化实现，完整版本需要处理 X11 键盘事件
    }
#endif

    return false;
}

void HotkeyManager::setHotkey(const QKeySequence& sequence)
{
    if (sequence.isEmpty()) {
        qWarning() << "Cannot set empty hotkey sequence";
        return;
    }

    m_hotkeySequence = sequence;
    qDebug() << "Hotkey changed to:" << m_hotkeySequence.toString();
}

void HotkeyManager::setEnabled(bool enabled)
{
    m_enabled = enabled;
    qDebug() << "HotkeyManager" << (enabled ? "enabled" : "disabled");
}

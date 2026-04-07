#include "Settings.h"
#include "Database.h"
#include <QSqlQuery>
#include <QSqlError>
#include <QDebug>

// 静态常量
const QString Settings::THEME_LIGHT = "light";
const QString Settings::THEME_DARK = "dark";
const QString Settings::THEME_AUTO = "auto";
const QString Settings::WINDOW_THEME_BLUR = "blur";
const QString Settings::WINDOW_THEME_SIMPLE = "simple";

// 静态实例
Settings* Settings::s_instance = nullptr;

Settings* Settings::instance()
{
    if (!s_instance) {
        s_instance = new Settings();
    }
    return s_instance;
}

Settings::Settings()
{
    // 初始化默认值
    m_cache["theme"] = THEME_AUTO;
    m_cache["history_limit"] = 50;
    m_cache["clipboard_enabled"] = false;
    m_cache["clipboard_limit"] = 20;
    m_cache["window_theme"] = WINDOW_THEME_SIMPLE;

    // 从数据库加载配置
    Database* db = Database::instance();
    if (!db->isOpen()) {
        qWarning() << "Database not open, using default settings";
        return;
    }

    QSqlQuery query = db->query("SELECT key, value FROM settings");
    if (!query.exec()) {
        qWarning() << "Failed to load settings:" << query.lastError().text();
        return;
    }

    while (query.next()) {
        QString key = query.value(0).toString();
        QString value = query.value(1).toString();
        m_cache[key] = value;
        qDebug() << "Loaded setting:" << key << "=" << value;
    }
}

void Settings::setValue(const QString& key, const QVariant& value)
{
    m_cache[key] = value;

    Database* db = Database::instance();
    if (!db->isOpen()) {
        qWarning() << "Database not open, setting not persisted";
        return;
    }

    QSqlQuery query(db->query(""));
    query.prepare("INSERT OR REPLACE INTO settings (key, value) VALUES (?, ?)");
    query.addBindValue(key);
    query.addBindValue(value.toString());

    if (!query.exec()) {
        qWarning() << "Failed to save setting:" << query.lastError().text();
        return;
    }

    qDebug() << "Saved setting:" << key << "=" << value.toString();
}

QVariant Settings::value(const QString& key, const QVariant& defaultValue) const
{
    if (m_cache.contains(key)) {
        return m_cache[key];
    }
    return defaultValue;
}

QString Settings::theme() const
{
    return value("theme", THEME_AUTO).toString();
}

void Settings::setTheme(const QString& theme)
{
    setValue("theme", theme);
}

int Settings::historyLimit() const
{
    return value("history_limit", 50).toInt();
}

void Settings::setHistoryLimit(int limit)
{
    setValue("history_limit", limit);
}

bool Settings::clipboardEnabled() const
{
    return value("clipboard_enabled", false).toBool();
}

void Settings::setClipboardEnabled(bool enabled)
{
    setValue("clipboard_enabled", enabled);
}

int Settings::clipboardLimit() const
{
    return value("clipboard_limit", 20).toInt();
}

void Settings::setClipboardLimit(int limit)
{
    setValue("clipboard_limit", limit);
}

QString Settings::windowTheme() const
{
    return value("window_theme", WINDOW_THEME_SIMPLE).toString();
}

void Settings::setWindowTheme(const QString& theme)
{
    setValue("window_theme", theme);
}

#ifndef SETTINGS_H
#define SETTINGS_H

#include <QString>
#include <QVariant>
#include <QMap>

/**
 * @brief 应用配置管理类
 *
 * 负责读写应用配置，数据存储在 SQLite 数据库中
 */
class Settings
{
public:
    // 单例实例
    static Settings* instance();

    // 读写配置
    void setValue(const QString& key, const QVariant& value);
    QVariant value(const QString& key, const QVariant& defaultValue = QVariant()) const;

    // 快捷访问常用配置
    QString theme() const;
    void setTheme(const QString& theme);

    int historyLimit() const;
    void setHistoryLimit(int limit);

    bool clipboardEnabled() const;
    void setClipboardEnabled(bool enabled);

    int clipboardLimit() const;
    void setClipboardLimit(int limit);

    QString windowTheme() const;
    void setWindowTheme(const QString& theme);

    // 预设主题
    static const QString THEME_LIGHT;
    static const QString THEME_DARK;
    static const QString THEME_AUTO;

    // 窗口主题
    static const QString WINDOW_THEME_BLUR;
    static const QString WINDOW_THEME_SIMPLE;

private:
    Settings();
    ~Settings() = default;

    // 禁止拷贝
    Settings(const Settings&) = delete;
    Settings& operator=(const Settings&) = delete;

    // 静态实例
    static Settings* s_instance;

private:
    mutable QMap<QString, QVariant> m_cache;
};

#endif // SETTINGS_H

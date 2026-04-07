#ifndef APPLICATION_H
#define APPLICATION_H

#include <QString>
#include <QStringList>
#include <QVariant>

/**
 * @brief 应用数据模型
 *
 * 表示一个系统应用程序，从 .desktop 文件加载
 */
class Application
{
public:
    Application();
    Application(const QString& id, const QString& name, const QString& iconPath,
                const QString& exec, const QString& desktopPath);

    // 基本信息
    QString id() const { return m_id; }
    void setId(const QString& id) { m_id = id; }

    QString name() const { return m_name; }
    void setName(const QString& name) { m_name = name; }

    QString genericName() const { return m_genericName; }
    void setGenericName(const QString& name) { m_genericName = name; }

    QString comment() const { return m_comment; }
    void setComment(const QString& comment) { m_comment = comment; }

    // 图标
    QString iconPath() const { return m_iconPath; }
    void setIconPath(const QString& path) { m_iconPath = path; }

    // 执行
    QString exec() const { return m_exec; }
    void setExec(const QString& exec) { m_exec = exec; }

    QString desktopPath() const { return m_desktopPath; }
    void setDesktopPath(const QString& path) { m_desktopPath = path; }

    // 分类
    QStringList categories() const { return m_categories; }
    void setCategories(const QStringList& categories) { m_categories = categories; }

    // 状态
    bool isValid() const { return m_valid; }
    void setValid(bool valid) { m_valid = valid; }

    bool isHidden() const { return m_hidden; }
    void setHidden(bool hidden) { m_hidden = hidden; }

    bool isNoDisplay() const { return m_noDisplay; }
    void setNoDisplay(bool noDisplay) { m_noDisplay = noDisplay; }

    // 匹配搜索
    bool matchesQuery(const QString& query) const;

    // 转换为 QVariantMap（用于 QML）
    QVariantMap toVariantMap() const;

    // 拼音匹配支持
    QString searchableText() const;

private:
    QString m_id;           // 唯一标识（通常是 desktop 文件名）
    QString m_name;         // 显示名称
    QString m_genericName;  // 通用名称
    QString m_comment;      // 描述/注释
    QString m_iconPath;     // 图标路径或名称
    QString m_exec;         // 执行命令
    QString m_desktopPath;  // .desktop 文件路径
    QStringList m_categories; // 分类
    bool m_valid = true;    // 是否有效
    bool m_hidden = false;  // 是否隐藏
    bool m_noDisplay = false; // NoDisplay 标志
};

#endif // APPLICATION_H

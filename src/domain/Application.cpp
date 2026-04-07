#include "Application.h"
#include <QRegularExpression>

Application::Application()
{
}

Application::Application(const QString& id, const QString& name, const QString& iconPath,
                         const QString& exec, const QString& desktopPath)
    : m_id(id)
    , m_name(name)
    , m_iconPath(iconPath)
    , m_exec(exec)
    , m_desktopPath(desktopPath)
    , m_valid(true)
{
}

bool Application::matchesQuery(const QString& query) const
{
    if (query.isEmpty()) {
        return true;
    }

    QString lowerQuery = query.toLower();

    // 匹配名称
    if (m_name.toLower().contains(lowerQuery)) {
        return true;
    }

    // 匹配通用名称
    if (!m_genericName.isEmpty() && m_genericName.toLower().contains(lowerQuery)) {
        return true;
    }

    // 匹配注释
    if (!m_comment.isEmpty() && m_comment.toLower().contains(lowerQuery)) {
        return true;
    }

    return false;
}

QVariantMap Application::toVariantMap() const
{
    QVariantMap map;
    map["id"] = m_id;
    map["name"] = m_name;
    map["genericName"] = m_genericName;
    map["comment"] = m_comment;
    map["iconPath"] = m_iconPath;
    map["exec"] = m_exec;
    map["desktopPath"] = m_desktopPath;
    map["categories"] = m_categories;
    map["valid"] = m_valid;
    map["hidden"] = m_hidden;
    map["noDisplay"] = m_noDisplay;
    return map;
}

QString Application::searchableText() const
{
    // 返回用于搜索的文本（名称 + 通用名称 + 注释）
    QStringList parts;
    parts << m_name;
    if (!m_genericName.isEmpty()) {
        parts << m_genericName;
    }
    if (!m_comment.isEmpty()) {
        parts << m_comment;
    }
    return parts.join(" ");
}

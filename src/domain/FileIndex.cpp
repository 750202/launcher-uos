#include "FileIndex.h"
#include <QFileInfo>
#include <QDateTime>

FileIndex::FileIndex()
{
}

FileIndex::FileIndex(const QString& id, const QString& name, const QString& path)
    : m_id(id)
    , m_name(name)
    , m_path(path)
    , m_valid(true)
{
}

bool FileIndex::exists() const
{
    return QFileInfo(m_path).exists();
}

bool FileIndex::matchesQuery(const QString& query) const
{
    if (query.isEmpty()) {
        return true;
    }

    QString lowerQuery = query.toLower();

    // 匹配文件名
    if (m_name.toLower().contains(lowerQuery)) {
        return true;
    }

    // 匹配路径
    if (m_path.toLower().contains(lowerQuery)) {
        return true;
    }

    // 匹配扩展名
    if (!m_extension.isEmpty() && m_extension.toLower().contains(lowerQuery)) {
        return true;
    }

    return false;
}

QVariantMap FileIndex::toVariantMap() const
{
    QVariantMap map;
    map["id"] = m_id;
    map["name"] = m_name;
    map["path"] = m_path;
    map["extension"] = m_extension;
    map["size"] = m_size;
    map["formattedSize"] = formattedSize();
    map["modifiedTime"] = m_modifiedTime;
    map["formattedModifiedTime"] = formattedModifiedTime();
    map["mimeType"] = m_mimeType;
    map["category"] = m_category;
    map["valid"] = m_valid;
    return map;
}

QString FileIndex::searchableText() const
{
    return m_name + " " + m_path;
}

QString FileIndex::formattedSize() const
{
    const qint64 KB = 1024;
    const qint64 MB = KB * 1024;
    const qint64 GB = MB * 1024;

    if (m_size >= GB) {
        return QString::number(m_size / (double)GB, 'f', 1) + " GB";
    } else if (m_size >= MB) {
        return QString::number(m_size / (double)MB, 'f', 1) + " MB";
    } else if (m_size >= KB) {
        return QString::number(m_size / (double)KB, 'f', 1) + " KB";
    } else {
        return QString::number(m_size) + " B";
    }
}

QString FileIndex::formattedModifiedTime() const
{
    if (!m_modifiedTime.isValid()) {
        return "";
    }

    QDateTime now = QDateTime::currentDateTime();
    qint64 daysDiff = m_modifiedTime.daysTo(now);

    if (daysDiff == 0) {
        return "今天";
    } else if (daysDiff == 1) {
        return "昨天";
    } else if (daysDiff < 7) {
        return QString::number(daysDiff) + "天前";
    } else if (daysDiff < 30) {
        return QString::number(daysDiff / 7) + "周前";
    } else if (daysDiff < 365) {
        return QString::number(daysDiff / 30) + "月前";
    } else {
        return m_modifiedTime.toString("yyyy-MM-dd");
    }
}

#ifndef FILEINDEX_H
#define FILEINDEX_H

#include <QString>
#include <QDateTime>
#include <QVariant>

/**
 * @brief 文件索引数据模型
 *
 * 表示一个已索引的文件，包含基本信息和搜索属性
 */
class FileIndex
{
public:
    FileIndex();
    FileIndex(const QString& id, const QString& name, const QString& path);

    // 基本信息
    QString id() const { return m_id; }
    void setId(const QString& id) { m_id = id; }

    QString name() const { return m_name; }
    void setName(const QString& name) { m_name = name; }

    QString path() const { return m_path; }
    void setPath(const QString& path) { m_path = path; }

    QString extension() const { return m_extension; }
    void setExtension(const QString& ext) { m_extension = ext; }

    // 文件属性
    qint64 size() const { return m_size; }
    void setSize(qint64 size) { m_size = size; }

    QDateTime modifiedTime() const { return m_modifiedTime; }
    void setModifiedTime(const QDateTime& time) { m_modifiedTime = time; }

    QString mimeType() const { return m_mimeType; }
    void setMimeType(const QString& type) { m_mimeType = type; }

    // 分类
    QString category() const { return m_category; }
    void setCategory(const QString& category) { m_category = category; }

    // 状态
    bool isValid() const { return m_valid; }
    void setValid(bool valid) { m_valid = valid; }

    bool exists() const;

    // 匹配搜索
    bool matchesQuery(const QString& query) const;

    // 转换为 QVariantMap（用于 QML）
    QVariantMap toVariantMap() const;

    // 搜索文本
    QString searchableText() const;

    // 格式化大小
    QString formattedSize() const;

    // 格式化时间
    QString formattedModifiedTime() const;

private:
    QString m_id;           // 唯一标识
    QString m_name;         // 文件名
    QString m_path;         // 完整路径
    QString m_extension;    // 扩展名
    qint64 m_size = 0;      // 文件大小（字节）
    QDateTime m_modifiedTime; // 修改时间
    QString m_mimeType;     // MIME 类型
    QString m_category;     // 分类（文档、图片、视频等）
    bool m_valid = true;    // 是否有效
};

#endif // FILEINDEX_H

#ifndef FILEINDEXER_H
#define FILEINDEXER_H

#include "FileIndex.h"
#include <QString>
#include <QList>
#include <QStringList>
#include <QObject>

/**
 * @brief 文件索引器
 *
 * 负责扫描指定目录，建立文件索引
 */
class FileIndexer : public QObject
{
    Q_OBJECT

public:
    // 单例实例
    static FileIndexer* instance();

    // 扫描目录
    void setScanDirectories(const QStringList& directories);
    QStringList scanDirectories() const { return m_scanDirectories; }

    // 开始索引
    void startIndexing();
    void stopIndexing();

    // 获取索引
    QList<FileIndex> allFiles() const { return m_files; }
    int count() const { return m_files.size(); }

    // 搜索文件
    QList<FileIndex> searchFiles(const QString& query) const;

    // 状态
    bool isIndexing() const { return m_indexing; }
    int progress() const { return m_progress; }

    // 设置过滤器
    void setNameFilters(const QStringList& filters);
    void setMaxDepth(int depth) { m_maxDepth = depth; }
    void setMaxFileSize(qint64 size) { m_maxFileSize = size; }

    // 默认扫描目录
    static QStringList defaultScanDirectories();

signals:
    void indexingStarted();
    void indexingProgress(int current, int total);
    void indexingFinished(int fileCount);
    void fileFound(const FileIndex& file);

private:
    FileIndexer();
    ~FileIndexer() = default;

    // 禁止拷贝
    FileIndexer(const FileIndexer&) = delete;
    FileIndexer& operator=(const FileIndexer&) = delete;

    // 扫描单个目录
    void scanDirectory(const QString& directoryPath, int depth = 0);

    // 确定文件分类
    QString determineCategory(const QString& fileName, const QString& mimeType) const;

    // 生成文件 ID
    QString generateFileId(const QString& path) const;

private:
    static FileIndexer* s_instance;
    QStringList m_scanDirectories;
    QList<FileIndex> m_files;
    QStringList m_nameFilters;
    int m_maxDepth = 5;
    qint64 m_maxFileSize = 100 * 1024 * 1024; // 100MB
    bool m_indexing = false;
    bool m_stopRequested = false;
    int m_progress = 0;
    int m_filesScanned = 0;
};

#endif // FILEINDEXER_H

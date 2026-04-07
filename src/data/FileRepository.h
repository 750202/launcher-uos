#ifndef FILEREPOSITORY_H
#define FILEREPOSITORY_H

#include "FileIndex.h"
#include <QString>
#include <QList>

/**
 * @brief 文件仓库类
 *
 * 负责文件查询、打开等操作
 */
class FileRepository
{
public:
    // 单例实例
    static FileRepository* instance();

    // 搜索文件
    QList<FileIndex> searchFiles(const QString& query) const;

    // 根据 ID 获取文件
    FileIndex fileById(const QString& id) const;

    // 打开文件
    bool openFile(const QString& id) const;
    bool openFile(const FileIndex& file) const;

    // 在文件管理器中显示
    bool revealInFileManager(const QString& path) const;

    // 获取最近文件
    QList<FileIndex> recentFiles(int limit = 20) const;

private:
    FileRepository();
    ~FileRepository() = default;

    // 禁止拷贝
    FileRepository(const FileRepository&) = delete;
    FileRepository& operator=(const FileRepository&) = delete;

private:
    static FileRepository* s_instance;
};

#endif // FILEREPOSITORY_H

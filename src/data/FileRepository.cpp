#include "FileRepository.h"
#include "FileIndexer.h"
#include <QDesktopServices>
#include <QUrl>
#include <QProcess>
#include <QFileInfo>
#include <QDebug>

FileRepository* FileRepository::s_instance = nullptr;

FileRepository* FileRepository::instance()
{
    if (!s_instance) {
        s_instance = new FileRepository();
    }
    return s_instance;
}

FileRepository::FileRepository()
{
}

QList<FileIndex> FileRepository::searchFiles(const QString& query) const
{
    return FileIndexer::instance()->searchFiles(query);
}

FileIndex FileRepository::fileById(const QString& id) const
{
    QList<FileIndex> files = FileIndexer::instance()->allFiles();
    for (const FileIndex& file : files) {
        if (file.id() == id) {
            return file;
        }
    }
    return FileIndex();
}

bool FileRepository::openFile(const QString& id) const
{
    FileIndex file = fileById(id);
    if (file.isValid()) {
        return openFile(file);
    }
    return false;
}

bool FileRepository::openFile(const FileIndex& file) const
{
    if (!file.isValid() || !file.exists()) {
        qWarning() << "File does not exist:" << file.path();
        return false;
    }

    QUrl url = QUrl::fromLocalFile(file.path());
    bool result = QDesktopServices::openUrl(url);

    if (result) {
        qDebug() << "Opened file:" << file.name();
    } else {
        qWarning() << "Failed to open file:" << file.path();
    }

    return result;
}

bool FileRepository::revealInFileManager(const QString& path) const
{
    QFileInfo fileInfo(path);
    if (!fileInfo.exists()) {
        return false;
    }

    QString dirPath = fileInfo.absolutePath();

    // 尝试使用不同的文件管理器
    // Deepin/UOS 使用 dde-file-manager
    // 通用方案使用 xdg-open

    bool success = false;

#ifdef Q_OS_LINUX
    // 尝试 deepin 文件管理器
    QProcess process;
    process.start("dde-file-manager", QStringList() << "--show-item" << path);
    success = process.waitForStarted();
    if (success) {
        process.waitForFinished();
        return true;
    }
#endif

    // 回退到打开所在目录
    QUrl url = QUrl::fromLocalFile(dirPath);
    return QDesktopServices::openUrl(url);
}

QList<FileIndex> FileRepository::recentFiles(int limit) const
{
    QList<FileIndex> files = FileIndexer::instance()->allFiles();

    // 按修改时间排序
    std::sort(files.begin(), files.end(), [](const FileIndex& a, const FileIndex& b) {
        return a.modifiedTime() > b.modifiedTime();
    });

    // 限制数量
    if (files.size() > limit) {
        files = files.mid(0, limit);
    }

    return files;
}

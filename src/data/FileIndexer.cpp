#include "FileIndexer.h"
#include <QDir>
#include <QDirIterator>
#include <QFileInfo>
#include <QMimeDatabase>
#include <QMimeType>
#include <QCryptographicHash>
#include <QDebug>
#include <QStandardPaths>

FileIndexer* FileIndexer::s_instance = nullptr;

FileIndexer* FileIndexer::instance()
{
    if (!s_instance) {
        s_instance = new FileIndexer();
    }
    return s_instance;
}

FileIndexer::FileIndexer()
    : QObject(nullptr)
{
    // 设置默认扫描目录
    m_scanDirectories = defaultScanDirectories();

    // 默认过滤器：排除隐藏文件和系统目录
    m_nameFilters = QStringList();
}

void FileIndexer::setScanDirectories(const QStringList& directories)
{
    m_scanDirectories = directories;
}

void FileIndexer::startIndexing()
{
    if (m_indexing) {
        return;
    }

    m_indexing = true;
    m_stopRequested = false;
    m_files.clear();
    m_progress = 0;
    m_filesScanned = 0;

    emit indexingStarted();

    qDebug() << "Starting file indexing...";

    for (const QString& directory : m_scanDirectories) {
        if (m_stopRequested) {
            break;
        }
        scanDirectory(directory);
    }

    m_indexing = false;
    m_progress = 100;

    qDebug() << "Indexing finished. Total files:" << m_files.size();
    emit indexingFinished(m_files.size());
}

void FileIndexer::stopIndexing()
{
    m_stopRequested = true;
}

QList<FileIndex> FileIndexer::searchFiles(const QString& query) const
{
    if (query.isEmpty()) {
        // 返回最近修改的文件（最多100个）
        QList<FileIndex> recent = m_files;
        std::sort(recent.begin(), recent.end(), [](const FileIndex& a, const FileIndex& b) {
            return a.modifiedTime() > b.modifiedTime();
        });
        if (recent.size() > 100) {
            recent = recent.mid(0, 100);
        }
        return recent;
    }

    QList<FileIndex> results;
    for (const FileIndex& file : m_files) {
        if (file.matchesQuery(query)) {
            results.append(file);
        }
    }
    return results;
}

void FileIndexer::setNameFilters(const QStringList& filters)
{
    m_nameFilters = filters;
}

QStringList FileIndexer::defaultScanDirectories()
{
    QStringList dirs;

    // 用户主目录
    QString home = QStandardPaths::writableLocation(QStandardPaths::HomeLocation);
    if (!home.isEmpty()) {
        dirs << home + "/Documents"
             << home + "/Downloads"
             << home + "/Desktop";
    }

    return dirs;
}

void FileIndexer::scanDirectory(const QString& directoryPath, int depth)
{
    if (m_stopRequested || depth > m_maxDepth) {
        return;
    }

    QDir dir(directoryPath);
    if (!dir.exists()) {
        return;
    }

    QDirIterator it(directoryPath, QDir::Files | QDir::Dirs | QDir::NoDotAndDotDot | QDir::NoSymLinks);
    while (it.hasNext() && !m_stopRequested) {
        QString path = it.next();
        QFileInfo fileInfo(path);

        // 跳过隐藏文件
        if (fileInfo.isHidden()) {
            continue;
        }

        // 跳过过大文件
        if (fileInfo.isFile() && fileInfo.size() > m_maxFileSize) {
            continue;
        }

        if (fileInfo.isDir()) {
            // 跳过一些不需要索引的目录
            QString dirName = fileInfo.fileName().toLower();
            if (dirName.startsWith('.') ||
                dirName == "node_modules" ||
                dirName == ".git" ||
                dirName == ".svn" ||
                dirName == "build" ||
                dirName == "debug" ||
                dirName == "release") {
                continue;
            }
            scanDirectory(path, depth + 1);
        } else if (fileInfo.isFile()) {
            // 创建文件索引
            FileIndex file;
            file.setId(generateFileId(path));
            file.setName(fileInfo.fileName());
            file.setPath(path);
            file.setExtension(fileInfo.suffix().toLower());
            file.setSize(fileInfo.size());
            file.setModifiedTime(fileInfo.lastModified());

            // 获取 MIME 类型
            QMimeDatabase mimeDb;
            QMimeType mimeType = mimeDb.mimeTypeForFile(fileInfo);
            file.setMimeType(mimeType.name());

            // 确定分类
            file.setCategory(determineCategory(fileInfo.fileName(), mimeType.name()));

            m_files.append(file);
            m_filesScanned++;

            // 更新进度
            if (m_filesScanned % 100 == 0) {
                emit indexingProgress(m_filesScanned, -1);
            }
        }
    }
}

QString FileIndexer::determineCategory(const QString& fileName, const QString& mimeType) const
{
    QString ext = QFileInfo(fileName).suffix().toLower();

    // 文档
    if (mimeType.startsWith("application/pdf") ||
        mimeType.startsWith("application/msword") ||
        mimeType.startsWith("application/vnd.") ||
        ext == "pdf" || ext == "doc" || ext == "docx" ||
        ext == "xls" || ext == "xlsx" ||
        ext == "ppt" || ext == "pptx" ||
        ext == "txt" || ext == "rtf" || ext == "md") {
        return "document";
    }

    // 图片
    if (mimeType.startsWith("image/") ||
        ext == "jpg" || ext == "jpeg" || ext == "png" ||
        ext == "gif" || ext == "bmp" || ext == "svg" || ext == "webp") {
        return "image";
    }

    // 视频
    if (mimeType.startsWith("video/") ||
        ext == "mp4" || ext == "avi" || ext == "mkv" ||
        ext == "mov" || ext == "wmv" || ext == "flv") {
        return "video";
    }

    // 音频
    if (mimeType.startsWith("audio/") ||
        ext == "mp3" || ext == "wav" || ext == "flac" ||
        ext == "aac" || ext == "ogg" || ext == "m4a") {
        return "audio";
    }

    // 压缩包
    if (mimeType.startsWith("application/zip") ||
        mimeType.startsWith("application/x-") ||
        ext == "zip" || ext == "rar" || ext == "7z" ||
        ext == "tar" || ext == "gz" || ext == "bz2") {
        return "archive";
    }

    // 代码
    if (ext == "cpp" || ext == "h" || ext == "py" || ext == "js" ||
        ext == "ts" || ext == "java" || ext == "go" || ext == "rs" ||
        ext == "sh" || ext == "json" || ext == "xml" || ext == "yaml" || ext == "yml") {
        return "code";
    }

    return "other";
}

QString FileIndexer::generateFileId(const QString& path) const
{
    return QString::fromLatin1(
        QCryptographicHash::hash(path.toUtf8(), QCryptographicHash::Md5).toHex()
    );
}

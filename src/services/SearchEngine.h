#ifndef SEARCHENGINE_H
#define SEARCHENGINE_H

#include "Application.h"
#include "FileIndex.h"
#include <QObject>
#include <QList>

class ApplicationRepository;
class FileIndexer;

/**
 * @brief 搜索引擎
 *
 * 协调应用搜索和文件搜索，合并结果
 */
class SearchEngine : public QObject
{
    Q_OBJECT

    // QML 可访问属性
    Q_PROPERTY(QString query READ query WRITE setQuery NOTIFY queryChanged)
    Q_PROPERTY(QString category READ category WRITE setCategory NOTIFY categoryChanged)
    Q_PROPERTY(int appCount READ appCount NOTIFY resultsChanged)
    Q_PROPERTY(int fileCount READ fileCount NOTIFY resultsChanged)
    Q_PROPERTY(bool searching READ searching NOTIFY searchingChanged)

public:
    explicit SearchEngine(QObject* parent = nullptr);
    ~SearchEngine() override;

    static SearchEngine* instance();

    // 初始化（加载应用、索引文件）
    Q_INVOKABLE void init();

    // 搜索
    Q_INVOKABLE void search();

    // 清空结果
    Q_INVOKABLE void clear();

    // 获取结果
    QList<Application> appResults() const { return m_appResults; }
    QList<FileIndex> fileResults() const { return m_fileResults; }

    // 属性访问
    QString query() const { return m_query; }
    void setQuery(const QString& query);

    QString category() const { return m_category; }
    void setCategory(const QString& category);

    int appCount() const { return m_appResults.size(); }
    int fileCount() const { return m_fileResults.size(); }
    bool searching() const { return m_searching; }

    // 启动应用
    Q_INVOKABLE bool launchApp(const QString& id);

    // 打开文件
    Q_INVOKABLE bool openFile(const QString& id);

    // 在文件管理器中显示
    Q_INVOKABLE bool revealFile(const QString& id);

signals:
    void queryChanged();
    void categoryChanged();
    void resultsChanged();
    void searchingChanged();
    void searchCompleted();

private:
    void setSearching(bool searching);

private:
    QString m_query;
    QString m_category;  // "all", "apps", "files"
    QList<Application> m_appResults;
    QList<FileIndex> m_fileResults;
    bool m_searching = false;
    bool m_initialized = false;

    ApplicationRepository* m_appRepo = nullptr;
    FileIndexer* m_fileIndexer = nullptr;
};

#endif // SEARCHENGINE_H

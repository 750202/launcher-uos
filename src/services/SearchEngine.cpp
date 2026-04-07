#include "SearchEngine.h"
#include "ApplicationRepository.h"
#include "FileIndexer.h"
#include "FileRepository.h"
#include <QDebug>

SearchEngine* SearchEngine::instance()
{
    static SearchEngine* s_instance = nullptr;
    if (!s_instance) {
        s_instance = new SearchEngine();
    }
    return s_instance;
}

SearchEngine::SearchEngine(QObject* parent)
    : QObject(parent)
    , m_category("all")
{
}

SearchEngine::~SearchEngine()
{
}

void SearchEngine::init()
{
    if (m_initialized) {
        return;
    }

    qDebug() << "SearchEngine initializing...";

    // 获取仓库实例
    m_appRepo = ApplicationRepository::instance();
    m_fileIndexer = FileIndexer::instance();

    // 加载应用
    setSearching(true);
    m_appRepo->loadApplications();
    qDebug() << "Loaded" << m_appRepo->count() << "applications";

    // 索引文件
    m_fileIndexer->startIndexing();
    qDebug() << "Indexed" << m_fileIndexer->count() << "files";

    m_initialized = true;
    setSearching(false);

    qDebug() << "SearchEngine initialized";
}

void SearchEngine::search()
{
    if (!m_initialized) {
        init();
    }

    setSearching(true);

    m_appResults.clear();
    m_fileResults.clear();

    QString query = m_query.toLower().trimmed();

    // 根据分类搜索
    if (m_category == "apps" || m_category == "all") {
        m_appResults = m_appRepo->searchApplications(query);
        qDebug() << "App search results:" << m_appResults.size();
    }

    if (m_category == "files" || m_category == "all") {
        m_fileResults = m_fileIndexer->searchFiles(query);
        qDebug() << "File search results:" << m_fileResults.size();
    }

    setSearching(false);
    emit resultsChanged();
    emit searchCompleted();
}

void SearchEngine::clear()
{
    m_appResults.clear();
    m_fileResults.clear();
    emit resultsChanged();
}

void SearchEngine::setQuery(const QString& query)
{
    if (m_query != query) {
        m_query = query;
        emit queryChanged();
    }
}

void SearchEngine::setCategory(const QString& category)
{
    if (m_category != category) {
        m_category = category;
        emit categoryChanged();
    }
}

bool SearchEngine::launchApp(const QString& id)
{
    return m_appRepo->launchApplication(id);
}

bool SearchEngine::openFile(const QString& id)
{
    return FileRepository::instance()->openFile(id);
}

bool SearchEngine::revealFile(const QString& id)
{
    FileIndex file = m_fileIndexer->searchFiles("").isEmpty() ? FileIndex() :
        *std::find_if(m_fileIndexer->allFiles().begin(), m_fileIndexer->allFiles().end(),
            [&id](const FileIndex& f) { return f.id() == id; });

    if (file.isValid()) {
        return FileRepository::instance()->revealInFileManager(file.path());
    }
    return false;
}

void SearchEngine::setSearching(bool searching)
{
    if (m_searching != searching) {
        m_searching = searching;
        emit searchingChanged();
    }
}

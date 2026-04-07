#include "SearchViewModel.h"
#include <QDebug>

SearchViewModel* SearchViewModel::instance()
{
    static SearchViewModel* s_instance = nullptr;
    if (!s_instance) {
        s_instance = new SearchViewModel();
    }
    return s_instance;
}

SearchViewModel::SearchViewModel(QObject* parent)
    : QObject(parent)
    , m_engine(SearchEngine::instance())
    , m_model(new SearchModel(this))
    , m_debounceTimer(new QTimer(this))
{
    // 设置防抖定时器为单次触发
    m_debounceTimer->setSingleShot(true);
    m_debounceTimer->setInterval(m_debounceDelay);

    // 连接定时器到搜索
    connect(m_debounceTimer, &QTimer::timeout, this, &SearchViewModel::search);

    // 连接引擎信号
    connect(m_engine, &SearchEngine::queryChanged, this, &SearchViewModel::queryChanged);
    connect(m_engine, &SearchEngine::categoryChanged, this, &SearchViewModel::categoryChanged);
    connect(m_engine, &SearchEngine::searchingChanged, this, &SearchViewModel::searchingChanged);
    connect(m_engine, &SearchEngine::searchCompleted, this, &SearchViewModel::onSearchCompleted);
}

SearchViewModel::~SearchViewModel()
{
}

void SearchViewModel::init()
{
    qDebug() << "SearchViewModel initializing...";
    m_engine->init();
    qDebug() << "SearchViewModel initialized";
}

void SearchViewModel::setQuery(const QString& query)
{
    m_engine->setQuery(query);

    // 触发防抖搜索
    if (query.isEmpty()) {
        m_debounceTimer->stop();
        clear();
    } else {
        m_debounceTimer->start();
    }
}

void SearchViewModel::setCategory(const QString& category)
{
    m_engine->setCategory(category);

    // 如果有查询内容，立即搜索
    if (!m_engine->query().isEmpty()) {
        search();
    }
}

void SearchViewModel::search()
{
    m_engine->search();
}

void SearchViewModel::clear()
{
    m_engine->clear();
    m_model->clear();
    emit resultCountChanged();
}

bool SearchViewModel::launchApp(const QString& id)
{
    return m_engine->launchApp(id);
}

bool SearchViewModel::openFile(const QString& id)
{
    return m_engine->openFile(id);
}

bool SearchViewModel::revealFile(const QString& id)
{
    return m_engine->revealFile(id);
}

void SearchViewModel::onSearchCompleted()
{
    updateModel();
    emit resultCountChanged();
}

void SearchViewModel::updateModel()
{
    // 获取应用结果
    QVariantList appResults;
    for (const Application& app : m_engine->appResults()) {
        appResults.append(app.toVariantMap());
    }

    // 获取文件结果
    QVariantList fileResults;
    for (const FileIndex& file : m_engine->fileResults()) {
        fileResults.append(file.toVariantMap());
    }

    // 更新模型
    if (m_engine->category() == "apps") {
        m_model->setAppResults(appResults);
    } else if (m_engine->category() == "files") {
        m_model->setFileResults(fileResults);
    } else {
        m_model->setResults(appResults, fileResults);
    }
}

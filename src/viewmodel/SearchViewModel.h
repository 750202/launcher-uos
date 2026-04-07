#ifndef SEARCHVIEWMODEL_H
#define SEARCHVIEWMODEL_H

#include "SearchEngine.h"
#include "SearchModel.h"
#include <QObject>
#include <QTimer>

/**
 * @brief 搜索视图模型
 *
 * 为 QML 提供搜索功能接口，协调 SearchEngine 和 SearchModel
 */
class SearchViewModel : public QObject
{
    Q_OBJECT

    // QML 可访问属性
    Q_PROPERTY(SearchModel* model READ model CONSTANT)
    Q_PROPERTY(QString query READ query WRITE setQuery NOTIFY queryChanged)
    Q_PROPERTY(QString category READ category WRITE setCategory NOTIFY categoryChanged)
    Q_PROPERTY(bool searching READ searching NOTIFY searchingChanged)
    Q_PROPERTY(int resultCount READ resultCount NOTIFY resultCountChanged)
    Q_PROPERTY(bool hasResults READ hasResults NOTIFY resultCountChanged)

public:
    explicit SearchViewModel(QObject* parent = nullptr);
    ~SearchViewModel() override;

    static SearchViewModel* instance();

    // 初始化
    Q_INVOKABLE void init();

    // 属性访问
    SearchModel* model() const { return m_model; }
    QString query() const { return m_engine->query(); }
    void setQuery(const QString& query);
    QString category() const { return m_engine->category(); }
    void setCategory(const QString& category);
    bool searching() const { return m_engine->searching(); }
    int resultCount() const { return m_model->rowCount(); }
    bool hasResults() const { return resultCount() > 0; }

    // 操作
    Q_INVOKABLE void search();
    Q_INVOKABLE void clear();
    Q_INVOKABLE bool launchApp(const QString& id);
    Q_INVOKABLE bool openFile(const QString& id);
    Q_INVOKABLE bool revealFile(const QString& id);

signals:
    void queryChanged();
    void categoryChanged();
    void searchingChanged();
    void resultCountChanged();

private slots:
    void onSearchCompleted();

private:
    void updateModel();

private:
    SearchEngine* m_engine = nullptr;
    SearchModel* m_model = nullptr;
    QTimer* m_debounceTimer = nullptr;
    int m_debounceDelay = 300;  // 300ms 防抖延迟
};

#endif // SEARCHVIEWMODEL_H

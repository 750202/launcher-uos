#ifndef SEARCHMODEL_H
#define SEARCHMODEL_H

#include <QAbstractListModel>
#include <QVariantList>

/**
 * @brief 搜索结果模型
 *
 * 为 QML ListView 提供数据模型，支持应用和文件结果
 */
class SearchModel : public QAbstractListModel
{
    Q_OBJECT

    // QML 可访问属性
    Q_PROPERTY(int count READ rowCount NOTIFY countChanged)

public:
    enum Roles {
        ItemIdRole = Qt::UserRole + 1,
        NameRole,
        SubtitleRole,
        IconRole,
        CategoryRole,   // "app" or "file"
        PathRole,
        TypeRole        // 显示用类型标签
    };

    explicit SearchModel(QObject* parent = nullptr);
    ~SearchModel() override = default;

    // QAbstractListModel 接口
    int rowCount(const QModelIndex& parent = QModelIndex()) const override;
    QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const override;
    QHash<int, QByteArray> roleNames() const override;

    // 数据操作
    Q_INVOKABLE void clear();
    Q_INVOKABLE void setAppResults(const QVariantList& apps);
    Q_INVOKABLE void setFileResults(const QVariantList& files);
    Q_INVOKABLE void setResults(const QVariantList& apps, const QVariantList& files);

    // 获取项
    Q_INVOKABLE QVariantMap get(int index) const;

signals:
    void countChanged();

private:
    struct SearchResult {
        QString id;
        QString name;
        QString subtitle;
        QString icon;
        QString category;  // "app" or "file"
        QString path;
    };

    QList<SearchResult> m_results;
};

#endif // SEARCHMODEL_H

#include "SearchModel.h"

SearchModel::SearchModel(QObject* parent)
    : QAbstractListModel(parent)
{
}

int SearchModel::rowCount(const QModelIndex& parent) const
{
    if (parent.isValid()) {
        return 0;
    }
    return m_results.size();
}

QVariant SearchModel::data(const QModelIndex& index, int role) const
{
    if (!index.isValid() || index.row() < 0 || index.row() >= m_results.size()) {
        return QVariant();
    }

    const SearchResult& result = m_results[index.row()];

    switch (role) {
    case ItemIdRole:
        return result.id;
    case NameRole:
        return result.name;
    case SubtitleRole:
        return result.subtitle;
    case IconRole:
        return result.icon;
    case CategoryRole:
        return result.category;
    case PathRole:
        return result.path;
    case TypeRole:
        return result.category == "app" ? QStringLiteral("应用") : QStringLiteral("文件");
    default:
        return QVariant();
    }
}

QHash<int, QByteArray> SearchModel::roleNames() const
{
    QHash<int, QByteArray> roles;
    roles[ItemIdRole] = "itemId";
    roles[NameRole] = "name";
    roles[SubtitleRole] = "subtitle";
    roles[IconRole] = "icon";
    roles[CategoryRole] = "itemCategory";
    roles[PathRole] = "path";
    roles[TypeRole] = "typeLabel";
    return roles;
}

void SearchModel::clear()
{
    if (m_results.isEmpty()) {
        return;
    }

    beginResetModel();
    m_results.clear();
    endResetModel();
    emit countChanged();
}

void SearchModel::setAppResults(const QVariantList& apps)
{
    beginResetModel();
    m_results.clear();

    for (const QVariant& var : apps) {
        QVariantMap map = var.toMap();
        SearchResult result;
        result.id = map["id"].toString();
        result.name = map["name"].toString();
        result.subtitle = map["comment"].toString();
        result.icon = map["iconPath"].toString();
        result.category = "app";
        result.path = map["desktopPath"].toString();
        m_results.append(result);
    }

    endResetModel();
    emit countChanged();
}

void SearchModel::setFileResults(const QVariantList& files)
{
    beginResetModel();
    m_results.clear();

    for (const QVariant& var : files) {
        QVariantMap map = var.toMap();
        SearchResult result;
        result.id = map["id"].toString();
        result.name = map["name"].toString();
        result.subtitle = map["formattedSize"].toString() + " • " + map["formattedModifiedTime"].toString();
        result.icon = "";  // 文件图标由 QML 根据扩展名确定
        result.category = "file";
        result.path = map["path"].toString();
        m_results.append(result);
    }

    endResetModel();
    emit countChanged();
}

void SearchModel::setResults(const QVariantList& apps, const QVariantList& files)
{
    beginResetModel();
    m_results.clear();

    // 先添加应用
    for (const QVariant& var : apps) {
        QVariantMap map = var.toMap();
        SearchResult result;
        result.id = map["id"].toString();
        result.name = map["name"].toString();
        result.subtitle = map["comment"].toString();
        result.icon = map["iconPath"].toString();
        result.category = "app";
        result.path = map["desktopPath"].toString();
        m_results.append(result);
    }

    // 再添加文件
    for (const QVariant& var : files) {
        QVariantMap map = var.toMap();
        SearchResult result;
        result.id = map["id"].toString();
        result.name = map["name"].toString();
        result.subtitle = map["formattedSize"].toString() + " • " + map["formattedModifiedTime"].toString();
        result.icon = "";
        result.category = "file";
        result.path = map["path"].toString();
        m_results.append(result);
    }

    endResetModel();
    emit countChanged();
}

QVariantMap SearchModel::get(int index) const
{
    if (index < 0 || index >= m_results.size()) {
        return QVariantMap();
    }

    const SearchResult& result = m_results[index];
    QVariantMap map;
    map["itemId"] = result.id;
    map["name"] = result.name;
    map["subtitle"] = result.subtitle;
    map["icon"] = result.icon;
    map["itemCategory"] = result.category;
    map["path"] = result.path;
    map["typeLabel"] = result.category == "app" ? QStringLiteral("应用") : QStringLiteral("文件");
    return map;
}

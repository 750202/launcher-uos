#ifndef DATABASE_H
#define DATABASE_H

#include <QString>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QVariant>
#include <QMap>

/**
 * @brief 数据库管理类
 *
 * 负责 SQLite 数据库的初始化、连接管理和基础操作
 */
class Database
{
public:
    // 单例实例
    static Database* instance();

    // 初始化数据库
    bool init();

    // 数据库检查
    bool isOpen() const;
    QSqlQuery query(const QString& sql);

    // 数据库路径
    QString databasePath() const;

private:
    Database();
    ~Database();

    // 禁止拷贝
    Database(const Database&) = delete;
    Database& operator=(const Database&) = delete;

    // 创建表
    bool createTables();

    // 创建 settings 表
    bool createSettingsTable();

    // 创建 favorites 表
    bool createFavoritesTable();

    // 创建 history 表
    bool createHistoryTable();

    // 创建 clipboard 表
    bool createClipboardTable();

private:
    static Database* s_instance;
    QSqlDatabase m_db;
    QString m_dbPath;
};

#endif // DATABASE_H

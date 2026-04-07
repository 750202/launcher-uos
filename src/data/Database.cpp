#include "Database.h"
#include <QDir>
#include <QFileInfo>
#include <QStandardPaths>
#include <QSqlError>
#include <QDebug>

// 静态实例
Database* Database::s_instance = nullptr;

Database* Database::instance()
{
    if (!s_instance) {
        s_instance = new Database();
    }
    return s_instance;
}

Database::Database()
{
    m_dbPath = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation) + "/launcher.db";
}

Database::~Database()
{
    if (m_db.isOpen()) {
        m_db.close();
    }
}

bool Database::init()
{
    // 确保数据库目录存在
    QString dbDir = QFileInfo(m_dbPath).absolutePath();
    QDir().mkpath(dbDir);

    // 检查连接类型
    if (QSqlDatabase::contains("launcher_connection")) {
        m_db = QSqlDatabase::database("launcher_connection");
    } else {
        m_db = QSqlDatabase::addDatabase("QSQLITE", "launcher_connection");
    }

    m_db.setDatabaseName(m_dbPath);

    // 打开数据库
    if (!m_db.open()) {
        qCritical() << "Failed to open database:" << m_db.lastError().text();
        return false;
    }

    // 启用外键约束
    QSqlQuery query(m_db);
    if (!query.exec("PRAGMA foreign_keys = ON")) {
        qWarning() << "Failed to enable foreign keys:" << query.lastError().text();
    }

    // 创建表
    return createTables();
}

bool Database::isOpen() const
{
    return m_db.isOpen();
}

QSqlQuery Database::query(const QString& sql)
{
    return QSqlQuery(sql, m_db);
}

QString Database::databasePath() const
{
    return m_dbPath;
}

bool Database::createTables()
{
    if (!createSettingsTable()) {
        qCritical() << "Failed to create settings table";
        return false;
    }

    if (!createFavoritesTable()) {
        qCritical() << "Failed to create favorites table";
        return false;
    }

    if (!createHistoryTable()) {
        qCritical() << "Failed to create history table";
        return false;
    }

    if (!createClipboardTable()) {
        qCritical() << "Failed to create clipboard table";
        return false;
    }

    return true;
}

bool Database::createSettingsTable()
{
    QSqlQuery query(m_db);
    QString sql =
        "CREATE TABLE IF NOT EXISTS settings ("
        "    key TEXT PRIMARY KEY NOT NULL,"
        "    value TEXT NOT NULL,"
        "    updated_at INTEGER DEFAULT (strftime('%s', 'now'))"
        ");";

    if (!query.exec(sql)) {
        qCritical() << "Failed to create settings table:" << query.lastError().text();
        return false;
    }

    qDebug() << "Settings table created successfully";
    return true;
}

bool Database::createFavoritesTable()
{
    QSqlQuery query(m_db);
    QString sql =
        "CREATE TABLE IF NOT EXISTS favorites ("
        "    id INTEGER PRIMARY KEY AUTOINCREMENT,"
        "    item_id TEXT NOT NULL,"
        "    item_type TEXT NOT NULL,"
        "    name TEXT NOT NULL,"
        "    added_at INTEGER DEFAULT (strftime('%s', 'now'))"
        ");";

    if (!query.exec(sql)) {
        qCritical() << "Failed to create favorites table:" << query.lastError().text();
        return false;
    }

    // 创建索引
    if (!query.exec("CREATE INDEX IF NOT EXISTS idx_favorites_item ON favorites(item_id, item_type);")) {
        qWarning() << "Failed to create favorites index:" << query.lastError().text();
    }

    qDebug() << "Favorites table created successfully";
    return true;
}

bool Database::createHistoryTable()
{
    QSqlQuery query(m_db);
    QString sql =
        "CREATE TABLE IF NOT EXISTS history ("
        "    id INTEGER PRIMARY KEY AUTOINCREMENT,"
        "    item_id TEXT NOT NULL,"
        "    item_type TEXT NOT NULL,"
        "    name TEXT NOT NULL,"
        "    launched_at INTEGER DEFAULT (strftime('%s', 'now'))"
        ");";

    if (!query.exec(sql)) {
        qCritical() << "Failed to create history table:" << query.lastError().text();
        return false;
    }

    // 创建索引
    if (!query.exec("CREATE INDEX IF NOT EXISTS idx_history_item ON history(item_id, item_type);")) {
        qWarning() << "Failed to create history index:" << query.lastError().text();
    }

    if (!query.exec("CREATE INDEX IF NOT EXISTS idx_history_time ON history(launched_at);")) {
        qWarning() << "Failed to create history time index:" << query.lastError().text();
    }

    qDebug() << "History table created successfully";
    return true;
}

bool Database::createClipboardTable()
{
    QSqlQuery query(m_db);
    QString sql =
        "CREATE TABLE IF NOT EXISTS clipboard ("
        "    id INTEGER PRIMARY KEY AUTOINCREMENT,"
        "    content TEXT NOT NULL,"
        "    type TEXT NOT NULL,"
        "    created_at INTEGER DEFAULT (strftime('%s', 'now'))"
        ");";

    if (!query.exec(sql)) {
        qCritical() << "Failed to create clipboard table:" << query.lastError().text();
        return false;
    }

    // 创建索引
    if (!query.exec("CREATE INDEX IF NOT EXISTS idx_clipboard_time ON clipboard(created_at);")) {
        qWarning() << "Failed to create clipboard time index:" << query.lastError().text();
    }

    qDebug() << "Clipboard table created successfully";
    return true;
}

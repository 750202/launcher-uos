#include <QCoreApplication>
#include <QDebug>
#include <QTimer>
#include <QDir>

#include "data/ApplicationRepository.h"
#include "data/FileIndexer.h"
#include "data/FileRepository.h"
#include "data/Database.h"
#include "data/Settings.h"

void testDatabase()
{
    qDebug() << "\n=== 测试数据库 ===";

    Database* db = Database::instance();
    if (db->init()) {
        qDebug() << "✅ 数据库初始化成功";
        qDebug() << "   路径:" << db->databasePath();
        qDebug() << "   状态:" << (db->isOpen() ? "已打开" : "未打开");
    } else {
        qDebug() << "❌ 数据库初始化失败";
    }
}

void testSettings()
{
    qDebug() << "\n=== 测试配置 ===";

    Settings* settings = Settings::instance();

    // 测试读写
    settings->setValue("test_key", "test_value");
    QString value = settings->value("test_key").toString();
    if (value == "test_value") {
        qDebug() << "✅ 配置读写正常";
    } else {
        qDebug() << "❌ 配置读写失败";
    }

    qDebug() << "   主题:" << settings->theme();
    qDebug() << "   历史限制:" << settings->historyLimit();
    qDebug() << "   剪贴板启用:" << settings->clipboardEnabled();
}

void testApplicationRepository()
{
    qDebug() << "\n=== 测试应用仓库 ===";

    ApplicationRepository* repo = ApplicationRepository::instance();

    qDebug() << "正在加载应用...";
    if (repo->loadApplications()) {
        qDebug() << "✅ 应用加载成功";
        qDebug() << "   数量:" << repo->count();

        // 显示前 10 个应用
        qDebug() << "\n   前 10 个应用:";
        QList<Application> apps = repo->allApplications();
        for (int i = 0; i < qMin(10, apps.size()); i++) {
            const Application& app = apps[i];
            qDebug() << "   " << (i + 1) << app.name() << "-" << app.comment();
        }

        // 测试搜索
        qDebug() << "\n   搜索测试 (关键词: '浏览器'):";
        QList<Application> results = repo->searchApplications("浏览器");
        qDebug() << "   找到" << results.size() << "个结果";
        for (int i = 0; i < qMin(3, results.size()); i++) {
            qDebug() << "   " << results[i].name();
        }
    } else {
        qDebug() << "❌ 应用加载失败 (可能是非 Linux 系统)";
    }
}

void testFileIndexer()
{
    qDebug() << "\n=== 测试文件索引器 ===";

    FileIndexer* indexer = FileIndexer::instance();

    // 设置扫描目录
    QStringList dirs;
    dirs << QDir::homePath() + "/Documents";
    dirs << QDir::homePath() + "/Downloads";
    indexer->setScanDirectories(dirs);

    qDebug() << "扫描目录:" << dirs;
    qDebug() << "正在索引文件...";

    indexer->startIndexing();

    qDebug() << "✅ 文件索引完成";
    qDebug() << "   数量:" << indexer->count();

    // 显示前 10 个文件
    qDebug() << "\n   前 10 个文件:";
    QList<FileIndex> files = indexer->allFiles();
    for (int i = 0; i < qMin(10, files.size()); i++) {
        const FileIndex& file = files[i];
        qDebug() << "   " << (i + 1) << file.name() << "-" << file.formattedSize();
    }
}

void testFileRepository()
{
    qDebug() << "\n=== 测试文件仓库 ===";

    FileRepository* repo = FileRepository::instance();

    // 测试最近文件
    qDebug() << "最近文件:";
    QList<FileIndex> recent = repo->recentFiles(5);
    for (int i = 0; i < recent.size(); i++) {
        qDebug() << "   " << (i + 1) << recent[i].name() << "-" << recent[i].formattedModifiedTime();
    }

    // 测试搜索
    qDebug() << "\n   搜索测试 (关键词: '.pdf'):";
    QList<FileIndex> results = repo->searchFiles(".pdf");
    qDebug() << "   找到" << results.size() << "个结果";
    for (int i = 0; i < qMin(3, results.size()); i++) {
        qDebug() << "   " << results[i].name();
    }
}

int main(int argc, char *argv[])
{
    QCoreApplication app(argc, argv);

    qDebug() << "========================================";
    qDebug() << "Launcher 后端核心流程测试";
    qDebug() << "========================================";

    // 运行测试
    testDatabase();
    testSettings();
    testApplicationRepository();
    testFileIndexer();
    testFileRepository();

    qDebug() << "\n========================================";
    qDebug() << "所有测试完成";
    qDebug() << "========================================";

    // 退出应用
    QTimer::singleShot(100, &app, &QCoreApplication::quit);

    return app.exec();
}

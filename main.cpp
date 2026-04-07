#include <QGuiApplication>
#include <QQuickView>
#include <QQmlContext>
#include <QScreen>
#include <QRect>
#include <QColor>
#include <QWindow>
#include <QQmlApplicationEngine>

// 后端组件
#include "src/data/Database.h"
#include "src/data/Settings.h"
#include "src/app/SingleInstance.h"
#include "src/app/HotkeyManager.h"
#include "src/viewmodel/SearchViewModel.h"
#include "src/viewmodel/SearchModel.h"

int main(int argc, char *argv[])
{
    QGuiApplication app(argc, argv);

    // 设置应用信息
    app.setApplicationName("Launcher");
    app.setApplicationDisplayName("Launcher 聚焦搜索");
    app.setOrganizationName("Deepin");
    app.setOrganizationDomain("deepin.org");
    app.setApplicationVersion("1.0.0");

    // 单例检查
    SingleInstance* singleInstance = SingleInstance::instance();
    if (!singleInstance->init()) {
        // 已有实例运行，退出
        return 0;
    }

    // 初始化数据库
    Database* database = Database::instance();
    if (!database->init()) {
        qCritical() << "Failed to initialize database";
        return -1;
    }

    // 加载配置
    Settings* settings = Settings::instance();

    // 初始化快捷键管理器
    HotkeyManager* hotkeyManager = HotkeyManager::instance();
    hotkeyManager->init();

    // 注册 QML 类型
    qmlRegisterType<SearchModel>("Launcher", 1, 0, "SearchModel");

    // 创建 SearchViewModel
    SearchViewModel* viewModel = SearchViewModel::instance();

    // 创建 QQmlApplicationEngine
    QQmlApplicationEngine engine;

    // 暴露 ViewModel 给 QML
    engine.rootContext()->setContextProperty("searchViewModel", viewModel);

    // 加载 QML
    const QUrl url(QStringLiteral("qrc:/qml/Main.qml"));
    QObject::connect(&engine, &QQmlApplicationEngine::objectCreated,
        &app, [url](QObject *obj, const QUrl &objUrl) {
            if (!obj && url == objUrl) QCoreApplication::exit(-1);
        }, Qt::QueuedConnection);

    engine.load(url);

    // 连接单例显示信号
    QObject::connect(singleInstance, &SingleInstance::showWindowRequested, [&]() {
        // 获取主窗口并显示
        QList<QObject*> roots = engine.rootObjects();
        if (!roots.isEmpty()) {
            QWindow* window = qobject_cast<QWindow*>(roots.first());
            if (window) {
                window->show();
                window->raise();
                window->requestActivate();
                qDebug() << "Window activated by another instance";
            }
        }
    });

    // 初始化 ViewModel（延迟加载，不阻塞 UI）
    QTimer::singleShot(100, [viewModel]() {
        viewModel->init();
    });

    return app.exec();
}

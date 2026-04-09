#include "ApplicationRepository.h"
#include <QDir>
#include <QDirIterator>
#include <QFile>
#include <QFileInfo>
#include <QTextStream>
#include <QProcess>
#include <QRegularExpression>
#include <QDebug>
#include <QStandardPaths>

ApplicationRepository* ApplicationRepository::s_instance = nullptr;

ApplicationRepository* ApplicationRepository::instance()
{
    if (!s_instance) {
        s_instance = new ApplicationRepository();
    }
    return s_instance;
}

ApplicationRepository::ApplicationRepository()
{
}

bool ApplicationRepository::loadApplications()
{
    m_applications.clear();
    m_idIndexMap.clear();

    // 扫描系统应用目录
    scanDirectory("/usr/share/applications");

    // 扫描用户应用目录
    QString userAppsDir = QStandardPaths::writableLocation(QStandardPaths::ApplicationsLocation);
    if (!userAppsDir.isEmpty()) {
        scanDirectory(userAppsDir);
    }

    // 扫描本地应用目录
    scanDirectory("/usr/local/share/applications");

    qDebug() << "Loaded" << m_applications.size() << "applications";
    return !m_applications.isEmpty();
}

QList<Application> ApplicationRepository::allApplications() const
{
    return m_applications;
}

QList<Application> ApplicationRepository::searchApplications(const QString& query) const
{
    if (query.isEmpty()) {
        return m_applications;
    }

    QList<Application> results;
    for (const Application& app : m_applications) {
        if (app.matchesQuery(query)) {
            results.append(app);
        }
    }
    return results;
}

Application ApplicationRepository::applicationById(const QString& id) const
{
    if (m_idIndexMap.contains(id)) {
        int index = m_idIndexMap[id];
        if (index >= 0 && index < m_applications.size()) {
            return m_applications[index];
        }
    }
    return Application();
}

bool ApplicationRepository::launchApplication(const QString& id) const
{
    Application app = applicationById(id);
    if (app.isValid()) {
        return launchApplication(app);
    }
    return false;
}

bool ApplicationRepository::launchApplication(const Application& app) const
{
    if (!app.isValid()) {
        return false;
    }

    QString exec = cleanExecCommand(app.exec());
    if (exec.isEmpty()) {
        return false;
    }

    // 使用 QProcess 启动应用
    QProcess process;
    process.setProgram(exec);
#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
    process.startDetached();
#else
    process.startDetached(exec);
#endif

    qDebug() << "Launching application:" << app.name() << "exec:" << exec;
    return true;
}

void ApplicationRepository::refresh()
{
    loadApplications();
}

Application ApplicationRepository::parseDesktopFile(const QString& filePath) const
{
    Application app;
    QFile file(filePath);

    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        return app;
    }

    app.setDesktopPath(filePath);

    // 从文件名生成 ID
    QFileInfo fileInfo(filePath);
    QString id = fileInfo.baseName();
    app.setId(id);

    bool inDesktopEntry = false;
    QString currentLocale = QLocale::system().name(); // 例如 "zh_CN"
    QString shortLocale = currentLocale.split('_').first(); // 例如 "zh"

    QTextStream in(&file);
    while (!in.atEnd()) {
        QString line = in.readLine().trimmed();

        // 跳过空行和注释
        if (line.isEmpty() || line.startsWith('#')) {
            continue;
        }

        // 检查是否进入 [Desktop Entry] 段
        if (line == "[Desktop Entry]") {
            inDesktopEntry = true;
            continue;
        }

        // 检查是否离开 [Desktop Entry] 段
        if (line.startsWith('[') && line != "[Desktop Entry]") {
            inDesktopEntry = false;
            continue;
        }

        if (!inDesktopEntry) {
            continue;
        }

        // 解析键值对
        int eqPos = line.indexOf('=');
        if (eqPos <= 0) {
            continue;
        }

        QString key = line.left(eqPos).trimmed();
        QString value = line.mid(eqPos + 1).trimmed();

        // 处理本地化的键（优先使用本地化版本）
        if (key.startsWith("Name[")) {
            QString locale = key.mid(5, key.length() - 6); // 提取 Name[zh_CN] 中的 zh_CN
            if (locale == currentLocale || locale == shortLocale) {
                app.setName(value);
            }
        } else if (key == "Name" && app.name().isEmpty()) {
            app.setName(value);
        } else if (key.startsWith("GenericName[")) {
            QString locale = key.mid(12, key.length() - 13);
            if (locale == currentLocale || locale == shortLocale) {
                app.setGenericName(value);
            }
        } else if (key == "GenericName" && app.genericName().isEmpty()) {
            app.setGenericName(value);
        } else if (key.startsWith("Comment[")) {
            QString locale = key.mid(8, key.length() - 9);
            if (locale == currentLocale || locale == shortLocale) {
                app.setComment(value);
            }
        } else if (key == "Comment" && app.comment().isEmpty()) {
            app.setComment(value);
        } else if (key == "Icon") {
            app.setIconPath(value);
        } else if (key == "Exec") {
            app.setExec(value);
        } else if (key == "Categories") {
#if QT_VERSION >= QT_VERSION_CHECK(5, 14, 0)
            app.setCategories(value.split(';', Qt::SkipEmptyParts));
#else
            app.setCategories(value.split(';', QString::SkipEmptyParts));
#endif
        } else if (key == "NoDisplay") {
            app.setNoDisplay(value.toLower() == "true");
        } else if (key == "Hidden") {
            app.setHidden(value.toLower() == "true");
        }
    }

    file.close();

    // 检查有效性
    app.setValid(!app.name().isEmpty() && !app.exec().isEmpty());

    // 过滤隐藏的应用
    if (app.isHidden() || app.isNoDisplay()) {
        app.setValid(false);
    }

    return app;
}

void ApplicationRepository::scanDirectory(const QString& directoryPath)
{
    QDir dir(directoryPath);
    if (!dir.exists()) {
        return;
    }

    QStringList filters;
    filters << "*.desktop";
    dir.setNameFilters(filters);

    QFileInfoList fileInfoList = dir.entryInfoList(QDir::Files);
    for (const QFileInfo& fileInfo : fileInfoList) {
        Application app = parseDesktopFile(fileInfo.absoluteFilePath());

        if (app.isValid()) {
            // 检查是否已存在（避免重复）
            if (!m_idIndexMap.contains(app.id())) {
                m_idIndexMap[app.id()] = m_applications.size();
                m_applications.append(app);
            }
        }
    }

    // 递归扫描子目录（某些桌面环境会在子目录中组织应用）
    QDirIterator it(directoryPath, QDir::Dirs | QDir::NoDotAndDotDot);
    while (it.hasNext()) {
        QString subDir = it.next();
        // 跳过一些特殊的目录
        if (!subDir.endsWith("/screensavers") && !subDir.endsWith("/autostart")) {
            scanDirectory(subDir);
        }
    }
}

QString ApplicationRepository::cleanExecCommand(const QString& exec) const
{
    QString result = exec;

    // 移除 %f, %F, %u, %U, %d, %D, %n, %N, %k, %v 等参数
    QRegularExpression re(R"(%[fFuUdDnNickvm])");
    result.remove(re);

    // 移除多余空格
    result = result.trimmed();
    result.replace(QRegularExpression("\\s+"), " ");

    return result;
}

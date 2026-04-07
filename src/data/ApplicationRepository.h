#ifndef APPLICATIONREPOSITORY_H
#define APPLICATIONREPOSITORY_H

#include "Application.h"
#include <QString>
#include <QList>
#include <QStringList>

/**
 * @brief 应用仓库类
 *
 * 负责加载、搜索、启动系统应用程序
 * 从 /usr/share/applications 和 ~/.local/share/applications 加载 .desktop 文件
 */
class ApplicationRepository
{
public:
    // 单例实例
    static ApplicationRepository* instance();

    // 加载应用
    bool loadApplications();

    // 获取所有应用
    QList<Application> allApplications() const;

    // 搜索应用
    QList<Application> searchApplications(const QString& query) const;

    // 根据 ID 获取应用
    Application applicationById(const QString& id) const;

    // 启动应用
    bool launchApplication(const QString& id) const;
    bool launchApplication(const Application& app) const;

    // 获取应用数量
    int count() const { return m_applications.size(); }

    // 刷新应用列表
    void refresh();

private:
    ApplicationRepository();
    ~ApplicationRepository() = default;

    // 禁止拷贝
    ApplicationRepository(const ApplicationRepository&) = delete;
    ApplicationRepository& operator=(const ApplicationRepository&) = delete;

    // 解析 .desktop 文件
    Application parseDesktopFile(const QString& filePath) const;

    // 扫描目录中的应用
    void scanDirectory(const QString& directoryPath);

    // 清理执行命令（移除 %f, %F, %u, %U 等参数）
    QString cleanExecCommand(const QString& exec) const;

private:
    static ApplicationRepository* s_instance;
    QList<Application> m_applications;
    QMap<QString, int> m_idIndexMap;  // ID -> 索引映射，用于快速查找
};

#endif // APPLICATIONREPOSITORY_H

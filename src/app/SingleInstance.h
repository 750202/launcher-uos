#ifndef SINGLEINSTANCE_H
#define SINGLEINSTANCE_H

#include <QObject>
#include <QLocalServer>
#include <QLocalSocket>

/**
 * @brief 单例管理类
 *
 * 确保应用只有一个实例运行，如果有新实例启动则通知已存在的实例
 */
class SingleInstance : public QObject
{
    Q_OBJECT

public:
    // 单例实例
    static SingleInstance* instance();

    // 初始化
    bool init();

    // 检查是否是唯一实例
    bool isUniqueInstance() const { return m_uniqueInstance; }

    // 显示窗口信号
    Q_SIGNAL void showWindowRequested();

private:
    explicit SingleInstance(QObject* parent = nullptr);

    // 禁止拷贝
    SingleInstance(const SingleInstance&) = delete;
    SingleInstance& operator=(const SingleInstance&) = delete;

    // 处理新连接
    void onNewConnection();

    // 通知原有实例
    bool notifyExistingInstance();

private:
    static SingleInstance* s_instance;
    QString m_serverName;
    bool m_uniqueInstance;
    QLocalServer* m_server;
};

#endif // SINGLEINSTANCE_H

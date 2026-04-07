#include "SingleInstance.h"
#include <QStandardPaths>
#include <QCoreApplication>
#include <QCryptographicHash>
#include <QDebug>

// 静态实例
SingleInstance* SingleInstance::s_instance = nullptr;

SingleInstance* SingleInstance::instance()
{
    if (!s_instance) {
        s_instance = new SingleInstance();
    }
    return s_instance;
}

SingleInstance::SingleInstance(QObject* parent)
    : QObject(parent)
    , m_uniqueInstance(false)
    , m_server(nullptr)
{
    // 使用应用信息生成唯一的服务器名
    QString appIdentifier = QCoreApplication::applicationName() + "_" +
                            QCoreApplication::organizationName();

    // 使用 MD5 哈希生成简短的服务器名
    QByteArray hash = QCryptographicHash::hash(appIdentifier.toUtf8(), QCryptographicHash::Md5);
    m_serverName = "launcher_" + hash.toHex();
}

bool SingleInstance::init()
{
    // 尝试通知已存在的实例
    if (notifyExistingInstance()) {
        qDebug() << "Another instance is running, activating it";
        return false;
    }

    // 创建本地服务器
    m_server = new QLocalServer(this);
    m_server->setSocketOptions(QLocalServer::UserAccessOption);

    // 移除可能存在的旧服务器
    QLocalServer::removeServer(m_serverName);

    // 监听
    if (!m_server->listen(m_serverName)) {
        qWarning() << "Failed to listen on local socket:" << m_server->errorString();
        return false;
    }

    // 连接新连接信号
    connect(m_server, &QLocalServer::newConnection, this, &SingleInstance::onNewConnection);

    m_uniqueInstance = true;
    qDebug() << "Single instance mode activated, server name:" << m_serverName;
    return true;
}

bool SingleInstance::notifyExistingInstance()
{
    QLocalSocket socket;
    socket.connectToServer(m_serverName);

    // 检查是否连接成功
    if (socket.waitForConnected(300)) {
        // 发送激活消息
        socket.write("show");
        socket.flush();
        socket.waitForBytesWritten(300);
        socket.disconnectFromServer();
        return true;
    }

    return false;
}

void SingleInstance::onNewConnection()
{
    QLocalSocket* client = m_server->nextPendingConnection();
    if (!client) {
        return;
    }

    connect(client, &QLocalSocket::readyRead, [this, client]() {
        QByteArray data = client->readAll();
        if (data == "show") {
            qDebug() << "Received show request from another instance";
            Q_EMIT showWindowRequested();
        }
    });

    connect(client, &QLocalSocket::disconnected, client, &QLocalSocket::deleteLater);
}

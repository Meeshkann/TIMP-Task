#include <QDebug>
#include <QCoreApplication>
#include <QString>

#include "mytcpserver.h"
#include "mydb.h"
#include "myFunction.h"

myFunc solver;

MyTcpServer::~MyTcpServer()
{

    stop();
    if (db) {
        db->disconnect();
        delete db;
        db = nullptr;
    }
}

MyTcpServer& MyTcpServer::getInstance()
{
    static MyTcpServer instance;
    return instance;
}

MyTcpServer::MyTcpServer(QObject *parent) : QObject(parent)
    , pTcpServer(nullptr)
    , pTcpSocket(nullptr)
    , db(nullptr)
    , port(0)
    , isRunFlag(false)
{
    db = new MyDBHandler(this);
    if (!db || !(*db)) {
       qWarning() << "DB connection is not available, auth operations will fail";
    }

    pTcpServer = new QTcpServer(this);

    connect(pTcpServer, &QTcpServer::newConnection, this, &MyTcpServer::slotNewConnection);
}

bool MyTcpServer::start(quint16 port)
{
    if (isRunFlag) {
        qDebug() << "Server already running on port" << this->port;
        return true;
    }

    this->port = port;

    if (!pTcpServer->listen(QHostAddress::Any, port)) {
        qDebug() << "Failed to start server on port" << port
                 << "Error:" << pTcpServer->errorString();
        return false;
    }

    isRunFlag = true;
    qDebug() << "Server started successfully on port" << port;
    return true;
}

void MyTcpServer::stop()
{
    if (!isRunFlag) return;

    // закрываем все клиентские соединения
    for (QTcpSocket* socket : socketBuffers.keys()) {
        socket->close();
        socket->deleteLater();
    }
    socketBuffers.clear();

    // закрытие сервера
    if (pTcpServer) {
        pTcpServer->close();
    }

    isRunFlag = false;
    qDebug() << "Server stopped";
}


bool MyTcpServer::isRunning() const
{
    return isRunFlag && pTcpServer && pTcpServer->isListening();
}

quint16 MyTcpServer::getPort() const
{
    return port;
}

// ================= слоты

void MyTcpServer::slotNewConnection(){
    QTcpSocket *socket = pTcpServer->nextPendingConnection();
    if (!socket) {
        return;
    }
    socketBuffers.insert(socket, "");
    socket->write("I am your server! Please login!!!\r\n");

    connect(socket,&QTcpSocket::readyRead,this, &MyTcpServer::slotServerRead);
    connect(socket,&QTcpSocket::disconnected,this,&MyTcpServer::slotClientDisconnected);

    qDebug() << "New client connected"; // можно добавить информацию о клиенте
}


void  MyTcpServer::slotClientDisconnected()
{
    QTcpSocket *socket = qobject_cast<QTcpSocket*>(sender());
    if (socket)
    {
        socketBuffers.remove(socket);
        socket->close();
        socket->deleteLater();
        qDebug() << "Client disconnected";
    }
}

void MyTcpServer::slotServerRead()
{
    QTcpSocket *socket = qobject_cast<QTcpSocket*>(sender());
    if (!socket) {
        return;
    }


    QString &res = socketBuffers[socket];

    if(res.size()> 65536)
    {
        socket->write("Error: message to long");
        res.clear();
        return;
    }


    while(socket->bytesAvailable() > 0)
    {
        QByteArray array = socket->readAll();
        qDebug() << "Received:" << array;

        res.append(QString::fromUtf8(array));
    }

    if (res.isEmpty()) return;

    if (res[res.size() - 1] != '\n') {
        return;
    }

    qDebug() << "Full buffer:" << res;

    CommandParser parser;
    CommandParser::ParsedCommand cmd = parser.parse(res.trimmed());

    qDebug() << "Parse result - command:" << cmd.command
             << "is_valid:" << cmd.is_valid
             << "error:" << cmd.error
             << "params:" << cmd.params;

    res = "";

    if (!cmd.is_valid)
    {
        socket->write(("ERROR: " + cmd.error + "\r\n").toUtf8());
        return;
    }

    switch(cmd.command)
    {
    case CommandParser::CMD_REGISTER:
        if (cmd.params.size() == 3 && db && db->regUser(cmd.params[0], cmd.params[1], cmd.params[2]))
        {
            socket->write("SUCCESS: User registered\r\n");
            qDebug() << "user registrated";
            res.clear();
        }
        else
        {
            socket->write("ERROR: Registration failed\r\n");
        }
        break;

    case CommandParser::CMD_LOGIN:
    case CommandParser::CMD_AUTH:
        if (cmd.params.size() == 2 && db && db->authUser(cmd.params[0], cmd.params[1]))
        {
            socket->write("SUCCESS: Login successful\r\n");
            qDebug() << "user logged";
            res.clear();
        }
        else
        {
            socket->write("ERROR: Invalid credentials\r\n");
        }
        break;

    case CommandParser::CMD_FORGOT_PASSWORD:
        if (cmd.params.size() == 2 && db && db->resetPasswordByEmail(cmd.params[0], cmd.params[1]))
        {
            socket->write("SUCCESS: Password reset by email\r\n");
            res.clear();
        }
        else
        {
            socket->write("ERROR: Password reset failed\r\n");
        }
        break;

    case CommandParser::CMD_HELP:
        socket->write(parser.getHelp().toUtf8());
        res.clear();
        break;

    case CommandParser::CMD_CALCULATE:
    {
        int a = cmd.params[0].toInt();
        int b = cmd.params[1].toInt();
        int c = cmd.params[2].toInt();
        QString data = solver.generateFunctionData(a, b, c);
        socket->write(("SUCCESS:" + data + "\r\n").toUtf8());
        break;
    }

    default:
        socket->write("ERROR: Unknown command\r\n");
        break;
    }
}












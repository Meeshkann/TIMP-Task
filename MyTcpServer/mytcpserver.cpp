#include <QDebug>
#include <QCoreApplication>
#include <QString>

#include "mytcpserver.h"
#include "mydb.h"

MyTcpServer::~MyTcpServer()
{
    if (db) {
        db->disconnect();
        delete db;
        db = nullptr;
    }
}

MyTcpServer::MyTcpServer(QObject *parent) : QObject(parent)
{
    db = new MyDBHandler(this);
    if (!db || !(*db)) {
       qWarning() << "DB connection is not available, auth operations will fail";
    }

    pTcpServer = new QTcpServer(this);
    port = 54678;

    connect(pTcpServer, &QTcpServer::newConnection, this, &MyTcpServer::slotNewConnection);

    if (!pTcpServer->listen(QHostAddress::Any, port))
    {
        qDebug() << "server is not started";
    }
    else
    {
        qDebug() << "server is started on port" << port;

    }

}

void  MyTcpServer::slotClientDisconnected()
{
    QTcpSocket *socket = qobject_cast<QTcpSocket*>(sender());
    if (socket)
    {
        socketBuffers.remove(socket);
        socket->close();
        socket->deleteLater();
    }
}

void MyTcpServer::slotServerRead()
{
    QTcpSocket *socket = qobject_cast<QTcpSocket*>(sender());
    if (!socket) {
        return;
    }


    QString &res = socketBuffers[socket];

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

    default:
        socket->write("ERROR: Unknown command\r\n");
        break;
    }



}

void MyTcpServer::slotNewConnection(){
    QTcpSocket *socket = pTcpServer->nextPendingConnection();
    if (!socket) {
        return;
    }
    socketBuffers.insert(socket, "");
    socket->write("I am auth server!\r\n");

    connect(socket,&QTcpSocket::readyRead,this, &MyTcpServer::slotServerRead);
    connect(socket,&QTcpSocket::disconnected,this,&MyTcpServer::slotClientDisconnected);

    qDebug() << "New client connected";
}















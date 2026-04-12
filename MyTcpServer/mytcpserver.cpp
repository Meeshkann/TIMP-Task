#include <QDebug>
#include "mytcpserver.h"
#include <QCoreApplication>
#include <QString>

MyTcpServer::~MyTcpServer()
{
    // delete db;
}

MyTcpServer::MyTcpServer(QObject *parent) : QObject(parent)
{
    // db = new PostGreDB(); // вспомнить потом
    // db->connect();
    db = nullptr;

    pTcpServer = new QTcpServer(this);
    port = 54678;

    connect(pTcpServer, &QTcpServer::newConnection, this, &MyTcpServer::slotNewConnection);

    if (!pTcpServer->listen(QHostAddress::Any, port))
    {
        qDebug() << "server is not started";
    }
    else
    {
        qDebug() << "server is started at port" << port;
    }

}

void  MyTcpServer::slotClientDisconnected()
{
    if (pTcpSocket)
    {
    pTcpSocket->close();
    pTcpSocket=nullptr;
    }
}

void MyTcpServer::slotServerRead()
{
    static QString res = "";

    while(pTcpSocket->bytesAvailable() > 0)
    {
        QByteArray array = pTcpSocket->readAll();
        qDebug() << "Received:" << array;

        res.append(QString::fromUtf8(array));
    }

    if (res.isEmpty()) return;



    qDebug() << "Full buffer:" << res;

    CommandParser parser;
    CommandParser::ParsedCommand cmd = parser.parse(res.trimmed());

    qDebug() << "Parse result - command:" << cmd.command
             << "is_valid:" << cmd.is_valid
             << "error:" << cmd.error
             << "params:" << cmd.params;

    if (!cmd.is_valid)
    {
        pTcpSocket->write(("ERROR: " + cmd.error + "\n").toUtf8());
        return;
    }

    switch(cmd.command)
    {
    case CommandParser::CMD_REGISTER:
        if (cmd.params.size() >= 2 && db && db->regUser(cmd.params[0], cmd.params[1]))
        {
            pTcpSocket->write("SUCCESS: User registered\n");
            res.clear();
        }
        else
        {
            pTcpSocket->write("ERROR: Registration failed\n");
        }
        break;

    case CommandParser::CMD_LOGIN:
    case CommandParser::CMD_AUTH:
        if (cmd.params.size() >= 2 && db && db->authUser(cmd.params[0], cmd.params[1]))
        {
            pTcpSocket->write("SUCCESS: Login successful\n");
            res.clear();
        }
        else
        {
            pTcpSocket->write("ERROR: Invalid credentials\n");
        }
        break;

    case CommandParser::CMD_HELP:
        pTcpSocket->write(parser.getHelp().toUtf8());
        res.clear();
        break;

    default:
        pTcpSocket->write("ERROR: Unknown command\n");
        break;
    }
}

void MyTcpServer::slotNewConnection(){
    //   if(server_status==1)
    pTcpSocket = pTcpServer->nextPendingConnection();
    pTcpSocket->write("I am echo server!\r\n");

    connect(pTcpSocket,&QTcpSocket::readyRead,this, &MyTcpServer::slotServerRead);
    connect(pTcpSocket,&QTcpSocket::disconnected,this,&MyTcpServer::slotClientDisconnected);

    qDebug() << "New client connected";
}















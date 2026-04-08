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

    pTcpServer = new QTcpServer(this);
    port = 54678;

    connect(pTcpServer, &QTcpServer::newConnection, this, &MyTcpServer::slotNewConnection);

    if (!pTcpServer->listen(QHostAddress::Any, port))
    {
        qDebug() << "server is not started";
    }
    else
    {
        qDebug() << "server is started";
    }

}

void  MyTcpServer::slotClientDisconnected()
{
    pTcpSocket->close();
}

void MyTcpServer::slotServerRead()
{
    QString res = "";
    while(pTcpSocket->bytesAvailable()>0)
    {
        QByteArray array = pTcpSocket->readAll();

        qDebug() << array << "\n";

        if(array=="\x01")
        {
            pTcpSocket->write(res.toUtf8());
            res = "";
        }
        else
            res.append(array);
    }
    pTcpSocket->write(res.toUtf8());
}

void MyTcpServer::slotNewConnection(){
    //   if(server_status==1){
    pTcpSocket = pTcpServer->nextPendingConnection();
    pTcpSocket->write("Hello, World!!! I am echo server!\r\n");

    connect(pTcpSocket,&QTcpSocket::readyRead,this, &MyTcpServer::slotServerRead);
    connect(pTcpSocket,&QTcpSocket::disconnected,this,&MyTcpServer::slotClientDisconnected);
    // }
}















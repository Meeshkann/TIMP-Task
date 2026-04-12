#ifndef MYTCPSERVER_H
#define MYTCPSERVER_H

#include <QObject>
#include <QTcpServer>
#include <QTcpSocket>
#include <QHash>
#include <mydb.h>
#include <message_parser.h>

class MyTcpServer : public QObject
{
    Q_OBJECT
public:
    explicit MyTcpServer(QObject *parent = nullptr);
    ~MyTcpServer();
public slots:
    void slotNewConnection();
    void slotClientDisconnected();
    void slotServerRead();
private:
    QTcpServer * pTcpServer;
    QTcpSocket * pTcpSocket;
    QHash<QTcpSocket*, QString> socketBuffers;
    MyDBHandler * db;
    quint16 port;
};

#endif // MYTCPSERVER_H

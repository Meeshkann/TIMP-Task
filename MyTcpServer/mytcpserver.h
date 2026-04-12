#ifndef MYTCPSERVER_H
#define MYTCPSERVER_H

#include <QObject>
#include <QTcpServer>
#include <QTcpSocket>
#include <QHash>
#include <dbinterface.h>
#include <postgre_db.h>
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
    dbInterface * db;
    QHash<QTcpSocket*, QString> socketBuffers;

    quint16 port;
};

#endif // MYTCPSERVER_H

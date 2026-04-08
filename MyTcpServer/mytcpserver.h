#ifndef MYTCPSERVER_H
#define MYTCPSERVER_H

#include <QObject>
#include <QTcpServer>
#include <QTcpSocket>
#include <dbinterface.h>
#include <postgre_db.h>

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

    quint16 port;
    //  еще поле
};

#endif // MYTCPSERVER_H

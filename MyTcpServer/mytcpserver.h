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
    static MyTcpServer& getInstance();
    ~MyTcpServer();

    bool start(quint16 port = 54678);
    void stop();
    bool isRunning() const;
    quint16 getPort() const;


public slots:
    void slotNewConnection();
    void slotClientDisconnected();
    void slotServerRead();
private:
    explicit MyTcpServer(QObject *parent = nullptr);
    MyTcpServer(const MyTcpServer&) = delete;
    MyTcpServer& operator=(const MyTcpServer&) = delete;



    QTcpServer * pTcpServer;
    QTcpSocket * pTcpSocket;
    QHash<QTcpSocket*, QString> socketBuffers;
    MyDBHandler * db;
    quint16 port;
    bool isRunFlag;
};

#endif // MYTCPSERVER_H

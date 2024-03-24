#ifndef MYTCPSERVER_H
#define MYTCPSERVER_H
#include <QTcpServer>
#include <QList>
#include "mytcpsocket.h"

class MyTcpServer : public QTcpServer
{
    Q_OBJECT
public:
    MyTcpServer();
    static MyTcpServer &getInstance();
    void incomingConnection(qintptr socketDescriptor);//
    void resend(const char* pername,PDU* pdu);//转发函数
    
public slots:
    void deleteSocket(MyTcpSocket* mysocket);//
    
private:
    QList<MyTcpSocket*> m_tcpSocketList;//链表m_tcpSocketList，类型为MyTcpSocket*
};

#endif // MYTCPSERVER_H

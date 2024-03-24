#include "mytcpserver.h"
#include <QDebug>
MyTcpServer::MyTcpServer()
{
    
}

MyTcpServer &MyTcpServer::getInstance()
{
    static MyTcpServer instance;
    return instance;
}

void MyTcpServer::incomingConnection(qintptr socketDescriptor)//接收客户端的时候，会进入该函数
{
   qDebug()<< "new client connect";
   MyTcpSocket* pTcpSocket = new MyTcpSocket;//产生一个指针，用指针指向new出来的一个对象
   pTcpSocket->setSocketDescriptor(socketDescriptor);
   m_tcpSocketList.append(pTcpSocket);//将套接字加入链表
   
   connect(pTcpSocket,SIGNAL(offline(MyTcpSocket*)),this,SLOT(deleteSocket(MyTcpSocket*)));
}

void MyTcpServer::resend(const char *pername, PDU *pdu)//加好友时对客户端发送过来的数据进行转发
{
    if(pername == NULL || pdu == NULL){
        return ;
    }
    QString strName = pername;
    for(int i=0;i<m_tcpSocketList.size();i++){
        if(strName == m_tcpSocketList.at(i)->getName()){
            m_tcpSocketList.at(i)->write((char*)pdu,pdu->uiPDULen);
            break;
        }
    }
}

void MyTcpServer::deleteSocket(MyTcpSocket *mysocket)//下线时删除链表中的套接字socket
{
    QList<MyTcpSocket*>::iterator iter = m_tcpSocketList.begin();
    for(;iter!=m_tcpSocketList.end();iter++){
        if(mysocket == *iter){
           // delete*iter;//先删除指针指向的对象
            //*iter=NULL;
            m_tcpSocketList.erase(iter);//删除指针
            break;
        }
    }
    for(int i=0;i<m_tcpSocketList.size();i++){
        qDebug()<<m_tcpSocketList.at(i)->getName();//先获得对应的指针，通过这个指针，调用getName函数
    }
}

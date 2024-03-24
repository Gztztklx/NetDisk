#ifndef MYTCPSOCKET_H
#define MYTCPSOCKET_H
#include <QObject>
#include <QSqlDatabase>//连接数据库
#include <QSqlQuery>//查询数据库
#include <QTcpSocket>//TCP网络编程
#include <QDebug>
#include <QStringList>//字符串列表
#include <QDir>
#include <QFile>
#include <QTimer>
#include "protocol.h"
#include "opedb.h"

class MyTcpSocket : public QTcpSocket
{
    Q_OBJECT
public:
    MyTcpSocket();
    QString getName();
    void copyDir(QString strSrcDir,QString strDestDir);
    
signals:
    void offline(MyTcpSocket* mysocket);
    
    
public slots:
    void recvMsg();//接收发送过来的数据
    void clientOffline();//用来发送客户端下线的信号
    void sendFileToClient();//发送文件给客户端
    
private:
    QString m_strName;
    QFile m_file;
    qint64 m_iTotal;//文件总的大小
    qint64 m_iRecved;//文件已经接受了多少
    bool m_bUpLoad;//判断是否处于上传文件的状态
    QTimer *m_pTimer;
};

#endif // MYTCPSOCKET_H

#ifndef TCPCLIENT_H
#define TCPCLIENT_H

#include <QWidget>
#include <QFile>
#include <QTcpSocket>
#include "protocol.h"
#include "opewidget.h"
#include "book.h"

namespace Ui {
class TcpClient;
}

class TcpClient : public QWidget
{
    Q_OBJECT
    
public:
    explicit TcpClient(QWidget *parent = 0);
    ~TcpClient();
    void loadConfig();
    
    static TcpClient& getInstance();
    QTcpSocket& getTcpSocket();
    QString& getname();
    QString& getCurPath();
    void setM_strCurPath(QString strCurPath);
    
public slots:
    void showConnect();
    void recvMsg();
    
private slots:
//    void on_send_pb_clicked();//之前进行的测试
    
    void on_login_pb_clicked();
    
    void on_regist_pb_clicked();
    
    void on_cancel_pb_clicked();
    
private:
    Ui::TcpClient *ui;
    QString m_strIP;
    quint16 m_usPort;
    QString m_strLoginName;//登录时自己的用户名
    QString m_strCurPath;//当前路径
    
    QTcpSocket m_tcpSocket;//通过QTcpSocket对象连接服务器，和服务器进行数据交互
    QFile m_file;
};

#endif // TCPCLIENT_H

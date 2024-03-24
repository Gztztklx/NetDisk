#include "tcpserver.h"
#include "ui_tcpserver.h"
#include "mytcpserver.h"
#include <QFile>
#include <QMessageBox>
#include <QByteArray>

TcpServer::TcpServer(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::TcpServer)
{
    ui->setupUi(this);
    loadConfig();
    MyTcpServer::getInstance().listen(QHostAddress(m_strIP),m_usPort);
}

TcpServer::~TcpServer()
{
    delete ui;
}
void TcpServer::loadConfig()
{
    QFile file(":/server.txt");
    if(file.open(QIODevice::ReadOnly))//打开文件
    {
        QByteArray baData=file.readAll();//读取数据
        QString strData = baData.toStdString().c_str();//转换成字符串
        file.close();
        strData.replace("\r\n"," ");//用空格替换\r\n
        QStringList strList=strData.split(" ");//用空格进行切分
        m_strIP=strList.at(0);//ip
        m_usPort=strList.at(1).toUShort();//端口号
        qDebug()<<"ip"<<m_strIP<<"port"<<m_usPort;//转换成数字
    }else{
        QMessageBox::critical(this,"open config","open config failed");
    }
}

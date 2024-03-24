#include "tcpclient.h"
#include "ui_tcpclient.h"
#include <QByteArray>
#include <QDebug>
#include <QMessageBox>
#include <QHostAddress>
#include "privatrchat.h"

TcpClient::TcpClient(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::TcpClient)
{
    ui->setupUi(this);
    resize(300,150);
    loadConfig();
    
    //关联信号槽
    connect(&m_tcpSocket,SIGNAL(connected()),this,SLOT(showConnect()));
    connect(&m_tcpSocket,SIGNAL(readyRead()),this,SLOT(recvMsg()));
    
    
    m_tcpSocket.connectToHost(QHostAddress(m_strIP),m_usPort);//连接服务器
}

TcpClient::~TcpClient()
{
    delete ui;
}
void TcpClient::loadConfig()
{
    QFile file(":/client.txt");
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

TcpClient &TcpClient::getInstance()
{
    static TcpClient instance;
    return instance;
}

QTcpSocket &TcpClient::getTcpSocket()
{
    return m_tcpSocket;
}

QString &TcpClient::getname()
{
    return m_strLoginName;
}

QString &TcpClient::getCurPath()
{
    return m_strCurPath;
}

void TcpClient::setM_strCurPath(QString strCurPath)
{
    m_strCurPath.clear();
    m_strCurPath = strCurPath;
}
void TcpClient::showConnect()
{
    QMessageBox::information(this,"连接服务器","连接服务器成功");
}

void TcpClient::recvMsg()
{
    if(OpeWidget::getInstance().getBook()->getDownFlag()){//如果为真，就直接接收数据，假就跳过
        qDebug()<<"开始接收数据";
        QByteArray buffer = m_tcpSocket.readAll();
        qDebug()<<"buffer的大小："<<buffer;
        int n = m_file.write(buffer);
        qDebug()<<n;
        qDebug()<<"已经往文件里写了";
        Book* pBook = OpeWidget::getInstance().getBook();
        pBook->m_iRecved += buffer.size();
        if(pBook->m_iTotal <= pBook->m_iRecved){
            qDebug()<<"数据接收完毕";
            m_file.close();
            pBook->m_iRecved = 0;
            pBook->m_iTotal = 0;
            pBook->setDownFlag(false);
            QMessageBox::information(this,"下载文件","下载文件成功");
        }
        
        
        /*
        QByteArray buff = m_tcpSocket.readAll();
        m_file.write(buff);
        Book* pBook = OpeWidget::getInstance().getBook();
        pBook->m_iRecved += buff.size();
        qDebug()<<"正在接收文件";
        if(pBook->m_iTotal <= pBook->m_iRecved){
            m_file.close();
            pBook->setDownFlag(false);
            pBook->m_iRecved = 0;
            pBook->m_iTotal = 0;
            qDebug()<<"文件接收完毕";
        }
          */    
    }
    
    qDebug()<<m_tcpSocket.bytesAvailable();//获取发送来的数据的大小，有可能会出错
    uint uiPDULen = 0;
    m_tcpSocket.read((char*)&uiPDULen,sizeof(uint));//先收取四个字节，获取此次发送来的数据总的大小
    uint uiMsgLen = uiPDULen-sizeof(PDU);//实际消息的长度=总的大小-结构体的大小
    PDU *pdu = mkPDU(uiMsgLen);
    m_tcpSocket.read((char*)pdu+sizeof(uint),uiPDULen-sizeof(uint));
    switch(pdu->uiMsgType){
    case ENUM_MSG_TYPE_REGIST_RESPOND://如果是注册回复
    {
        if (strcmp(pdu->caData,REGIST_OK)==0){
            QMessageBox::information(this,"注册",REGIST_OK);
        }else if(strcmp(pdu->caData,REGIST_FAILED)==0){
            QMessageBox::warning(this,"注册",REGIST_FAILED);
        }else{
           
        }
        break;
    }
    case ENUM_MSG_TYPE_LOGIN_RESPOND://如果是登录回复
    {
        if (strcmp(pdu->caData,LOGIN_OK)==0){
            m_strCurPath = QString("./%1").arg(m_strLoginName);//当前路径
            QMessageBox::information(this,"登录",LOGIN_OK);
            OpeWidget::getInstance().show();//显示操作面板
            this->hide();//登陆的界面隐藏掉
        }else if(strcmp(pdu->caData,LOGIN_FAILED)==0){
            m_strLoginName ='\0';
            QMessageBox::critical(this,"登录",LOGIN_FAILED);
        }else{
            
        }
        break;
    }
    case ENUM_MSG_TYPE_ALL_ONLINE_RESPOND://如果是所有在线用户回复
    {
        OpeWidget::getInstance().getFriend()->showALLOnlineUsr(pdu);
        break;
    }
    case ENUM_MSG_TYPE_SEARCH_USR_RESPOND://如果是搜索指定用户的回复
    {
        if(strcmp(pdu->caData,SEARCH_USR_NO)==0){
            QMessageBox::warning(this,"搜索指定用户",QString("%1:该用户不存在！").arg(OpeWidget::getInstance().getFriend()->m_strSearchName));
        }else if(strcmp(pdu->caData,SEARCH_USR_ONLINE)==0){
            QMessageBox::information(this,"搜索指定用户",QString("%1:该用户已上线！").arg(OpeWidget::getInstance().getFriend()->m_strSearchName));
        }else if(strcmp(pdu->caData,SEARCH_USR_OFFLINE)==0){
            QMessageBox::critical(this,"搜索指定用户",QString("%1:该用户已离开！").arg(OpeWidget::getInstance().getFriend()->m_strSearchName));
        }
    }
    case ENUM_MSG_TYPE_ADD_FRIEND_REQUEST://这是由服务器转发的其他客户端发送的添加好友的请求
    {
        char caName[32] = {'\0'};//请求添加您为好友的人的用户名
        char caMySelf[32] = {'\0'};
        strncpy(caName,pdu->caData+32,32);
        strncpy(caMySelf,pdu->caData,32);
        int ret = QMessageBox::information(this,"添加好友",QString("%1请求添加您为好友").arg(caName),QMessageBox::Yes,QMessageBox::No);
        PDU *respdu=mkPDU(0);
        memcpy(respdu->caData,pdu->caData,32);
        memcpy(respdu->caData+32,caMySelf,32);
        if(QMessageBox::Yes == ret){
            respdu->uiMsgType=ENUM_MSG_TYPE_ADD_FRIEND_AGGREE;//同意添加好友
        }else{
            respdu->uiMsgType=ENUM_MSG_TYPE_ADD_FRIEND_REFUSE;//拒绝好友请求
        }
        m_tcpSocket.write((char*)respdu,respdu->uiPDULen);
        free(respdu);
        pdu=NULL;
        break;
    }
    case ENUM_MSG_TYPE_ADD_FRIEND_RESPOND://这是添加好友失败由服务器发送回来的报告
    {
        QMessageBox::critical(this,"添加失败",pdu->caData);
        break;
    }
    case ENUM_MSG_TYPE_ADD_FRIEND_AGGREE://同意好友添加请求
    {
        char caPerName[32] = {'\0'};
        memcpy(caPerName,pdu->caData,32);
        QMessageBox::information(this,"添加好友",QString("成功添加%1为好友").arg(caPerName));
        break;
    }
    case ENUM_MSG_TYPE_ADD_FRIEND_REFUSE://拒绝好友添加请求
    {
        char caPerName[32] = {'\0'};
        memcpy(caPerName,pdu->caData,32);
        QMessageBox::information(this,"添加好友",QString("%1拒绝添加您为好友").arg(caPerName));
        break;
    }
    case ENUM_MSG_TYPE_FLUSH_FRIEND_RESPOND://刷新好友的回复
    {
        OpeWidget::getInstance().getFriend()->updateFriendList(pdu);
        break;
    }
    case ENUM_MSG_TYPE_DELETE_FRIEND_REQUEST://经服务器转发的其他客户端的删除好友通知
    {
        char caName[32] = {'\0'};
        memcpy(caName,pdu->caData,32);
        QMessageBox::information(this,"删除好友",QString("%1与您的好友关系已断开").arg(caName));
        break;
    }
    case ENUM_MSG_TYPE_DELETE_FRIEND_RESPOND://关于自己删除好友的操作的通知
    {
        QMessageBox::information(this,"删除好友","删除好友成功");
        break;
    }
    case ENUM_MSG_TYPE_PRIVATR_CHAT_REQUEST://客户端收到了服务器转发的其他客户端发送的私聊请求
    {
        if(Privatrchat::getInstance().isHidden()){
            Privatrchat::getInstance().show();
        }
        char caSendName[32] = {"'\0"};
        memcpy(caSendName,pdu->caData,32);
        QString strSendName = caSendName;
        Privatrchat::getInstance().setChatName(strSendName);
        Privatrchat::getInstance().updateMsg(pdu);
        
     
        break;  
    }
        
    case ENUM_MSG_TYPE_GROUND_CHAT_REQUEST://客户端收到了服务器转发的其他客户端发送的群聊请求
    {
        OpeWidget::getInstance().getFriend()->updateGroundMsg(pdu);
        break;
    }
    case ENUM_MSG_TYPE_CREATE_DIR_RESPOND://客户端接收到了创建文件夹的回复
    {
        QMessageBox::information(this,"创建文件夹",pdu->caData);
        break;
    }
    case ENUM_MSG_TYPE_FLUSH_FILE_RESPOND://客户端接收到了刷新文件的回复
    {
        OpeWidget::getInstance().getBook()->updateFileList(pdu);
        QString strCurPath = OpeWidget::getInstance().getBook()->getEntDir();
        if(!strCurPath.isEmpty()){
            m_strCurPath.clear();
            m_strCurPath = strCurPath;
        }
        break;
    }
    case ENUM_MSG_TYPE_DEL_DIR_RESPOND://收到了删除文件的回复
    {
        QMessageBox::information(this,"删除文件夹",pdu->caData);
        break;
    }
    case ENUM_MSG_TYPE_RENAME_RESPOND:
    {
        QMessageBox::information(this,"重命名文件",pdu->caData);
        break;
    }
    case ENUM_MSG_TYPE_ENTER_FILE_RESPOND://收到了进入文件的回复
    {
        
        OpeWidget::getInstance().getBook()->clearEnterDir();
        QMessageBox::information(this,"进入文件夹",pdu->caData);
        break;
    }
    case ENUM_MSG_TYPE_UPLOAD_FILE_RESPOND://收到了上传文件的回复
    {
        QMessageBox::information(this,"上传文件",pdu->caData);
        break;
    }
    case ENUM_MSG_TYPE_DOWN_LOAD_RESPOND://收到了下载文件的回复
    {
        qDebug()<<pdu->caData;
        char caFileName[32] = {'\0'};
        sscanf(pdu->caData,"%s %11d",caFileName,&(OpeWidget::getInstance().getBook()->m_iTotal));
        if(strlen(caFileName)>0 && OpeWidget::getInstance().getBook()->m_iTotal>0){
            OpeWidget::getInstance().getBook()->setDownFlag(true);//处于下载状态
            m_file.setFileName(OpeWidget::getInstance().getBook()->getSaveLoadPath());
            qDebug()<<OpeWidget::getInstance().getBook()->getSaveLoadPath();
            if(!m_file.open(QIODevice::WriteOnly)){
                QMessageBox::warning(this,"下载文件","获得保存文件路径失败");
            }
        }
        break;
    }
    case ENUM_MSG_TYPE_SHARE_FILE_RESPOND://分享文件的回复
    {
        QMessageBox::information(this,"共享文件",pdu->caData);
        break;
    }
    case ENUM_MSG_TYPE_SHARE_FILE_NOTE://有人要分享文件给你的通知
    {
        char *pPath = new char[pdu->uiMsgLen];
        memcpy(pPath,pdu->caMsg,pdu->uiMsgLen);
        char *pos = strrchr(pPath,'/');//从后面开始寻找 ‘/’
        if(NULL != pos){
            pos++;
            QString strNote = QString("%1 share file-->%2,\nWill you accept it?").arg(pdu->caData).arg(pos);
            int ret = QMessageBox::question(this,"共享文件",strNote);
            if(QMessageBox::Yes == ret){
                PDU* respdu = mkPDU(pdu->uiMsgLen);
                respdu->uiMsgType = ENUM_MSG_TYPE_SHARE_FILE_NOTE_RESPOND;
                memcpy(respdu->caMsg,pdu->caMsg,pdu->uiMsgLen);
                QString strName = TcpClient::getInstance().getname();
                strcpy(respdu->caData,strName.toStdString().c_str());
                m_tcpSocket.write((char*)respdu,respdu->uiPDULen);
                free(respdu);
                respdu=NULL;
            }
        }
        delete []pPath;
        pPath=NULL;
        break;
    }
    case ENUM_MSG_TYPE_MOVE_FILE_RESPOND://移动文件的回复
    {
        QMessageBox::information(this,"移动文件",pdu->caData);
        break;
    }
        
    
        
    default:
        break;
    }
    free(pdu);
    pdu=NULL;
}
#if 0
void TcpClient::on_send_pb_clicked()
{
    QString strMsg = ui->lineEdit->text();//获取LineEdit上要发送的数据
    if(!strMsg.isEmpty()){//检查一下信息是不是空的
        PDU *pdu=mkPDU(strMsg.size()+1);//要发送数据的大小
        pdu->uiMsgType=8888;//消息类型
        memcpy(pdu->caMsg,strMsg.toStdString().c_str(),strMsg.size());//复制
        qDebug()<<(char*)(pdu->caMsg);
        m_tcpSocket.write((char*)pdu,pdu->uiPDULen);//发送
        free(pdu);//发送之后释放空间
        pdu=NULL;
    }else{
        QMessageBox::warning(this,"信息发送","发送的信息不能为空");
    }
}
#endif
void TcpClient::on_login_pb_clicked()//登录
{
    QString strName = ui->name_le->text();
    QString strPwd = ui->pwd_le->text();
    if(strName.isEmpty()){
         QMessageBox::critical(this,"登录","登录失败：用户名为空");
    }else if(strPwd.isEmpty()){
        QMessageBox::critical(this,"登录","登录失败：密码为空");
    }else{
        qDebug()<<strName;
        qDebug()<<strPwd;
        PDU* pdu=mkPDU(0);
        pdu->uiMsgType=ENUM_MSG_TYPE_LOGIN_REQUEST;
        strncpy(pdu->caData,strName.toStdString().c_str(),32);
        strncpy(pdu->caData+32,strPwd.toStdString().c_str(),32);
        m_strLoginName =strName;
        m_tcpSocket.write((char*)pdu,pdu->uiPDULen);
        free(pdu);
        pdu=NULL;
    }
}

void TcpClient::on_regist_pb_clicked()//注册
{
    QString strName = ui->name_le->text();
    QString strPwd = ui->pwd_le->text();
    if(strName.isEmpty()){
         QMessageBox::critical(this,"注册","注册失败：用户名为空");
    }else if(strPwd.isEmpty()){
        QMessageBox::critical(this,"注册","注册失败：密码为空");
    }else{
        qDebug()<<strName;
        qDebug()<<strPwd;
        PDU* pdu=mkPDU(0);
        pdu->uiMsgType=ENUM_MSG_TYPE_REGIST_REQUEST;
        strncpy(pdu->caData,strName.toStdString().c_str(),32);
        strncpy(pdu->caData+32,strPwd.toStdString().c_str(),32);
        m_tcpSocket.write((char*)pdu,pdu->uiPDULen);
        free(pdu);
        pdu=NULL;
    }
}

void TcpClient::on_cancel_pb_clicked()//注销
{
    
}

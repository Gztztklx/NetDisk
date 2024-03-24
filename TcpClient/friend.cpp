#include "friend.h"
#include "protocol.h"
#include "tcpclient.h"
#include <QInputDialog>//输入框，专门用来输入数据的
#include <QDebug>
#include "privatrchat.h"
#include <QMessageBox>

Friend::Friend(QWidget *parent) : QWidget(parent)
{
    m_pShowMsgTE = new QTextEdit;//显示信息的对象
    m_pFrinendListWidget = new QListWidget;//显示好友列表
    m_pInputMsgLE = new QLineEdit;//信息输入框
    
    m_pname = new QLabel("");
    m_pDelFriendPB = new QPushButton("删除好友");//删除好友按钮
    m_pFlushFriendPB = new QPushButton("刷新在线好友");//刷新在线好友列表按钮
    m_pShowOnLineUsrPB = new QPushButton("显示在线用户");//查看在线的用户的按钮
    m_pSearchUsrPB = new QPushButton("查找用户");//查找用户的按钮
    m_pMsgSendPB = new QPushButton("信息发送");//发送信息的按钮
    m_pPrivateChatPB = new QPushButton("私聊");//私聊的按钮
    
    //布局
    QVBoxLayout *pRightPBVBL = new QVBoxLayout;//第一个垂直布局
    pRightPBVBL->addWidget(m_pname);//用户名的标签
    pRightPBVBL->addWidget(m_pDelFriendPB);//删除好友按钮
    pRightPBVBL->addWidget(m_pFlushFriendPB);//刷新在线好友列表按钮
    pRightPBVBL->addWidget(m_pShowOnLineUsrPB);//查看在线的用户的按钮
    pRightPBVBL->addWidget(m_pSearchUsrPB);//查找用户的按钮
    pRightPBVBL->addWidget(m_pPrivateChatPB);//私聊的按钮
    
    QHBoxLayout *pTopHBL = new QHBoxLayout;//第一个水平布局
    pTopHBL ->addWidget(m_pShowMsgTE);//显示信息的对象
    pTopHBL ->addWidget(m_pFrinendListWidget);//显示好友列表
    pTopHBL ->addLayout(pRightPBVBL);//最后加上第一个垂直布局
    
    QHBoxLayout *pMsgHBL =new QHBoxLayout;//第二个水平布局
    pMsgHBL->addWidget(m_pInputMsgLE);//信息输入框
    pMsgHBL->addWidget(m_pMsgSendPB);//发送信息的按钮
    
    m_pOnline = new Online;
    
    QVBoxLayout *pMain = new QVBoxLayout;//总的布局
    pMain->addLayout(pTopHBL);//加上第一个水平布局
    pMain->addLayout(pMsgHBL);//加上第二个水平布局
    pMain->addWidget(m_pOnline);
    m_pOnline->hide();//隐藏
    
    setMPname();
    
    setLayout(pMain);//所有的控件都在最后这个布局里面
    connect(m_pShowOnLineUsrPB,SIGNAL(clicked(bool)),this,SLOT(showOnline()));//连接“显示所有用户”的按钮和显示所有用户的槽函数
    connect(m_pSearchUsrPB,SIGNAL(clicked(bool)),this,SLOT(searchUsr()));//连接“查找指定用户”和查找指定用户的槽函数
    connect(m_pFlushFriendPB,SIGNAL(clicked(bool)),this,SLOT(flushFriend()));//连接“刷新好友”按钮和刷新好友列表的槽函数
    connect(m_pDelFriendPB,SIGNAL(clicked(bool)),this,SLOT(delFriend()));//连接“删除好友”按钮和删除好友的槽函数
    connect(m_pPrivateChatPB,SIGNAL(clicked(bool)),this,SLOT(privateChat()));//连接私聊按钮和私聊的槽函数，显示私聊界面
    connect(m_pMsgSendPB,SIGNAL(clicked(bool)),this,SLOT(groupChat()));//连接“信息发送”按钮和群聊的槽函数
}

void Friend::showALLOnlineUsr(PDU* pdu)
{
    if(pdu == NULL){
        return;
    }else{
        m_pOnline->showUsr(pdu);
    }
}

void Friend::setMPname()
{
    m_pname->setText(TcpClient::getInstance().getname());
}

void Friend::updateFriendList(PDU *pdu)
{
    if(pdu == NULL){
        return;
    }
    uint uiSize = pdu->uiMsgLen/32;
    char caName[32] = {'\0'};
    for(uint i=0;i<uiSize;i++){
        memcpy(caName,(char*)(pdu->caMsg)+i*32,32);
        m_pFrinendListWidget->clear();
        m_pFrinendListWidget->addItem(caName);
    }
}

void Friend::updateGroundMsg(PDU *pdu)
{
    char caPerName[32] = {'\0'};
    strncpy(caPerName,pdu->caData,32);
    QString strMsg = QString("【%1】 says : %2").arg(caPerName).arg((char*)pdu->caMsg);
    m_pShowMsgTE->append(strMsg);
}

QListWidget *Friend::getFriendList()
{
    return m_pFrinendListWidget;
}

void Friend::showOnline()
{
    if(m_pOnline->isHidden()){
        m_pOnline->show();
        
        PDU* pdu=mkPDU(0);
        pdu->uiMsgType=ENUM_MSG_TYPE_ALL_ONLINE_REQUEST;
        TcpClient::getInstance().getTcpSocket().write((char*)pdu,pdu->uiPDULen);
        free(pdu);
        pdu=NULL;
    }else{
        m_pOnline->hide();
      
    }
}

void Friend::searchUsr()
{
    m_strSearchName = QInputDialog::getText(this,"查找指定用户","用户名");
    if(!m_strSearchName.isEmpty()){
        qDebug()<<m_strSearchName;
        PDU* pdu = mkPDU(0);
        pdu->uiMsgType=ENUM_MSG_TYPE_SEARCH_USR_REQUEST;
        memcpy(pdu->caData,m_strSearchName.toStdString().c_str(),m_strSearchName.size());
        TcpClient::getInstance().getTcpSocket().write((char*)pdu,pdu->uiPDULen);
        free(pdu);
        pdu=NULL;
    }
}

void Friend::flushFriend()
{
    QString strname = TcpClient::getInstance().getname();
    PDU* pdu = mkPDU(0);
    pdu->uiMsgType = ENUM_MSG_TYPE_FLUSH_FRIEND_REQUEST;
    memcpy(pdu->caData,strname.toStdString().c_str(),strname.size());
    TcpClient::getInstance().getTcpSocket().write((char*)pdu,pdu->uiPDULen);
    free(pdu);
    pdu=NULL;
}

void Friend::delFriend()
{
    if(m_pFrinendListWidget->currentItem() == NULL){
        return;
    }
    QString strFriendname = m_pFrinendListWidget->currentItem()->text();
    qDebug()<<strFriendname;
    PDU* pdu=mkPDU(0);
    QString strSelfName = TcpClient::getInstance().getname();
    pdu->uiMsgType = ENUM_MSG_TYPE_DELETE_FRIEND_REQUEST;
    memcpy(pdu->caData,strSelfName.toStdString().c_str(),strSelfName.size());
    memcpy(pdu->caData+32,strFriendname.toStdString().c_str(),strFriendname.size());
    TcpClient::getInstance().getTcpSocket().write((char*)pdu,pdu->uiPDULen);
    free(pdu);
    pdu=NULL;
}

void Friend::privateChat()
{
    if(m_pFrinendListWidget->currentItem() == NULL){
        QMessageBox::information(this,"私聊","请选择私聊对象：");
        return ;
    }
    QString strChatName = m_pFrinendListWidget->currentItem()->text();
    Privatrchat::getInstance().setChatName(strChatName);
    if(Privatrchat::getInstance().isHidden()){
        Privatrchat::getInstance().show();
    }
}

void Friend::groupChat()//群聊
{
    QString strMsg = m_pInputMsgLE->text();
    if(!strMsg.isEmpty()){
        PDU* pdu=mkPDU(strMsg.size()*4+1);
        pdu->uiMsgType = ENUM_MSG_TYPE_GROUND_CHAT_REQUEST;//群聊请求
        QString strName = TcpClient::getInstance().getname();
        strncpy(pdu->caData,strName.toStdString().c_str(),strName.size());
        strcpy((char*)(pdu->caMsg),strMsg.toStdString().c_str());
        TcpClient::getInstance().getTcpSocket().write((char*)pdu,pdu->uiPDULen);
        QString data = QString("%1 : says 【%2】 ").arg(strMsg).arg(strName);
       m_pShowMsgTE->append(data);
       m_pInputMsgLE->clear();
       free(pdu);
       pdu=NULL;
    }else{
        QMessageBox::warning(this,"信息发送","信息输入框不能为空");
    }
    
}


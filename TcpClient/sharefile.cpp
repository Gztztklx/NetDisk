#include "sharefile.h"
#include <QDebug>
#include "tcpclient.h"

shareFile::shareFile(QWidget *parent) : QWidget(parent)
{
    m_pSelectAllPB = new QPushButton("全选");//全选
    m_pCancelSelectPB = new QPushButton("取消选择");//反选
    m_pOKPB = new QPushButton("确定");//确定
    m_pCancelPB = new QPushButton("取消");//取消此次分享
    m_pSA = new QScrollArea;//中间用来显示好友的展示区域
    m_pFriendW = new QWidget;
    m_pFriendWVBL = new QVBoxLayout(m_pFriendW);
    m_pButtonGroup = new QButtonGroup(m_pFriendW);//用来管理我们的好友
    m_pButtonGroup->setExclusive(false);//多选
    QHBoxLayout *pTopHBL = new QHBoxLayout;
    pTopHBL->addWidget(m_pSelectAllPB);
    pTopHBL->addWidget(m_pCancelSelectPB);
    pTopHBL->addStretch();//在右边加一个弹簧
    QHBoxLayout *pDownHBL = new QHBoxLayout;
    pDownHBL->addWidget(m_pOKPB);
    pDownHBL->addWidget(m_pCancelPB);
    QVBoxLayout* pMainHBL = new QVBoxLayout;
    pMainHBL->addLayout(pTopHBL);
    pMainHBL->addWidget(m_pSA);
    pMainHBL->addLayout(pDownHBL);
    setLayout(pMainHBL);
    connect(m_pCancelSelectPB,SIGNAL(clicked(bool)),this,SLOT(cancelSelect()));
    connect(m_pSelectAllPB,SIGNAL(clicked(bool)),this,SLOT(selectAll()));
    connect(m_pOKPB,SIGNAL(clicked(bool)),this,SLOT(ok()));
    connect(m_pCancelPB,SIGNAL(clicked(bool)),this,SLOT(cancel()));
}

shareFile &shareFile::getInstance()
{
    static shareFile instance;
    return instance;
}

void shareFile::updateFriend(QListWidget *pFriendList)
{
    if(NULL == pFriendList){
        return;
    }
    QAbstractButton* temp = NULL;
    QList<QAbstractButton*> preFriendList = m_pButtonGroup->buttons();//分享文件时会获得此时的在线好友列表，但之前分享文件时的好友列表还在上面，需要先清楚之前的列表
    for(int i=0;i<preFriendList.size();i++){
        temp = preFriendList[i];
        m_pFriendWVBL->removeWidget(preFriendList[i]);
        m_pButtonGroup->removeButton(preFriendList[i]);
        preFriendList.removeOne(temp);
        delete temp;
        temp = NULL;
    }//通过循环移除
    QCheckBox *pCB = NULL;
    qDebug()<<"pFriendList->Count:"<<pFriendList->count();
    qDebug()<<"pFriendList->Size:"<<pFriendList->size();
    for(int i=0;i<pFriendList->count();i++){
        qDebug()<<"第"<<i<<"次布局";
        pCB = new QCheckBox(pFriendList->item(i)->text());
        qDebug()<<pFriendList->item(i)->text();
        m_pFriendWVBL->addWidget(pCB);
        m_pButtonGroup->addButton(pCB);
    }
    m_pSA->setWidget(m_pFriendW);
}

void shareFile::cancelSelect()//取消选择按钮
{
    QList<QAbstractButton*> cbList = m_pButtonGroup->buttons();
    for(int i=0;i<cbList.size();i++){
        if(cbList[i]->isChecked()){
            cbList[i]->setChecked(false);
        }
    }
}

void shareFile::selectAll()//全选
{
    QList<QAbstractButton*> cbList = m_pButtonGroup->buttons();
    for(int i=0;i<cbList.size();i++){
        if(!cbList[i]->isChecked()){
            cbList[i]->setChecked(true);
        }
    }  
}

void shareFile::ok()//确定，开始分享
{
    QString strName = TcpClient::getInstance().getname();
    QString strCurPath = TcpClient::getInstance().getCurPath();
    QString strShareFileName = OpeWidget::getInstance().getBook()->getShareFileName();
    QString strPath = strCurPath+"/"+strShareFileName;
    QList<QAbstractButton*> cbList = m_pButtonGroup->buttons();
    int num = 0;
    for(int i=0;i<cbList.size();i++){
        if(cbList[i]->isChecked()){
            num++;
        }
    }
    PDU* pdu = mkPDU(32*num + strPath.size()*3+1);
    pdu->uiMsgType = ENUM_MSG_TYPE_SHARE_FILE_REQUEST;
    sprintf(pdu->caData,"%s %d",strName.toStdString().c_str(),num);
    int j=0;
    for(int i=0;i<cbList.size();i++){
        if(cbList[i]->isChecked()){
            memcpy((char*)(pdu->caMsg)+j*32,cbList[i]->text().toStdString().c_str(),32);
            j++;
        }  
    }
    memcpy((char*)(pdu->caMsg)+num*32,strPath.toStdString().c_str(),strPath.size());
    TcpClient::getInstance().getTcpSocket().write((char*)pdu,pdu->uiPDULen);
    free(pdu);
    pdu=NULL;
}

void shareFile::cancel()//取消分享
{
    hide();
}

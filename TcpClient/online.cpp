#include "online.h"
#include "ui_online.h"
#include <QDebug>
#include "tcpclient.h"

Online::Online(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Online)
{
    ui->setupUi(this);
}

Online::~Online()
{
    delete ui;
}

void Online::showUsr(PDU *pdu)
{
    if(pdu == NULL){
        return;
    }else{
        uint uiSize = pdu->uiMsgLen/32;
        char caTmp[32];
        for(uint i=0;i<uiSize;i++){
            memcpy(caTmp,(char*)(pdu->caMsg)+i*32,32);
            ui->online_hw->addItem(caTmp);
        }
    }
}

void Online::on_addFriend_pb_clicked()
{
    QListWidgetItem *pItem = ui->online_hw->currentItem();//获取添加好友页面当前行
    QString strPerUsrName = pItem->text();//获取内容,获取了要添加好友的用户名
    QString strLoginName = TcpClient::getInstance().getname();//获取了自己登录时的用户名
    PDU* pdu = mkPDU(0);
    pdu->uiMsgType = ENUM_MSG_TYPE_ADD_FRIEND_REQUEST;
    memcpy(pdu->caData,strPerUsrName.toStdString().c_str(),strPerUsrName.size());//要添加的好友的名字
    memcpy(pdu->caData+32,strLoginName.toStdString().c_str(),strLoginName.size());//自己的名字
    TcpClient::getInstance().getTcpSocket().write((char*)pdu,pdu->uiPDULen);//发送
    free(pdu);
    pdu=NULL;
}

#include "privatrchat.h"
#include "ui_privatrchat.h"
#include "protocol.h"
#include "tcpclient.h"
#include <QMessageBox>

Privatrchat::Privatrchat(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Privatrchat)
{
    ui->setupUi(this);
}

Privatrchat::~Privatrchat()
{
    delete ui;
}

void Privatrchat::setChatName(QString strName)
{
    m_strChatName = strName;
    m_strLoginName = TcpClient::getInstance().getname();
}

Privatrchat &Privatrchat::getInstance()
{
    static Privatrchat instance;
    return instance;
}

void Privatrchat::updateMsg(const PDU *pdu)
{
    if(pdu == NULL){
        return;
    }
    char caSendName[32] = {'\0'};
    memcpy(caSendName,pdu->caData,32);
    QString strMsg = QString("【%1】 says: %2").arg(caSendName).arg((char*)(pdu->caMsg));
    ui->showMsg_te->append(strMsg);
}

void Privatrchat::on_sendMsg_pb_clicked()
{
    QString strMsg = ui->inputNsg_le->text();
    if(!strMsg.isEmpty()){
        PDU* pdu=mkPDU(strMsg.size()*4+1);
        pdu->uiMsgType = ENUM_MSG_TYPE_PRIVATR_CHAT_REQUEST;
        memcpy(pdu->caData,m_strLoginName.toStdString().c_str(),m_strLoginName.size());
        memcpy(pdu->caData+32,m_strChatName.toStdString().c_str(),m_strChatName.size());
        strcpy((char*)pdu->caMsg,strMsg.toStdString().c_str());
        TcpClient::getInstance().getTcpSocket().write((char*)pdu,pdu->uiPDULen);
        free(pdu);
        pdu=NULL;
        QString data = QString("%1 : says 【%2】 ").arg(strMsg).arg(m_strLoginName);
        ui->showMsg_te->append(data);
        ui->inputNsg_le->clear();
    }else{
        QMessageBox::information(this,"输入文本框","发送不能为空");
    }
}

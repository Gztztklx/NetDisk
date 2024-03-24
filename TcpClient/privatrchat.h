#ifndef PRIVATRCHAT_H
#define PRIVATRCHAT_H

#include <QWidget>
#include "protocol.h"
namespace Ui {
class Privatrchat;
}

class Privatrchat : public QWidget
{
    Q_OBJECT
    
public:
    explicit Privatrchat(QWidget *parent = 0);
    ~Privatrchat();
    
    void setChatName(QString strName);
    static Privatrchat &getInstance();
    void updateMsg(const PDU* pdu);
    
private slots:
    void on_sendMsg_pb_clicked();
    
private:
    Ui::Privatrchat *ui;
    QString m_strChatName;
    QString m_strLoginName;
};

#endif // PRIVATRCHAT_H

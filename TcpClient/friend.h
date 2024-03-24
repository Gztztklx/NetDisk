#ifndef FRIEND_H
#define FRIEND_H

#include <QWidget>
#include <QTextEdit>//信息的显示
#include <QListWidget>
#include <QLineEdit>//信息的输入
#include <QPushButton>//按钮
#include <QVBoxLayout>//垂直布局
#include <QHBoxLayout>//水平布局
#include <QLabel>//标签
#include "online.h"



class Friend : public QWidget
{
    Q_OBJECT
public:
    explicit Friend(QWidget *parent = nullptr);
    void showALLOnlineUsr(PDU* pdu);
    void setMPname();
    QString m_strSearchName;//搜索指定用户时保存一下名字
    void updateFriendList(PDU* pdu);//更新好友列表
    void updateGroundMsg(PDU *pdu);//更新群聊信息
    QListWidget *getFriendList();
    
signals:
    
public slots:
    void showOnline();//展示所有在线用户的槽函数
    void searchUsr();//查找指定用户的槽函数
    void flushFriend();//刷新好友列表的按钮的槽函数
    void delFriend();//删除好友的槽函数
    void privateChat();//私聊按钮的槽函数
    void groupChat();//群聊的槽函数
    
private:
    QTextEdit *m_pShowMsgTE;//显示信息的对象
    QListWidget *m_pFrinendListWidget;//显示好友列表
    QLineEdit *m_pInputMsgLE;//信息输入框
    
    QLabel *m_pname;//用户名
    QPushButton *m_pDelFriendPB;//删除好友按钮
    QPushButton *m_pFlushFriendPB;//刷新在线好友列表按钮
    QPushButton *m_pShowOnLineUsrPB;//查看在线的用户
    QPushButton *m_pSearchUsrPB;//查找某个用户的按钮
    QPushButton *m_pMsgSendPB;//发送信息的按钮
    QPushButton *m_pPrivateChatPB;//私聊的按钮
    
    Online* m_pOnline;//产生Online的对象
    
    
};

#endif // FRIEND_H

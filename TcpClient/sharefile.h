#ifndef SHAREFILE_H
#define SHAREFILE_H

#include <QWidget>
#include <QPushButton>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QButtonGroup>
#include <QScrollArea>
#include <QCheckBox>
#include <QListWidget>

class shareFile : public QWidget
{
    Q_OBJECT
public:
    explicit shareFile(QWidget *parent = nullptr);
    static shareFile& getInstance();
    void updateFriend(QListWidget *pFriendList);
    
signals:
    
public slots:
    void cancelSelect();//取消选择
    void selectAll();
    void ok();
    void cancel();
    
private:
    QPushButton *m_pSelectAllPB;//全选
    QPushButton *m_pCancelSelectPB;//取消选择
    
    QPushButton *m_pOKPB;//确定
    QPushButton *m_pCancelPB;//取消此次分享
    
    QScrollArea *m_pSA;//中间用来显示好友的展示区域
    QWidget *m_pFriendW;
    QVBoxLayout *m_pFriendWVBL;//这一块小区域的垂直布局
    QButtonGroup *m_pButtonGroup;//用来管理我们的好友
    
};

#endif // SHAREFILE_H

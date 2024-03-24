#ifndef OPEDB_H
#define OPEDB_H

#include <QObject>
#include <QSqlDatabase>//连接数据库
#include <QSqlQuery>//查询数据库
#include <QStringList>//字符串列表

class OpeDB : public QObject
{
    Q_OBJECT
public:
    explicit OpeDB(QObject *parent = nullptr);
    static OpeDB& getInstance();
    void init();
    ~OpeDB();
    bool handleRegist(const char *name,const char *pwd);//数据库里处理注册的函数，如果插入数据成功，就代表注册成功
    bool handleLogin(const char *name,const char *pwd);//数据库里处理登录的函数
    void handleOffline(const char *name);//客户端下线时的处理函数
    QStringList handleAllOnline();//数据库关于显示所有在线用户的操作
    int handleSearchUsr(const char *name);//数据库中关于搜索指定用户的操作
    int handleAddFriend(const char* pername,const char *name);//查找一下看看他们是不是好友
    void handleAgreeAddFriend(const char* pername, const char* name);//让两个本来不是好友的用户在数据库中成为好友
    QStringList handleFLushFriend(const char* name);//刷新好友列表
    bool handleDelFriend(const char* name,const char* friendName);
    
    
signals:
    
public slots:
    
private:
    QSqlDatabase m_db;//连接数据库
};

#endif // OPEDB_H

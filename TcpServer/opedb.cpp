#include "opedb.h"
#include <QMessageBox>
#include <QDebug>

OpeDB::OpeDB(QObject *parent) : QObject(parent)
{
    m_db=QSqlDatabase::addDatabase("QSQLITE");
}

OpeDB &OpeDB::getInstance()
{
    static OpeDB instance;
    return instance;
}

void OpeDB::init()//数据库初始化
{
    m_db.setHostName("localhost");//连接数据库的ip地址，localhost：本地
    m_db.setDatabaseName("C:\\Users\\86187\\Desktop\\qt\\project\\iotek_rayfile\\TcpServer\\cloud.db");//要打开数据库的名字
    
    if(m_db.open()){
        QSqlQuery query;
        query.exec("select* from usrInfo");
        while(query.next()){
            QString data = QString("%1,%2,%3").arg(query.value(0).toString()).arg(query.value(1).toString()).arg(query.value(2).toString());
            qDebug()<<data;
        }
        query.exec("update usrInfo set online=0 where online=1;");//将所有online=1 的用户online 改为0
    }else{
        QMessageBox::critical(NULL,"打开数据库","打开数据库失败");
        
    }
}

OpeDB::~OpeDB()
{
    m_db.close();
}

bool OpeDB::handleRegist(const char *name, const char *pwd)//注册
{
    if(name == NULL || pwd == NULL){
        qDebug()<<"name | pwd is NULL";
        return false;
    }else{
        QString data = QString("insert into usrInfo(name,pwd) values(\'%1\',\'%2\');").arg(name).arg(pwd);//插入数据库的语句
        qDebug()<<data;
        QSqlQuery query;
        return query.exec(data);//返回执行的结果
    }
}

bool OpeDB::handleLogin(const char *name, const char *pwd)//登录
{
    if(name == NULL || pwd == NULL){
        qDebug()<<"name | pwd is NULL";
        return false;
    }else{
        QString data = QString("select* from usrInfo where name=\'%1\' and pwd = \'%2\' and online =0;").arg(name).arg(pwd);//插入数据库的语句
        qDebug()<<data;
        QSqlQuery query;
        query.exec(data);//返回执行的结果
        if(query.next()){
            data=QString("update usrInfo set online=1 where name=\'%1\' and pwd = \'%2\';").arg(name).arg(pwd);//将在线状态置为1
       //     qDebug()<<data;
            QSqlQuery query;
            query.exec(data);
            return true;
        }else{
            return false;
        }
    }
}

void OpeDB::handleOffline(const char *name)//从数据库让该用户下线
{
    if(name == NULL){
        qDebug()<<"name is NULL";
        return ;
    }else{
        QString data=QString("update usrInfo set online=0 where name=\'%1\';").arg(name);//将在线状态置为0
    //    qDebug()<<data;
        QSqlQuery query;
        query.exec(data);
    }
}

QStringList OpeDB::handleAllOnline()//数据库关于显示所有在线用户的操作
{
    QString data = QString("select name from usrInfo where online=1;");
    QSqlQuery query;
    query.exec(data);
    QStringList result;
    result.clear();
    while (query.next()) {
        result.append(query.value(0).toString());//value得到的类型是QVariant，然后用toString转换成字符串放进result字符串列表中
        qDebug()<<query.value(0).toString();
    }
    qDebug()<<"--------------------";
    return result;
}

int OpeDB::handleSearchUsr(const char *name)
{
    if(name == NULL){
        return -1;
    }
    QString data = QString("select online from usrInfo where name=\'%1\';").arg(name);
    QSqlQuery query;
    query.exec(data);
    if(query.next()){//表示存在
        int ret = query.value(0).toInt();
        if(ret == 1){//存在且在线
            return 1;
        }else if(ret == 0){//存在但不在线
            return 0;
        }
    }
        return -1;//不存在
}

int OpeDB::handleAddFriend(const char *pername, const char *name)
{
    if(pername == NULL || name == NULL){
        return -1;//程序错误，操作无效
    }
    QString data = QString("select* from friend where (id=(select id   from usrInfo where name=\'%1\') and friendId =(select id from usrInfo where name=\'%2\')) or (id=(select id from usrInfo where name=\'%3\') and friendId =(select id from usrInfo where name=\'%4\'));").arg(pername).arg(name).arg(name).arg(pername);
//    qDebug()<<data;
    QSqlQuery query;
    query.exec(data);
    if(query.next()){
        return 0;//双方已经互为好友
    }else{//如果双方不是好友，就判断一下在不在线
        QString data = QString("select online from usrInfo where name=\'%1\';").arg(pername);
        QSqlQuery query;
        query.exec(data);
        if(query.next()){//表示存在
            int ret = query.value(0).toInt();
            if(ret == 1){//存在且在线
                return 1;
            }else if(ret == 0){//存在但不在线
                return 2;
            }else{
                return 3;//不存在
            }
        }      
    }
    return 3;//不存在
}

void OpeDB::handleAgreeAddFriend(const char *pername, const char *name)
{
    if(pername == NULL || name == NULL){
        return;
    }
    QString data = QString("insert into friend(id,friendId) values((select id from usrInfo where name=\'%1\'),(select id from usrInfo where name=\'%2\'));").arg(pername).arg(name);
    qDebug()<<data;
    QSqlQuery query;
    bool ret = query.exec(data);
    if(ret){
        qDebug()<<"数据库里添加好友成功";
    }else{
        qDebug()<<"数据库里添加好友失败";
    }
}

QStringList OpeDB::handleFLushFriend(const char *name)
{
    QStringList strFriendList;
    strFriendList.clear();//产生一个空的好友列表
    if(name == NULL){
        return strFriendList;
    }
    QString data = QString("select name from usrInfo where online=1 and  id in (select id from friend where friendId=(select id from usrInfo where name=\'%1\'))").arg(name);
 //   qDebug()<<data;
    QSqlQuery query;
    query.exec(data);
    while(query.next()){
        strFriendList.append(query.value(0).toString());
        qDebug()<<query.value(0).toString();
    }
            data = QString("select name from usrInfo where online=1  and id in (select friendId from friend where id=(select id from usrInfo where name=\'%1\'))").arg(name);
  //  qDebug()<<data;
    //query.clear();
    query.exec(data);
    while(query.next()){
        strFriendList.append(query.value(0).toString());
        qDebug()<<query.value(0).toString();
    }
    return strFriendList;
}

bool OpeDB::handleDelFriend(const char *name, const char *friendName)
{
    if(name == NULL || friendName == NULL){
        return false;
    }
    QString data = QString("delete from friend where id=(select id from usrInfo where name=\'%1\' ) and friendId=(select if from usrInfo where name=\'%2\'); ").arg(name).arg(friendName);
    QSqlQuery query;
    query.exec(data);
    
    data = QString("delete from friend where id=(select id from usrInfo where name=\'%1\' ) and friendId=(select if from usrInfo where name=\'%2\'); ").arg(friendName).arg(name);
    query.exec(data);
    return true;
}

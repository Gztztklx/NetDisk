#ifndef PROTOCOL_H
#define PROTOCOL_H

//
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

typedef unsigned int uint;

#define REGIST_OK "regist ok"//注册成功
#define REGIST_FAILED "regist failed ： name existed"//注册失败:用户名重复
#define LOGIN_OK "login ok"//登录成功
#define LOGIN_FAILED "login failed ： name error or pwd error or relogin"//登录失败:用户名或密码或重复登录

#define SEARCH_USR_NO "no such people"  //没有该用户
#define SEARCH_USR_ONLINE "online"  //该用户在线
#define SEARCH_USR_OFFLINE "offline" //该用户离开

#define UNKNOW_ERROR "unknow err" //未知错误
#define EXISTED_FRIEND "friend exist" //朋友已经存在
#define ADD_FRIEND_OFFLINE "usr offline" //要添加的用户已离开
#define ADD_FRIEND_NO_EXIST "usr not exist" //要添加的用户不存在

#define DELETE_FRIEND_OK "delete friend ok"  //删除好友成功

#define DIR_CREATE_OK "dir create ok" //文件夹创建成功
#define FILE_NAME_EXIST "file name exist" //文件名已经存在

#define PATH_NO_EXIST "path no exist" //路径不存在


#define DEL_OK "delete ok"  //删除文件成功
#define DEL_FAILED "delete  failed" //删除文件失败
#define RENAME_OK "rename ok"   //重命名成功
#define RENAME_FAILED "rename failed"   //重命名失败

#define MOVE_FILE_OK "move file ok"
#define MOVE_FILE_FAILED "move file failed"
#define SYSTEM_ERROR "system_error"

enum ENUM_MSG_TYPE{
    ENUM_MSG_TYPE_MIN = 0,//最小
    
    ENUM_MSG_TYPE_REGIST_REQUEST,//注册请求
    ENUM_MSG_TYPE_REGIST_RESPOND,//注册回复
    
    ENUM_MSG_TYPE_LOGIN_REQUEST,//登录请求
    ENUM_MSG_TYPE_LOGIN_RESPOND,//登录回复
    
    ENUM_MSG_TYPE_ALL_ONLINE_REQUEST,//所有在线用户请求
    ENUM_MSG_TYPE_ALL_ONLINE_RESPOND,//所有在线用户回复
    
    ENUM_MSG_TYPE_SEARCH_USR_REQUEST,//搜索指定用户请求
    ENUM_MSG_TYPE_SEARCH_USR_RESPOND,//搜索指定用户回复
    
    ENUM_MSG_TYPE_ADD_FRIEND_REQUEST,//添加好友请求
    ENUM_MSG_TYPE_ADD_FRIEND_RESPOND,//添加好友回复
    
    ENUM_MSG_TYPE_ADD_FRIEND_AGGREE,//同意好友请求
    ENUM_MSG_TYPE_ADD_FRIEND_REFUSE,//拒绝好友请求
    
    ENUM_MSG_TYPE_FLUSH_FRIEND_REQUEST,//刷新好友请求
    ENUM_MSG_TYPE_FLUSH_FRIEND_RESPOND,//刷新好友回复
    
    ENUM_MSG_TYPE_DELETE_FRIEND_REQUEST,//删除好友请求
    ENUM_MSG_TYPE_DELETE_FRIEND_RESPOND,//删除好友回复
    
    ENUM_MSG_TYPE_PRIVATR_CHAT_REQUEST,//私聊请求
    ENUM_MSG_TYPE_PRIVATR_CHAT_RESPOND,//私聊回复
    
    ENUM_MSG_TYPE_GROUND_CHAT_REQUEST,//群聊请求
    ENUM_MSG_TYPE_GROUND_CHAT_RESPOND,//群聊回复
    
    ENUM_MSG_TYPE_CREATE_DIR_REQUEST,//创建文件夹请求
    ENUM_MSG_TYPE_CREATE_DIR_RESPOND,//创建文件夹回复
    
    ENUM_MSG_TYPE_FLUSH_FILE_REQUEST,//刷新文件请求
    ENUM_MSG_TYPE_FLUSH_FILE_RESPOND,//刷新文件回复
    
    ENUM_MSG_TYPE_DEL_DIR_REQUEST,//删除文件夹请求
    ENUM_MSG_TYPE_DEL_DIR_RESPOND,//删除文件夹回复

    ENUM_MSG_TYPE_RENAME_REQUEST,//重命名文件请求
    ENUM_MSG_TYPE_RENAME_RESPOND,//重命名文件回复
    
    ENUM_MSG_TYPE_ENTER_FILE_REQUEST,//进入文件夹请求
    ENUM_MSG_TYPE_ENTER_FILE_RESPOND,//进入文件夹回复
    
    ENUM_MSG_TYPE_UPLOAD_FILE_REQUEST,//上传文件请求
    ENUM_MSG_TYPE_UPLOAD_FILE_RESPOND,//上传文件回复
      
    ENUM_MSG_TYPE_DOWN_LOAD_REQUEST,//下载文件请求
    ENUM_MSG_TYPE_DOWN_LOAD_RESPOND,//下载文件回复
    
    ENUM_MSG_TYPE_SHARE_FILE_REQUEST,//共享文件请求
    ENUM_MSG_TYPE_SHARE_FILE_RESPOND,//共享文件回复
    ENUM_MSG_TYPE_SHARE_FILE_NOTE,//共享文件通知
    ENUM_MSG_TYPE_SHARE_FILE_NOTE_RESPOND,//共享文件通知的回复
    
    ENUM_MSG_TYPE_MOVE_FILE_REQUEST,//移动文件请求
    ENUM_MSG_TYPE_MOVE_FILE_RESPOND,//移动文件回复
        
    
    ENUM_MSG_TYPE_MAX = 0x00ffffff,//最大32
};

struct FileInfo
{
    char caFileName[32];//文件名字
    int iFileType; //文件类型
};


struct PDU
{
    uint uiPDULen;//总的协议数据单元大小
    uint uiMsgType;//消息类型
    char caData[64];
    uint uiMsgLen;//实际消息长度
    int caMsg[];//实际消息
};

PDU *mkPDU(uint uiMsgLen);

#endif // PROTOCOL_H

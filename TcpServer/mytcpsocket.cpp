#include "mytcpsocket.h"
#include <stdio.h>
#include <QDebug>
#include <QFileInfoList>
#include "mytcpserver.h"


MyTcpSocket::MyTcpSocket()
{
    connect(this,SIGNAL(readyRead()),this,SLOT(recvMsg()));
    connect(this,SIGNAL(disconnected()),this,SLOT(clientOffline()));
    m_bUpLoad = false;
    m_pTimer = new QTimer;
    connect(m_pTimer,SIGNAL(timeout()),this,SLOT(sendFileToClient()));
}

QString MyTcpSocket::getName()
{
    return m_strName;
}

void MyTcpSocket::copyDir(QString strSrcDir, QString strDestDir)
{
    QDir dir;
    dir.mkdir(strDestDir);//创建目标目录
    
    dir.setPath(strSrcDir);//设置好路径
    QFileInfoList fileInfoList = dir.entryInfoList();//获取源文件的文件信息
    QString srcTmp;
    QString destTmp;
    for(int i=0;i<fileInfoList.size();i++){
        if(fileInfoList[i].isFile()){
            srcTmp = strSrcDir+'/'+fileInfoList[i].fileName();
            destTmp = strSrcDir+'/'+fileInfoList[i].fileName();
            bool ret = QFile::copy(srcTmp,destTmp);
            if(ret){
                qDebug()<<"共享成功";
            }else{
                qDebug()<<"共享失败";
            }
            
        }else if(fileInfoList[i].isDir()){
            if(QString(".") == fileInfoList[i].fileName() || QString("..") == fileInfoList[i].fileName()){
                continue;
            }
            srcTmp = strSrcDir+'/'+fileInfoList[i].fileName();
            destTmp = strSrcDir+'/'+fileInfoList[i].fileName();
            copyDir(srcTmp,destTmp);
        }
    }
     
}

void MyTcpSocket::recvMsg()
{
    if(m_bUpLoad){//以上传文件的状态
        QByteArray buff = readAll();
        m_file.write(buff);
        m_iRecved += buff.size();
        qDebug()<<"正在接收文件";
        if(m_iTotal <= m_iRecved){
            m_file.close();
            m_bUpLoad = false;
            PDU* respdu = mkPDU(0);
            respdu->uiMsgType = ENUM_MSG_TYPE_UPLOAD_FILE_RESPOND;
            strcpy(respdu->caData,UPLOAD_FILE_OK);
            write((char*)respdu,respdu->uiPDULen);
            free(respdu);
            respdu=NULL;
            qDebug()<<"文件接收完毕";
        }
    }
    qDebug()<<this->bytesAvailable();//获取发送来的数据的大小，有可能会出错
    uint uiPDULen = 0;
    this->read((char*)&uiPDULen,sizeof(uint));//先收取四个字节，获取此次发送来的数据总的大小
    uint uiMsgLen = uiPDULen-sizeof(PDU);//实际消息的长度=总的大小-结构体的大小
    PDU *pdu = mkPDU(uiMsgLen);
    this->read((char*)pdu+sizeof(uint),uiPDULen-sizeof(uint));
    switch(pdu->uiMsgType){
    case ENUM_MSG_TYPE_REGIST_REQUEST:{//如果是注册请求
        char caName[32]={'\0'};
        char caPwd[32]={'\0'};
        strncpy(caName,pdu->caData,32);
        strncpy(caPwd,pdu->caData+32,32);
        bool ret = OpeDB::getInstance().handleRegist(caName,caPwd);
        PDU* respdu = mkPDU(0);
        respdu->uiMsgType=ENUM_MSG_TYPE_REGIST_RESPOND;//类型为注册回复
        if(ret){
            strcpy (respdu->caData,REGIST_OK);
            write((char*)respdu,respdu->uiPDULen);//成功把结果返回给客户端
            free(respdu);
            respdu=NULL;
            QDir dir;
            qDebug()<<"create dir:"<<dir.mkdir(QString("./%1").arg(caName));
        }else{
            strcpy (respdu->caData,REGIST_FAILED);
            write((char*)respdu,respdu->uiPDULen);//失败把结果返回给客户端
            free(respdu);
            respdu=NULL;
        }
        break;
    }
    case ENUM_MSG_TYPE_LOGIN_REQUEST:{//如果是登录请求
        char caName[32]={'\0'};
        char caPwd[32]={'\0'};
        strncpy(caName,pdu->caData,32);
        strncpy(caPwd,pdu->caData+32,32);
        bool ret = OpeDB::getInstance().handleLogin(caName,caPwd);
        PDU* respdu = mkPDU(0);
        respdu->uiMsgType=ENUM_MSG_TYPE_LOGIN_RESPOND;//类型为登录回复
        if(ret){
            strcpy (respdu->caData,LOGIN_OK);
            m_strName = caName;//记录用户的用户名
            write((char*)respdu,respdu->uiPDULen);//成功把结果返回给客户端
            free(respdu);
            respdu=NULL;
        }else{
            qDebug()<<"ret:"<<ret;
            strcpy (respdu->caData,LOGIN_FAILED);
            qDebug()<<"respdu->caData:"<<respdu->caData;
            write((char*)respdu,respdu->uiPDULen);//失败把结果返回给客户端
            free(respdu);
            respdu=NULL;
        }
        break;
    }
    case ENUM_MSG_TYPE_ALL_ONLINE_REQUEST:{//如果是所有在线用户请求
        QStringList ret = OpeDB::getInstance().handleAllOnline();
        uint uiMsgLen = ret.size()*32;//消息部分的长度
        PDU* respdu = mkPDU(uiMsgLen);
        respdu->uiMsgType=ENUM_MSG_TYPE_ALL_ONLINE_RESPOND;//类型为所有在线用户回复
        for(int i=0;i<ret.size();i++){//将列表中的数据复制到caMsg
            memcpy((char*)(respdu->caMsg)+i*32,ret.at(i).toStdString().c_str(),ret.at(i).size());
        }   
        write((char*)respdu,respdu->uiPDULen);
        free(respdu);
        respdu=NULL;
        break;
    }
    case ENUM_MSG_TYPE_SEARCH_USR_REQUEST:{//如果是搜索指定用户的请求
        int ret = OpeDB::getInstance().handleSearchUsr(pdu->caData);
        PDU *respdu = mkPDU(0);
        respdu->uiMsgType = ENUM_MSG_TYPE_SEARCH_USR_RESPOND;
        if(ret == -1){
            strcpy(respdu->caData,SEARCH_USR_NO);
        }else if(ret == 1){
            strcpy(respdu->caData,SEARCH_USR_ONLINE);
        }else if(ret == 0){
            strcpy(respdu->caData,SEARCH_USR_OFFLINE);
        }
        write((char*)respdu,respdu->uiPDULen);
        free(respdu);
        respdu=NULL;
        break;
    }
    case ENUM_MSG_TYPE_ADD_FRIEND_REQUEST:{//如果是要添加好友的请求
        char caPerName[32]={'\0'};
        char caName[32]={'\0'};
        strncpy(caPerName,pdu->caData,32);
        strncpy(caName,pdu->caData+32,32);
        int ret = OpeDB::getInstance().handleAddFriend(caPerName,caName);
        PDU* respdu=mkPDU(0);
        if(ret == -1){
            respdu=mkPDU(0);
            respdu->uiMsgType=ENUM_MSG_TYPE_ADD_FRIEND_RESPOND;
            strcpy(respdu->caData,UNKNOW_ERROR);//未知错误
            write((char*)respdu,pdu->uiPDULen);
            free(respdu);
            respdu=NULL;
        }else if(ret == 0){
            respdu=mkPDU(0);
            respdu->uiMsgType=ENUM_MSG_TYPE_ADD_FRIEND_RESPOND;
            strcpy(respdu->caData,EXISTED_FRIEND);//已经是好友
            write((char*)respdu,pdu->uiPDULen);
            free(respdu);
            respdu=NULL;
        }else if(ret == 1){
            MyTcpServer::getInstance().resend(caPerName,pdu);
        }else if(ret == 2){
            respdu=mkPDU(0);
            respdu->uiMsgType=ENUM_MSG_TYPE_ADD_FRIEND_RESPOND;
            strcpy(respdu->caData,ADD_FRIEND_OFFLINE);//对方不在线
            write((char*)respdu,pdu->uiPDULen);
            free(respdu);
            respdu=NULL;
        }else if(ret == 3){
            respdu=mkPDU(0);
            respdu->uiMsgType=ENUM_MSG_TYPE_ADD_FRIEND_RESPOND;
            strcpy(respdu->caData,ADD_FRIEND_NO_EXIST);//对方不存在
            write((char*)respdu,pdu->uiPDULen);
            free(respdu);
            respdu=NULL;
        }
       
        break;
    }
    case ENUM_MSG_TYPE_ADD_FRIEND_AGGREE://如果是同意添加好友的请求
    {
        char caPerName[32] = {'\0'};
        char caName[32] = {'\0'};
        strncpy(caPerName,pdu->caData,32);
        strncpy(caName,pdu->caData+32,32);
        OpeDB::getInstance().handleAddFriend(caPerName,caName);//在数据库中进行添加好友操作
        MyTcpServer::getInstance().resend(caName,pdu);//将接受的数据进行转发
        OpeDB::getInstance().handleAgreeAddFriend(caPerName,caName);
        break;
    }
    case ENUM_MSG_TYPE_ADD_FRIEND_REFUSE://如果是拒绝添加好友的请求
    {
        char caName[32] = {'\0'};
        strncpy(caName,pdu->caData+32,32);
        MyTcpServer::getInstance().resend(caName,pdu);//转发
        break;
    }
    case ENUM_MSG_TYPE_FLUSH_FRIEND_REQUEST://收到刷新好友的请求
    {
        char caName[32] = {'\0'};
        strncpy(caName,pdu->caData,32);
        QStringList ret = OpeDB::getInstance().handleFLushFriend(caName);
        uint uiMsgLen = ret.size()*32;
        PDU* respdu = mkPDU(uiMsgLen);
        respdu->uiMsgType=ENUM_MSG_TYPE_FLUSH_FRIEND_RESPOND;
        for(int i=0;i<ret.size();i++){
            memcpy((char*)(respdu->caMsg)+i*32,ret.at(i).toStdString().c_str(),ret.at(i).size());
        }
        write((char*)respdu,respdu->uiPDULen);
        free(respdu);
        respdu=NULL;
        break;
    }
    case ENUM_MSG_TYPE_DELETE_FRIEND_REQUEST://收到删除好友的请求
    {
        char caSelfName[32]={'\0'};
        char caFriendName[32]={'\0'};
        strncpy(caSelfName,pdu->caData,32);
        strncpy(caFriendName,pdu->caData+32,32);
        OpeDB::getInstance(). handleDelFriend(caSelfName,caFriendName);
        PDU* respdu=mkPDU(0);
        respdu->uiMsgType = ENUM_MSG_TYPE_DELETE_FRIEND_RESPOND;//删除好友回复
        strcpy(respdu->caData,DELETE_FRIEND_OK);//删除好友成功
        write((char*)respdu,respdu->uiPDULen);
        free(respdu);
        pdu=NULL;
        
        MyTcpServer::getInstance().resend(caFriendName,pdu);
    }
    case ENUM_MSG_TYPE_PRIVATR_CHAT_REQUEST://服务器收到私聊的请求
    {
        char caPerName[32] = {'\0'};
        memcpy(caPerName,pdu->caData+32,32);
        MyTcpServer::getInstance().resend(caPerName,pdu);
        qDebug()<<caPerName;
        break;
    }
    case ENUM_MSG_TYPE_GROUND_CHAT_REQUEST://服务器收到群聊请求
    {
        char caName[32] = {'\0'};
        strncpy(caName,pdu->caData,32);
        QString temp;
        QStringList onlineFriend = OpeDB::getInstance().handleFLushFriend(caName);
        qDebug()<<(char*)(pdu->caMsg);
        for(int i=0;i<onlineFriend.size();i++){
            temp = onlineFriend.at(i);
            MyTcpServer::getInstance().resend(temp.toStdString().c_str(),pdu);
        }
        break;
    }
    case ENUM_MSG_TYPE_CREATE_DIR_REQUEST://服务器收到了创建文件夹的请求
    {
        qDebug()<<"服务器收到了创建文件夹的请求";
        QDir dir;
        QString strCurPath = QString("%1").arg((char*)(pdu->caMsg));
        bool ret = dir.exists(QString(strCurPath));//当前路径存在不存在
        PDU* respdu = NULL;
        if(ret){//当前目录存在
            qDebug()<<"当前目录存在";
            char caNewDir[32] = {'\0'};
            memcpy(caNewDir,pdu->caData+32,32);
            QString strNewPath = strCurPath+"/"+caNewDir;
            qDebug()<<strNewPath;
            ret = dir.exists(strNewPath);
            qDebug()<<"-->"<<ret;
            if(ret){//要创建的文件已经存在
                qDebug()<<"当前目录存在";
                respdu = mkPDU(0);
                respdu->uiMsgType = ENUM_MSG_TYPE_CREATE_DIR_RESPOND;
                strcpy(respdu->caData,FILE_NAME_EXIST);
            }else{//要创建的文件夹不存在，正好创建
                dir.mkdir(strNewPath);
                respdu = mkPDU(0);
                respdu->uiMsgType = ENUM_MSG_TYPE_CREATE_DIR_RESPOND;
                strcpy(respdu->caData,DIR_CREATE_OK);
            }
        }else{//当前目录不存在
            qDebug()<<"当前目录不存在";
            respdu = mkPDU(0);
            respdu->uiMsgType = ENUM_MSG_TYPE_CREATE_DIR_RESPOND;
            strcpy(respdu->caData,PATH_NO_EXIST);
        }
        write((char*)respdu,respdu->uiPDULen);
        free(respdu);
        respdu = NULL;
        break;
    }
    case ENUM_MSG_TYPE_FLUSH_FILE_REQUEST://刷新文件请求
    {
        char *pCurPath = new char[pdu->uiMsgLen];
        memcpy(pCurPath,pdu->caMsg,pdu->uiMsgLen);
        QDir dir(pCurPath);//这是他要操作的路径
        QFileInfoList fileInfoList = dir.entryInfoList();//遍历文件夹，不包含递归
        qDebug()<<"QFileInfoList:"<<fileInfoList;
        int iFileCount = fileInfoList.size();
        PDU* respdu = mkPDU(sizeof(FileInfo)*iFileCount);
        respdu->uiMsgType = ENUM_MSG_TYPE_FLUSH_FILE_RESPOND;
        FileInfo *pFileInfo = NULL;
        QString strFileName;
        for(int i=0;i<fileInfoList.size();i++){
            pFileInfo = (FileInfo*)(respdu->caMsg)+i;
            strFileName = fileInfoList[i].fileName().toLocal8Bit();
            qDebug()<<"strFileName："<<strFileName;
            memcpy(pFileInfo->caFileName,strFileName.toStdString().c_str(),strFileName.size()*3);
            if(fileInfoList[i].isDir()){
                pFileInfo->iFileType=0;//表示这是一个文件夹
            }else if(fileInfoList[i].isFile()){
                pFileInfo->iFileType=1;//表示这是一个常规文件
            }
            qDebug()<<pFileInfo->caFileName<<pFileInfo->iFileType;
        }
        write((char*)respdu,respdu->uiPDULen);
        free(respdu);
        delete []pCurPath;
        pCurPath = NULL;
        respdu = NULL;
        break;
    }
    case ENUM_MSG_TYPE_DEL_DIR_REQUEST://删除目录的请求
    {
        char caName[32] = {'\0'};
        strcpy(caName,pdu->caData);
        char* pPath = new char[pdu->uiMsgLen];
        memcpy(pPath,pdu->caMsg,pdu->uiMsgLen);
        QString strPath = QString("%1/%2").arg(pPath).arg(caName);
        qDebug()<<strPath;
        QFileInfo fileInfo(strPath);
        bool ret = false;
        PDU *respdu = NULL;
        if(fileInfo.isDir()){
            QDir dir;
            dir.setPath(strPath);
            ret = dir.removeRecursively();
        }else if(fileInfo.isFile()){
            QDir dir;
            ret = dir.remove(strPath);
        }
        if(ret){
            respdu = mkPDU(0);
            respdu->uiMsgType = ENUM_MSG_TYPE_DEL_DIR_RESPOND;
            memcpy(respdu->caData,DEL_OK,strlen(DEL_OK));
        }else{
            respdu = mkPDU(0);
            respdu->uiMsgType = ENUM_MSG_TYPE_DEL_DIR_RESPOND;
            memcpy(respdu->caData,DEL_FAILED,strlen(DEL_FAILED));
        }
        write((char*)respdu,respdu->uiPDULen);
        free(respdu);
        delete []pPath;
        pPath = NULL;
        respdu = NULL;
        break;
    }
    case ENUM_MSG_TYPE_RENAME_REQUEST://重命名文件的请求
    {
        char oldName[32] = {'\0'};
        char newName[32] = {'\0'};
        strncpy(oldName,pdu->caData,32);
        strncpy(newName,pdu->caData+32,32);
        char* pPath = new char[pdu->uiMsgLen];
        memcpy(pPath,pdu->caMsg,pdu->uiMsgLen);
        QString strOldPath = QString("%1/%2").arg(pPath).arg(oldName);
        QString strNewPath = QString("%1/%2").arg(pPath).arg(newName);
        qDebug()<<"strOldPath:"<<strOldPath;
        qDebug()<<"strNewPath:"<<strNewPath;
        QDir dir;
        bool ret = dir.rename(strOldPath,strNewPath);
        PDU* respdu = mkPDU(0);
        respdu->uiMsgType = ENUM_MSG_TYPE_RENAME_RESPOND;
        if(ret){
            strcpy(respdu->caData,RENAME_OK);
        }else{
            strcpy(respdu->caData,RENAME_FAILED);
        }
        write((char*)respdu,respdu->uiPDULen);
        free(respdu);
        delete []pPath;
        pPath = NULL;
        respdu = NULL;
        break;
    }
    case ENUM_MSG_TYPE_ENTER_FILE_REQUEST://进入文件夹请求
    {
        char fileName[32] = {'\0'};
        strncpy(fileName,pdu->caData,32);
        char* pPath = new char[pdu->uiMsgLen];
        memcpy(pPath,pdu->caMsg,pdu->uiMsgLen);
        QString strPath = QString("%1/%2").arg(pPath).arg(fileName);
        qDebug()<<"strPath:"<<strPath;
        QFileInfo fileInfo(strPath);
        PDU* respdu = NULL;
        if(fileInfo.isDir()){
            QDir dir(strPath);//这是他要操作的路径
            QFileInfoList fileInfoList = dir.entryInfoList();//遍历文件夹，不包含递归
            qDebug()<<"QFileInfoList:"<<fileInfoList;
            int iFileCount = fileInfoList.size();
            PDU* respdu = mkPDU(sizeof(FileInfo)*iFileCount);
            respdu->uiMsgType = ENUM_MSG_TYPE_FLUSH_FILE_RESPOND;
            FileInfo *pFileInfo = NULL;
            QString strFileName;
            for(int i=0;i<fileInfoList.size();i++){
                pFileInfo = (FileInfo*)(respdu->caMsg)+i;
                strFileName = fileInfoList[i].fileName().toLocal8Bit();
                qDebug()<<"strFileName："<<strFileName;
                memcpy(pFileInfo->caFileName,strFileName.toStdString().c_str(),strFileName.size()*3);
                if(fileInfoList[i].isDir()){
                    pFileInfo->iFileType=0;//表示这是一个文件夹
                }else if(fileInfoList[i].isFile()){
                    pFileInfo->iFileType=1;//表示这是一个常规文件
                }
                qDebug()<<pFileInfo->caFileName<<pFileInfo->iFileType;
            }
            write((char*)respdu,respdu->uiPDULen);
            free(respdu);
            respdu = NULL;
        }else{
            respdu = mkPDU(0);
            respdu->uiMsgType = ENUM_MSG_TYPE_ENTER_FILE_RESPOND;
            strcpy(respdu->caData,ENTER_DIR_FAILED);
            write((char*)respdu,respdu->uiPDULen);
            free(respdu);
            respdu=NULL;
        }
        delete []pPath;
        pPath = NULL;
        break;
    }
    case ENUM_MSG_TYPE_UPLOAD_FILE_REQUEST://上传文件的请求
    {
        char caFileName[32] = {'\0'};
        qint64 fileSize = 0;
        sscanf(pdu->caData,"%s %11d",caFileName,&fileSize);
        char* pPath = new char[pdu->uiMsgLen];
        memcpy(pPath,pdu->caMsg,pdu->uiMsgLen);
        QString strPath = QString("%1/%2").arg(pPath).arg(caFileName);
        qDebug()<<"strPath:"<<strPath;
        m_file.setFileName(strPath);//这个文件的对象现在操作这个路径
        if(m_file.open(QIODevice::WriteOnly)){//以只写的方式打开    如果这个文件不存在的话就会自动创建文件
            m_bUpLoad = true;
            m_iTotal = fileSize;
            m_iRecved = 0;
        }
        delete []pPath;
        pPath = NULL;
        break;
    }
    case ENUM_MSG_TYPE_DOWN_LOAD_REQUEST://下载文件的请求
    {
        char fileName[32] = {'\0'};
        strncpy(fileName,pdu->caData,32);
        char* pPath = new char[pdu->uiMsgLen];
        memcpy(pPath,pdu->caMsg,pdu->uiMsgLen);
        QString strPath = QString("%1/%2").arg(pPath).arg(fileName);
        qDebug()<<"strPath:"<<strPath;
        QFileInfo fileInfo(strPath);
        qint64 fileSize = fileInfo.size();
        PDU* respdu = mkPDU(0);
        respdu->uiMsgType = ENUM_MSG_TYPE_DOWN_LOAD_RESPOND;
        sprintf(respdu->caData,"%s %11d",fileName,fileSize);
        write((char*)respdu,respdu->uiPDULen);
        free(respdu);
        respdu=NULL;
        delete []pPath;
        pPath = NULL;
        m_file.setFileName(strPath);
        bool ret = m_file.open(QIODevice::ReadOnly);
        qDebug()<<"ret :"<<ret;
        m_pTimer->start(1000);
        break;
    }
    case ENUM_MSG_TYPE_SHARE_FILE_REQUEST://分享文件的请求
    {
        char caSendName[32] = {'\0'};
        int num=0;
        sscanf(pdu->caData,"%s %d",caSendName,&num);
        int size = num*32;
        PDU* respdu = mkPDU(pdu->uiMsgLen-size);
        respdu->uiMsgType = ENUM_MSG_TYPE_SHARE_FILE_NOTE;
        strcpy(respdu->caData,caSendName);//发送者的名字
        memcpy(respdu->caMsg,(char*)(pdu->caMsg)+size,pdu->uiMsgLen-size);
        char caRecvName[32] = {'\0'};
        for(int i=0;i<num;i++){
            memcpy(caRecvName,(char*)(pdu->caMsg)+i*32,32);
            MyTcpServer::getInstance().resend(caRecvName,respdu);
        }
        free(respdu); 
        respdu = NULL;
        respdu = mkPDU(0);
        respdu->uiMsgType = ENUM_MSG_TYPE_SHARE_FILE_RESPOND;
        strcpy(respdu->caData,"share file ok");
        write((char*)respdu,respdu->uiPDULen);
        free(respdu);
        respdu=NULL;
        break;
    }
    case ENUM_MSG_TYPE_SHARE_FILE_NOTE_RESPOND://服务器收到了有人同意共享文件的回复
    {
        qDebug()<<"有人同意了共享文件的回复";
        QString strRecvPath = QString("./%1").arg(pdu->caData);
        qDebug()<<"strRecvPath:"<<strRecvPath;
        QString strShareFilePath = QString("%1").arg((char*)pdu->caMsg);
        qDebug()<<"strShareFilePath"<<strShareFilePath;
        
        QFileInfo fileInfo(strShareFilePath);//对此路径上的文件进行操作
        int index = strShareFilePath.lastIndexOf('/');
        QString strFileName = strShareFilePath.right(strShareFilePath.size()-index-1);
        strRecvPath = strRecvPath+"/"+strFileName;
        qDebug()<<"strRecvPath:"<<strRecvPath;
        if(fileInfo.isFile()){
            qDebug()<<"这是一个常规文件";
            bool ret = QFile::copy(strShareFilePath,strRecvPath);
            qDebug()<<"共享完毕";
            if(ret){
                qDebug()<<"共享成功";
            }else{
                qDebug()<<"共享失败";
            }
            
        }else if(fileInfo.isDir()){
            qDebug()<<"这是一个dir";
            copyDir(strShareFilePath,strRecvPath);
            qDebug()<<"共享完毕";
        }
        break;
    }
     
    case ENUM_MSG_TYPE_MOVE_FILE_REQUEST://服务器收到了移动文件的请求
    {
        qDebug()<<"收到了移动文件的请求";
        char caFileName[32] = {'\0'};
        int srcLen = 0;
        int destLen = 0;
        sscanf(pdu->caData,"%d %d %s",&srcLen,&destLen,caFileName);
        qDebug()<<"strFileName："<<caFileName;
        char *pSrcPath = new char[srcLen+1];
        char *pDestPath = new char[destLen+1+32];
        memset(pSrcPath,'\0',srcLen+1);
        memset(pDestPath,'\0',destLen+1+32);
        memcpy(pSrcPath,pdu->caMsg,srcLen);
        memcpy(pDestPath,(char*)(pdu->caMsg)+(srcLen+1),srcLen);
        PDU* respdu = mkPDU(0);
        qDebug()<<"源文件夹："<<pSrcPath;
        qDebug()<<"目标文件夹："<<pDestPath;
        respdu->uiMsgType = ENUM_MSG_TYPE_MOVE_FILE_RESPOND;
        QFileInfo fileInfo(pDestPath);
        if(fileInfo.isDir()){
            strcat(pDestPath,"/");
            strcat(pDestPath,caFileName);
            qDebug()<<"目标文件夹："<<pDestPath;
            bool ret = QFile::rename(pSrcPath,pDestPath);
            if(ret){
                qDebug()<<"移动成功";
                strcpy(pdu->caData,MOVE_FILE_OK);
            }else{
                strcpy(pdu->caData,SYSTEM_ERROR);
                qDebug()<<"移动失败";
            }
        }else if(fileInfo.isFile()){
            strcpy(pdu->caData,MOVE_FILE_FAILED);
            qDebug()<<"目标文件夹为常规文件";
        }
        write((char*)respdu,respdu->uiPDULen);
        free(respdu);
        respdu=NULL;
        delete []pDestPath;
        pDestPath=NULL;
        delete []pSrcPath;
        pSrcPath=NULL;
        break;
    }
        
        
    default:
        break;
    }
    free(pdu);
    pdu=NULL;
    
   // qDebug()<<"用户名："<<caName<<";"<<"密码："<<caPwd<<";"<<"类型："<<pdu->uiMsgType;
    
}

void MyTcpSocket::clientOffline()
{
    OpeDB::getInstance().handleOffline(m_strName.toStdString().c_str());//函数要求char*类型，先将QString类型转换为c++的string类型，再获得它数据的地址，就转换成了char*类型
    emit offline(this);//发送信号
}

void MyTcpSocket::sendFileToClient()
{
    m_pTimer->stop();
    char *pData = new char[4096];
    qint64 ret = 0;
    while(true){
        ret = m_file.read(pData,4096);
        sleep(0.1);
        qDebug()<<"读取数据的大小："<<ret;
        if(ret >0 && ret <= 4096){
            write(pData,ret);
        }else if(0 == ret){
            m_file.close();
            qDebug()<<"文件发送完毕，马上退出";
            break;
        }else if(0 > ret){
            qDebug()<<"发送文件给客户端过程中出错,发送已经停止";
            m_file.close();
            break;
        }
    }
    qDebug()<<"已经退出循环发送";
    delete []pData;
}

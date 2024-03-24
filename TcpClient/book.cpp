#include "book.h"
#include "tcpclient.h"
#include <QInputDialog>
#include <QMessageBox>
#include <QDebug>
#include "opewidget.h"
#include "sharefile.h"

Book::Book(QWidget *parent) : QWidget(parent)
{
    downLoadState = false;
    m_strEnterDir.clear();
    m_pTimer = new QTimer;//定时器
    m_pBookListW = new QListWidget ;//文件列表
    m_pReturnPB = new QPushButton("返回") ;//返回按钮
    m_pCreateDirPB = new QPushButton("创建文件夹");//创建文件夹
    m_pDelDirPB = new QPushButton("删除文件");//删除文件夹
    m_pRenamePB = new QPushButton("重命名文件");//重命名文件
    m_pFlushPB = new QPushButton("刷新文件");//刷新
    m_pMoveFilePB = new QPushButton("移动文件");//移动文件
    
    QVBoxLayout *pDirVBL = new QVBoxLayout;//文件夹布局
    pDirVBL->addWidget(m_pReturnPB);
    pDirVBL->addWidget(m_pCreateDirPB);
    pDirVBL->addWidget(m_pRenamePB);
    pDirVBL->addWidget(m_pFlushPB);
    pDirVBL->addWidget(m_pDelDirPB);
    
    m_pUploadPB = new QPushButton("上传文件");//上传文件
    m_pDownLoadPB = new QPushButton("下载文件");//下载文件
    m_pSelectDirPB = new QPushButton("选择目标文件夹");//选择目标文件夹
    m_pSelectDirPB->setEnabled(false);//设置为不可用
    m_pShareFilePB = new QPushButton("分享文件");//分享文件
    
    QVBoxLayout *pFileVBL = new QVBoxLayout;
    pFileVBL->addWidget(m_pUploadPB);
    pFileVBL->addWidget(m_pDownLoadPB);
    pFileVBL->addWidget(m_pShareFilePB);
    pFileVBL->addWidget(m_pMoveFilePB);
    pFileVBL->addWidget(m_pSelectDirPB);
    
    QHBoxLayout *pMain = new QHBoxLayout;
    pMain->addWidget(m_pBookListW);
    pMain->addLayout(pDirVBL);
    pMain->addLayout(pFileVBL);
    
    setLayout(pMain);//将这个布局设置为窗口的总布局
    
    connect(m_pCreateDirPB,SIGNAL(clicked(bool)),this,SLOT(createDir()));
    connect(m_pFlushPB,SIGNAL(clicked(bool)),this,SLOT(flushFile()));
    connect(m_pDelDirPB,SIGNAL(clicked(bool)),this,SLOT(delDir()));
    connect(m_pRenamePB,SIGNAL(clicked(bool)),this,SLOT(reName()));
    connect(m_pBookListW,SIGNAL(doubleClicked(QModelIndex)),this,SLOT(enterDir(QModelIndex)));
    connect(m_pReturnPB,SIGNAL(clicked(bool)),this,SLOT(returnDir()));
    connect(m_pUploadPB,SIGNAL(clicked(bool)),this,SLOT(upLoadFile()));
    connect(m_pTimer,SIGNAL(timeout()),this,SLOT(upLoadFileData()));
    connect(m_pDownLoadPB,SIGNAL(clicked(bool)),this,SLOT(downLoadFile()));
    connect(m_pShareFilePB,SIGNAL(clicked(bool)),this,SLOT(shareFileUi()));
    connect(m_pMoveFilePB,SIGNAL(clicked(bool)),this,SLOT(moveShareFile()));
    connect(m_pSelectDirPB,SIGNAL(clicked(bool)),this,SLOT(selectDir()));
}

void Book::clearEnterDir()
{
    m_strEnterDir.clear();
}

QString Book::getEntDir()
{
    return m_strEnterDir;
}

QString Book::getSaveLoadPath()
{
    return strSaveLoadFilePath;
}

void Book::setDownFlag(bool state)
{
    downLoadState = state;
}

bool Book::getDownFlag()
{
    return downLoadState;
}

QString Book::getShareFileName()
{
    return m_strShareFileName;
}

void Book::createDir()//创建文件夹请求
{
    QString strNewDir = QInputDialog::getText(this,"新建文件夹","新文件夹名字");
    if(strNewDir == NULL){
        QMessageBox::warning(this,"新建文件夹","新建文件夹名字为空");
        return;
    }
    QString strName = TcpClient::getInstance().getname();
    QString strCurPath = TcpClient::getInstance().getCurPath();
    PDU* pdu = mkPDU(strCurPath.size()*3+1);
    pdu->uiMsgType = ENUM_MSG_TYPE_CREATE_DIR_REQUEST;
    strncpy(pdu->caData,strName.toStdString().c_str(),32);
    strncpy(pdu->caData+32,strNewDir.toStdString().c_str(),32);
    memcpy(pdu->caMsg,strCurPath.toStdString().c_str(),strCurPath.size());
    TcpClient::getInstance().getTcpSocket().write((char*)pdu,pdu->uiPDULen);
    free(pdu);
    pdu = NULL;
}

void Book::flushFile()//刷新文件请求
{
    QString strCurPath = TcpClient::getInstance().getCurPath();
    PDU* pdu = mkPDU(strCurPath.size()*3+1);
    pdu->uiMsgType = ENUM_MSG_TYPE_FLUSH_FILE_REQUEST;
    strncpy((char*)(pdu->caMsg),strCurPath.toStdString().c_str(),strCurPath.size());
    TcpClient::getInstance().getTcpSocket().write((char*)pdu,pdu->uiPDULen);
    free(pdu);
    pdu = NULL;
}

void Book::updateFileList(const PDU* respdu)//更新文件列表
{
    if(respdu == NULL){
        return;
    }
    QListWidgetItem * pItemTmp = NULL;
    int row = m_pBookListW->count();
    while(m_pBookListW->count()>0){
        pItemTmp = m_pBookListW->item(row-1);
        m_pBookListW->removeItemWidget(pItemTmp);
        delete pItemTmp;
        row = row-1;
    }
    //m_pBookListW->clear();

    FileInfo *pFileInfo = NULL;
    int iCount = respdu->uiMsgLen/sizeof(FileInfo);
    for(int i=0;i<iCount;i++){
        
        pFileInfo = (FileInfo*)(respdu->caMsg)+i;
        qDebug()<<pFileInfo->caFileName<<pFileInfo->iFileType;
        if(pFileInfo->caFileName == QString(".") || pFileInfo->caFileName == QString("..")){
            continue;
        }
        QListWidgetItem *pItem = new QListWidgetItem;
        if(0 == pFileInfo->iFileType){
            pItem->setIcon(QIcon(QPixmap(":/dir.jpg")));
        }else if(1 == pFileInfo->iFileType){
            pItem->setIcon(QIcon(QPixmap(":/reg.jpg")));
        }
        pItem->setText(pFileInfo->caFileName);
        m_pBookListW->addItem(pItem);
    }
}

void Book::delDir()//删除文件夹
{
    QString strCurPath = TcpClient::getInstance().getCurPath();
    QListWidgetItem *pItem = m_pBookListW->currentItem();
    if(pItem == NULL){
        QMessageBox::warning(this,"删除文件","请选择要删除的文件");
    }else{
        QString strDelName = pItem->text();
        qDebug()<<"strDelName:"<<strDelName;
        PDU* pdu = mkPDU(strCurPath.size()*3+1);
        pdu->uiMsgType = ENUM_MSG_TYPE_DEL_DIR_REQUEST;
        strncpy(pdu->caData,strDelName.toStdString().c_str(),strDelName.size()*3);
        qDebug()<<"pdu->cadata:"<<pdu->caData;
        memcpy(pdu->caMsg,strCurPath.toStdString().c_str(),strCurPath.size());
        TcpClient::getInstance().getTcpSocket().write((char*)pdu,pdu->uiPDULen);
        free(pdu);
        pdu = NULL;
    }
}

void Book::reName()//重命名文件
{
    QString strCurPath = TcpClient::getInstance().getCurPath();//当前路径
    QListWidgetItem *pItem = m_pBookListW->currentItem();
    if(pItem == NULL){
        QMessageBox::warning(this,"重命名","请选择要重命名的文件");
    }else{
        QString strOldName = pItem->text();
        qDebug()<<"strOldName:"<<strOldName;
        QString strNewName = QInputDialog::getText(this,"重命名文件","请输入新的文件名：");
        qDebug()<<"strNewName:"<<strNewName;
        if(strNewName.isEmpty()){
            QMessageBox::warning(this,"重命名文件","重命名文件名为空");
        }
        PDU* pdu = mkPDU(strCurPath.size()*3+1);
        pdu->uiMsgType = ENUM_MSG_TYPE_RENAME_REQUEST;
        strncpy(pdu->caData,strOldName.toStdString().c_str(),strOldName.size()*3);
        strncpy(pdu->caData+32,strNewName.toStdString().c_str(),strNewName.size()*3);
        qDebug()<<"pdu->cadata:"<<pdu->caData;
        memcpy(pdu->caMsg,strCurPath.toStdString().c_str(),strCurPath.size());
        TcpClient::getInstance().getTcpSocket().write((char*)pdu,pdu->uiPDULen);
        free(pdu);
        pdu = NULL;
    }
}

void Book::enterDir(const QModelIndex &index)//进入文件夹
{
    QString strDirName = index.data().toString();
    qDebug()<<strDirName;
    
    QString strCurPath = TcpClient::getInstance().getCurPath();
    m_strEnterDir = QString("%1/%2").arg(strCurPath).arg(strDirName);
    qDebug()<<"进入成功之后的路径："<<m_strEnterDir;
    PDU* pdu = mkPDU(strCurPath.size()*3);
    pdu->uiMsgType = ENUM_MSG_TYPE_ENTER_FILE_REQUEST;
    strncpy(pdu->caData,strDirName.toStdString().c_str(),strDirName.size()*3);
    qDebug()<<"pdu->caData:"<<pdu->caData;
    memcpy(pdu->caMsg,strCurPath.toStdString().c_str(),strCurPath.size()*3);
    TcpClient::getInstance().getTcpSocket().write((char*)pdu,pdu->uiPDULen);
    free(pdu);
    pdu = NULL;
}

void Book::returnDir()//返回上一级目录
{
    QString strCurPath = TcpClient::getInstance().getCurPath();
    QString strRootPath = "./" + TcpClient::getInstance().getname();
    if(strCurPath == strRootPath){
        QMessageBox::information(this,"返回上一级目录","已经在家目录中");
    }else{
        qDebug()<<"当前所在目录："<<strCurPath;
        int index = strCurPath.lastIndexOf('/');//从后面开始查找，查找第一个‘/’
        strCurPath.remove(index,strCurPath.size()-index);
        qDebug()<<"上一级路径："<<strCurPath;
        TcpClient::getInstance().setM_strCurPath(strCurPath);
        m_strEnterDir.clear();
        m_strEnterDir = strCurPath;
        flushFile();
    }
}

void Book::upLoadFile()//上传文案请求
{
    qDebug()<<"进入上传文件的函数：";
    QString strCurPath = TcpClient::getInstance().getCurPath();
    strUpLoadFilePath = QFileDialog::getOpenFileName();//获取一个文件的路径和名称
    qDebug()<<"strCurPath:"<<strCurPath;
    qDebug()<<"strUpLoadFilePath:"<<strUpLoadFilePath;
    if(!strUpLoadFilePath.isEmpty()){
        int index = strUpLoadFilePath.lastIndexOf('/');
        QString strFileName = strUpLoadFilePath.right(strUpLoadFilePath.size()-index-1);
        qDebug()<<"strFileName:"<<strFileName;
        QFile file(strUpLoadFilePath);
        qint64 fileSize = file.size();//获得这个文件的大小
        PDU* pdu = mkPDU(strCurPath.size()*3+1);
        pdu->uiMsgType = ENUM_MSG_TYPE_UPLOAD_FILE_REQUEST;
        memcpy(pdu->caMsg,strCurPath.toStdString().c_str(),strCurPath.size()*3);
        sprintf(pdu->caData,"%s %11d",strFileName.toStdString().c_str(),fileSize);
        TcpClient::getInstance().getTcpSocket().write((char*)pdu,pdu->uiPDULen);
        qDebug()<<"上传文件请求已经发送";
        free(pdu);
        pdu = NULL;//先发送请求之后，等待1000毫秒，再上传文件
        
        m_pTimer->start(1000);//为了防止二进制文件和包被服务器一块接收
    }else{
        QMessageBox::warning(this,"上传文件","上传文件不能为空");
        return;
    }
}

void Book::upLoadFileData()//上传文件内容
{
    m_pTimer->stop();
    QFile file(strUpLoadFilePath);
    if(!file.open(QIODevice::ReadOnly)){
        QMessageBox::warning(this,"上传文件","打开失败");
        return;
    }
    char *pBuffer = new char[4096];
    qint64 ret = 0;
    while(true){
        ret = file.read(pBuffer,4096);//读到pbuffer里面去，读4096个字节
        if(ret >0 && ret <= 4096){
            TcpClient::getInstance().getTcpSocket().write(pBuffer,ret);
        }else if(ret == 0){//读完了
            break;
        }else{
            QMessageBox::warning(this,"上传文件","上传失败：文件读取失败");
            break;
        }
    }
    file.close();
    delete []pBuffer;
    pBuffer = NULL;
}

void Book::downLoadFile()
{
    
    QListWidgetItem *pItem = m_pBookListW->currentItem();
    QString strSaveFilePath = QFileDialog::getSaveFileName();
    if(strSaveFilePath.isEmpty()){
        QMessageBox::information(this,"下载文件","请指定要保存的位置");
        strSaveFilePath.clear();
    }else{
        strSaveLoadFilePath = strSaveFilePath;
    }
    if(pItem == NULL){
        QMessageBox::warning(this,"下载文件","请选择要下载的文件");
    }else{

        QString strCurPath = TcpClient::getInstance().getCurPath();
        PDU* pdu = mkPDU(strCurPath.size()*3+1);
        pdu->uiMsgType = ENUM_MSG_TYPE_DOWN_LOAD_REQUEST;
        QString strFileName = pItem->text();
        strcpy(pdu->caData,strFileName.toStdString().c_str());
        memcpy(pdu->caMsg,strCurPath.toStdString().c_str(),strCurPath.size()*3);
        TcpClient::getInstance().getTcpSocket().write((char*)pdu,pdu->uiPDULen);
        free(pdu);
        pdu = NULL;
    }
    
    
}

void Book::shareFileUi()
{
    qDebug()<<"进入了分享文件函数";
    QListWidgetItem *pItem = m_pBookListW->currentItem();
    if(pItem == NULL){
        QMessageBox::warning(this,"分享文件","请选择要分享的文件");
        return;
    }else{
        m_strShareFileName = pItem->text();
    }
    Friend* pFriend = OpeWidget::getInstance().getFriend();//先获得这个对象
    pFriend->flushFriend();
    QListWidget *pFriendList = pFriend->getFriendList();
    if(shareFile::getInstance().isHidden()){
        shareFile::getInstance().show();
    }
    shareFile::getInstance().updateFriend(pFriendList);
    
}

void Book::moveShareFile()
{
    QListWidgetItem *pCurItem = m_pBookListW->currentItem();
    if(pCurItem == NULL){
        QMessageBox::warning(this,"移动文件","请选择要移动的文件:");
        return;
    }
    m_strMoveFileName = pCurItem->text();//获得文件名
    QString strCurPath = TcpClient::getInstance().getCurPath();//获得当前路径
    m_strMoveFilePath = strCurPath+"/"+m_strMoveFileName;//要移动的文件名字和他的路径
    m_pSelectDirPB->setEnabled(true);
}

void Book::selectDir()
{
    QListWidgetItem *pCurItem = m_pBookListW->currentItem();
    if(pCurItem == NULL){
        QMessageBox::warning(this,"移动文件","请选择目标文件夹:");
        return;
    }
    QString strDestDir = pCurItem->text();//获得文件名
    qDebug()<<"获得文件名QString ："<<strDestDir;
    QString strCurPath = TcpClient::getInstance().getCurPath();//获得当前路径
    m_strSelectDirPath = strCurPath+"/"+strDestDir;//要移动的文件名字和他的路径
    int srcLen = m_strMoveFilePath.size()*3;
    int destLen = m_strSelectDirPath.size()*3;
    PDU* pdu = mkPDU(srcLen+ destLen+2);
    pdu->uiMsgType = ENUM_MSG_TYPE_MOVE_FILE_REQUEST;
    sprintf(pdu->caData,"%d %d %s",srcLen,destLen,m_strMoveFileName);
    printf("文件名：%s",m_strMoveFileName);
    qDebug()<<"pdu->caData:"<<pdu->caData;
    memcpy((char*)pdu->caMsg,m_strMoveFilePath.toStdString().c_str(),srcLen);
    memcpy((char*)(pdu->caMsg)+srcLen+1,m_strSelectDirPath.toStdString().c_str(),destLen);
    
    TcpClient::getInstance().getTcpSocket().write((char*)pdu,pdu->uiPDULen);
    free(pdu);
    pdu=NULL;
    qDebug()<<"成功发送";
    m_pSelectDirPB->setEnabled(false);
}

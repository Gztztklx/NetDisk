#ifndef BOOK_H
#define BOOK_H

#include <QWidget>
#include <QListWidget>
#include <QPushButton>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QFileDialog>
#include <protocol.h>
#include <QTimer>


class Book : public QWidget
{
    Q_OBJECT
public:
    explicit Book(QWidget *parent = nullptr);
    void clearEnterDir();
    QString getEntDir();
    QString getSaveLoadPath();
    void setDownFlag(bool state);
    bool getDownFlag();
    
    qint64 m_iTotal;//要下载文件总的大小
    qint64 m_iRecved;//已经下载了多少个字节
    QString getShareFileName();//获得要分享的文件名
    
    
signals:
    
public slots:
    void createDir();
    void flushFile();
    void updateFileList(const PDU* respdu);
    void delDir();
    void reName();
    void enterDir(const QModelIndex &index);
    void returnDir();
    void upLoadFile();
    void upLoadFileData();
    void downLoadFile();
    void shareFileUi();
    void moveShareFile();
    void selectDir();
    
private:
    QListWidget *m_pBookListW;//文件列表
    QPushButton *m_pReturnPB;//返回按钮
    QPushButton *m_pCreateDirPB;//创建文件夹
    QPushButton *m_pDelDirPB;//删除文件夹
    QPushButton *m_pRenamePB;//重命名文件
    QPushButton *m_pFlushPB;//刷新
    QPushButton *m_pUploadPB;//上传文件
    QPushButton *m_pDownLoadPB;//下载文件
    QPushButton *m_pDelFilePB;//删除文件
    QPushButton *m_pShareFilePB;//分享文件
    QPushButton *m_pMoveFilePB;//移动文件
    QPushButton *m_pSelectDirPB;//选择目标文件夹
    
    QString m_strEnterDir;//当前所处路径
    QString strUpLoadFilePath;//要上传文件的路径
    QTimer *m_pTimer;
    QString strSaveLoadFilePath;//下载文件时文件保存在这里
    bool downLoadState;
    QString m_strShareFileName;//要分享的文件名
    QString m_strMoveFileName;//要移动的文件名
    QString m_strMoveFilePath;//移动文件时源文件路径（带文件名）
    QString m_strSelectDirPath;//移动文件时，目标目录路径（带目录名）
    
};

#endif // BOOK_H

#include "tcpclient.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    
    TcpClient::getInstance().show();
    qDebug()<<"1:"<<sizeof(ENUM_MSG_TYPE);

    return a.exec();
}

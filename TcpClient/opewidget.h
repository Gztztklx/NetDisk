#ifndef OPEWIDGET_H
#define OPEWIDGET_H
#include "friend.h"
#include "book.h"
#include <QWidget>
#include <QListWidget>
#include <QStackedWidget>//堆上窗口


class OpeWidget:public QWidget
{
    Q_OBJECT
public:
    OpeWidget(QWidget* parent=0);
    static OpeWidget& getInstance();
    Friend *getFriend();
    Book *getBook();
signals:
    
public slots:
    
private:
    QListWidget* m_pListW;
    Friend* m_pFriend;
    Book* m_pBook;
    
    QStackedWidget* m_pSW;
};

#endif // OPEWIDGET_H

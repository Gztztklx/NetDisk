/********************************************************************************
** Form generated from reading UI file 'privatrchat.ui'
**
** Created by: Qt User Interface Compiler version 6.5.2
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_PRIVATRCHAT_H
#define UI_PRIVATRCHAT_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QTextEdit>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_Privatrchat
{
public:
    QHBoxLayout *horizontalLayout_2;
    QVBoxLayout *verticalLayout;
    QTextEdit *showMsg_te;
    QHBoxLayout *horizontalLayout;
    QLineEdit *inputNsg_le;
    QPushButton *sendMsg_pb;

    void setupUi(QWidget *Privatrchat)
    {
        if (Privatrchat->objectName().isEmpty())
            Privatrchat->setObjectName("Privatrchat");
        Privatrchat->resize(400, 300);
        horizontalLayout_2 = new QHBoxLayout(Privatrchat);
        horizontalLayout_2->setObjectName("horizontalLayout_2");
        verticalLayout = new QVBoxLayout();
        verticalLayout->setObjectName("verticalLayout");
        showMsg_te = new QTextEdit(Privatrchat);
        showMsg_te->setObjectName("showMsg_te");
        QFont font;
        font.setFamilies({QString::fromUtf8("Arial")});
        font.setPointSize(14);
        showMsg_te->setFont(font);

        verticalLayout->addWidget(showMsg_te);

        horizontalLayout = new QHBoxLayout();
        horizontalLayout->setObjectName("horizontalLayout");
        inputNsg_le = new QLineEdit(Privatrchat);
        inputNsg_le->setObjectName("inputNsg_le");
        inputNsg_le->setFont(font);

        horizontalLayout->addWidget(inputNsg_le);

        sendMsg_pb = new QPushButton(Privatrchat);
        sendMsg_pb->setObjectName("sendMsg_pb");
        sendMsg_pb->setFont(font);

        horizontalLayout->addWidget(sendMsg_pb);


        verticalLayout->addLayout(horizontalLayout);


        horizontalLayout_2->addLayout(verticalLayout);


        retranslateUi(Privatrchat);

        QMetaObject::connectSlotsByName(Privatrchat);
    } // setupUi

    void retranslateUi(QWidget *Privatrchat)
    {
        Privatrchat->setWindowTitle(QCoreApplication::translate("Privatrchat", "Form", nullptr));
        sendMsg_pb->setText(QCoreApplication::translate("Privatrchat", "\345\217\221\351\200\201", nullptr));
    } // retranslateUi

};

namespace Ui {
    class Privatrchat: public Ui_Privatrchat {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_PRIVATRCHAT_H

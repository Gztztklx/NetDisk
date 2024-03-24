/********************************************************************************
** Form generated from reading UI file 'tcpclient.ui'
**
** Created by: Qt User Interface Compiler version 6.5.2
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_TCPCLIENT_H
#define UI_TCPCLIENT_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QGridLayout>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QSpacerItem>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_TcpClient
{
public:
    QGridLayout *gridLayout_2;
    QGridLayout *gridLayout;
    QLabel *pwd_lab;
    QPushButton *login_pb;
    QLabel *name_lab;
    QPushButton *regist_pb;
    QSpacerItem *horizontalSpacer;
    QPushButton *cancel_pb;
    QLineEdit *name_le;
    QLineEdit *pwd_le;

    void setupUi(QWidget *TcpClient)
    {
        if (TcpClient->objectName().isEmpty())
            TcpClient->setObjectName("TcpClient");
        TcpClient->resize(322, 155);
        gridLayout_2 = new QGridLayout(TcpClient);
        gridLayout_2->setSpacing(6);
        gridLayout_2->setContentsMargins(11, 11, 11, 11);
        gridLayout_2->setObjectName("gridLayout_2");
        gridLayout = new QGridLayout();
        gridLayout->setSpacing(6);
        gridLayout->setObjectName("gridLayout");
        pwd_lab = new QLabel(TcpClient);
        pwd_lab->setObjectName("pwd_lab");
        QFont font;
        font.setFamilies({QString::fromUtf8("Arial")});
        font.setPointSize(10);
        pwd_lab->setFont(font);

        gridLayout->addWidget(pwd_lab, 1, 0, 1, 1);

        login_pb = new QPushButton(TcpClient);
        login_pb->setObjectName("login_pb");
        QFont font1;
        font1.setFamilies({QString::fromUtf8("Arial")});
        font1.setPointSize(22);
        login_pb->setFont(font1);

        gridLayout->addWidget(login_pb, 2, 0, 1, 4);

        name_lab = new QLabel(TcpClient);
        name_lab->setObjectName("name_lab");
        name_lab->setFont(font);

        gridLayout->addWidget(name_lab, 0, 0, 1, 1);

        regist_pb = new QPushButton(TcpClient);
        regist_pb->setObjectName("regist_pb");

        gridLayout->addWidget(regist_pb, 3, 0, 1, 1);

        horizontalSpacer = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        gridLayout->addItem(horizontalSpacer, 3, 1, 1, 1);

        cancel_pb = new QPushButton(TcpClient);
        cancel_pb->setObjectName("cancel_pb");

        gridLayout->addWidget(cancel_pb, 3, 3, 1, 1);

        name_le = new QLineEdit(TcpClient);
        name_le->setObjectName("name_le");

        gridLayout->addWidget(name_le, 0, 1, 1, 3);

        pwd_le = new QLineEdit(TcpClient);
        pwd_le->setObjectName("pwd_le");
        pwd_le->setEchoMode(QLineEdit::Password);

        gridLayout->addWidget(pwd_le, 1, 1, 1, 3);


        gridLayout_2->addLayout(gridLayout, 0, 0, 1, 1);


        retranslateUi(TcpClient);

        QMetaObject::connectSlotsByName(TcpClient);
    } // setupUi

    void retranslateUi(QWidget *TcpClient)
    {
        TcpClient->setWindowTitle(QCoreApplication::translate("TcpClient", "TcpClient", nullptr));
        pwd_lab->setText(QCoreApplication::translate("TcpClient", "\345\257\206\347\240\201\357\274\232", nullptr));
        login_pb->setText(QCoreApplication::translate("TcpClient", "\347\231\273\345\275\225", nullptr));
        name_lab->setText(QCoreApplication::translate("TcpClient", "\347\224\250\346\210\267\345\220\215\357\274\232", nullptr));
        regist_pb->setText(QCoreApplication::translate("TcpClient", "\346\263\250\345\206\214", nullptr));
        cancel_pb->setText(QCoreApplication::translate("TcpClient", "\346\263\250\351\224\200", nullptr));
    } // retranslateUi

};

namespace Ui {
    class TcpClient: public Ui_TcpClient {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_TCPCLIENT_H

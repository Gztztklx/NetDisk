/********************************************************************************
** Form generated from reading UI file 'online.ui'
**
** Created by: Qt User Interface Compiler version 6.5.2
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_ONLINE_H
#define UI_ONLINE_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QListWidget>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QSpacerItem>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_Online
{
public:
    QHBoxLayout *horizontalLayout_2;
    QHBoxLayout *horizontalLayout;
    QListWidget *online_hw;
    QVBoxLayout *verticalLayout;
    QSpacerItem *verticalSpacer;
    QPushButton *addFriend_pb;
    QSpacerItem *verticalSpacer_2;

    void setupUi(QWidget *Online)
    {
        if (Online->objectName().isEmpty())
            Online->setObjectName("Online");
        Online->resize(448, 381);
        horizontalLayout_2 = new QHBoxLayout(Online);
        horizontalLayout_2->setObjectName("horizontalLayout_2");
        horizontalLayout = new QHBoxLayout();
        horizontalLayout->setObjectName("horizontalLayout");
        online_hw = new QListWidget(Online);
        new QListWidgetItem(online_hw);
        new QListWidgetItem(online_hw);
        new QListWidgetItem(online_hw);
        new QListWidgetItem(online_hw);
        new QListWidgetItem(online_hw);
        new QListWidgetItem(online_hw);
        online_hw->setObjectName("online_hw");
        QFont font;
        font.setPointSize(16);
        online_hw->setFont(font);

        horizontalLayout->addWidget(online_hw);

        verticalLayout = new QVBoxLayout();
        verticalLayout->setObjectName("verticalLayout");
        verticalSpacer = new QSpacerItem(20, 40, QSizePolicy::Minimum, QSizePolicy::Expanding);

        verticalLayout->addItem(verticalSpacer);

        addFriend_pb = new QPushButton(Online);
        addFriend_pb->setObjectName("addFriend_pb");
        QFont font1;
        font1.setFamilies({QString::fromUtf8("Arial")});
        font1.setPointSize(24);
        addFriend_pb->setFont(font1);

        verticalLayout->addWidget(addFriend_pb);

        verticalSpacer_2 = new QSpacerItem(20, 40, QSizePolicy::Minimum, QSizePolicy::Expanding);

        verticalLayout->addItem(verticalSpacer_2);


        horizontalLayout->addLayout(verticalLayout);


        horizontalLayout_2->addLayout(horizontalLayout);


        retranslateUi(Online);

        QMetaObject::connectSlotsByName(Online);
    } // setupUi

    void retranslateUi(QWidget *Online)
    {
        Online->setWindowTitle(QCoreApplication::translate("Online", "Form", nullptr));

        const bool __sortingEnabled = online_hw->isSortingEnabled();
        online_hw->setSortingEnabled(false);
        QListWidgetItem *___qlistwidgetitem = online_hw->item(0);
        ___qlistwidgetitem->setText(QCoreApplication::translate("Online", "11111", nullptr));
        QListWidgetItem *___qlistwidgetitem1 = online_hw->item(1);
        ___qlistwidgetitem1->setText(QCoreApplication::translate("Online", "2222", nullptr));
        QListWidgetItem *___qlistwidgetitem2 = online_hw->item(2);
        ___qlistwidgetitem2->setText(QCoreApplication::translate("Online", "3333", nullptr));
        QListWidgetItem *___qlistwidgetitem3 = online_hw->item(3);
        ___qlistwidgetitem3->setText(QCoreApplication::translate("Online", "4444", nullptr));
        QListWidgetItem *___qlistwidgetitem4 = online_hw->item(4);
        ___qlistwidgetitem4->setText(QCoreApplication::translate("Online", "5555", nullptr));
        QListWidgetItem *___qlistwidgetitem5 = online_hw->item(5);
        ___qlistwidgetitem5->setText(QCoreApplication::translate("Online", "6666", nullptr));
        online_hw->setSortingEnabled(__sortingEnabled);

        addFriend_pb->setText(QCoreApplication::translate("Online", "\345\212\240\345\245\275\345\217\213", nullptr));
    } // retranslateUi

};

namespace Ui {
    class Online: public Ui_Online {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_ONLINE_H

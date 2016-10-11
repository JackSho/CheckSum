/********************************************************************************
** Form generated from reading UI file 'checksum.ui'
**
** Created by: Qt User Interface Compiler version 5.2.1
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_CHECKSUM_H
#define UI_CHECKSUM_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QCheckBox>
#include <QtWidgets/QGridLayout>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QSpacerItem>
#include <QtWidgets/QSplitter>
#include <QtWidgets/QTableView>
#include <QtWidgets/QTreeView>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>
#include "MoveLabel.h"

QT_BEGIN_NAMESPACE

class Ui_CheckSumClass
{
public:
    QWidget *centralWidget;
    QGridLayout *gridLayout;
    QSplitter *splitter;
    QWidget *widget_Left;
    QVBoxLayout *verticalLayout_2;
    QTreeView *treeView_explorer;
    QPushButton *pushButton_refresh;
    QWidget *layoutWidget;
    QGridLayout *gridLayout_Right;
    QLineEdit *lineEdit_fileName;
    QVBoxLayout *verticalLayout;
    QWidget *widget_icon;
    QGridLayout *gridLayout_2;
    QWidget *widget_BigIcon;
    QGridLayout *gridLayout_3;
    MoveLabel *label_NoIcon;
    QPushButton *pushButton_add;
    QSpacerItem *verticalSpacer_2;
    QPushButton *pushButton_delete;
    QSpacerItem *verticalSpacer_3;
    QPushButton *pushButton_deleteAll;
    QSpacerItem *verticalSpacer_4;
    QPushButton *pushButton_hideShowLeft;
    QSpacerItem *verticalSpacer_5;
    MoveLabel *label_Percent;
    QHBoxLayout *horizontalLayout;
    QPushButton *pushButton_refreshAll;
    QPushButton *pushButton_refreshSelected;
    QPushButton *pushButton_exit;
    QLineEdit *lineEdit_MD5;
    QHBoxLayout *horizontalLayout_2;
    MoveLabel *label_3;
    QLineEdit *lineEdit_checkSum;
    MoveLabel *label_4;
    QLineEdit *lineEdit_CRC32;
    MoveLabel *label_6;
    QLineEdit *lineEdit_Size;
    QCheckBox *checkBox_autoWidth;
    MoveLabel *label_2;
    QLineEdit *lineEdit_SHA1;
    MoveLabel *label;
    QTableView *tableView_records;
    MoveLabel *label_5;

    void setupUi(QMainWindow *CheckSumClass)
    {
        if (CheckSumClass->objectName().isEmpty())
            CheckSumClass->setObjectName(QStringLiteral("CheckSumClass"));
        CheckSumClass->resize(892, 600);
        QFont font;
        font.setFamily(QString::fromUtf8("\345\276\256\350\275\257\351\233\205\351\273\221"));
        font.setPointSize(11);
        CheckSumClass->setFont(font);
        CheckSumClass->setStyleSheet(QLatin1String("QTableView{\n"
"	alternate-background-color:rgb(221, 238, 255);\n"
"}\n"
"/*\n"
"QTableView::item::hover{\n"
"	background-color:red;\n"
"	font-weight:bold;\n"
"	color:red;\n"
"}*/"));
        centralWidget = new QWidget(CheckSumClass);
        centralWidget->setObjectName(QStringLiteral("centralWidget"));
        gridLayout = new QGridLayout(centralWidget);
        gridLayout->setSpacing(6);
        gridLayout->setContentsMargins(11, 11, 11, 11);
        gridLayout->setObjectName(QStringLiteral("gridLayout"));
        splitter = new QSplitter(centralWidget);
        splitter->setObjectName(QStringLiteral("splitter"));
        splitter->setFont(font);
        splitter->setOrientation(Qt::Horizontal);
        widget_Left = new QWidget(splitter);
        widget_Left->setObjectName(QStringLiteral("widget_Left"));
        widget_Left->setFont(font);
        verticalLayout_2 = new QVBoxLayout(widget_Left);
        verticalLayout_2->setSpacing(6);
        verticalLayout_2->setContentsMargins(11, 11, 11, 11);
        verticalLayout_2->setObjectName(QStringLiteral("verticalLayout_2"));
        treeView_explorer = new QTreeView(widget_Left);
        treeView_explorer->setObjectName(QStringLiteral("treeView_explorer"));
        QFont font1;
        font1.setFamily(QString::fromUtf8("\345\276\256\350\275\257\351\233\205\351\273\221"));
        font1.setPointSize(11);
        font1.setKerning(true);
        treeView_explorer->setFont(font1);
        treeView_explorer->setFocusPolicy(Qt::StrongFocus);

        verticalLayout_2->addWidget(treeView_explorer);

        pushButton_refresh = new QPushButton(widget_Left);
        pushButton_refresh->setObjectName(QStringLiteral("pushButton_refresh"));
        pushButton_refresh->setFont(font1);
        pushButton_refresh->setFocusPolicy(Qt::StrongFocus);

        verticalLayout_2->addWidget(pushButton_refresh);

        splitter->addWidget(widget_Left);
        layoutWidget = new QWidget(splitter);
        layoutWidget->setObjectName(QStringLiteral("layoutWidget"));
        layoutWidget->setFont(font);
        gridLayout_Right = new QGridLayout(layoutWidget);
        gridLayout_Right->setSpacing(6);
        gridLayout_Right->setContentsMargins(11, 11, 11, 11);
        gridLayout_Right->setObjectName(QStringLiteral("gridLayout_Right"));
        gridLayout_Right->setContentsMargins(0, 0, 0, 0);
        lineEdit_fileName = new QLineEdit(layoutWidget);
        lineEdit_fileName->setObjectName(QStringLiteral("lineEdit_fileName"));
        lineEdit_fileName->setFont(font1);
        lineEdit_fileName->setFocusPolicy(Qt::StrongFocus);

        gridLayout_Right->addWidget(lineEdit_fileName, 0, 2, 1, 1);

        verticalLayout = new QVBoxLayout();
        verticalLayout->setSpacing(6);
        verticalLayout->setObjectName(QStringLiteral("verticalLayout"));
        widget_icon = new QWidget(layoutWidget);
        widget_icon->setObjectName(QStringLiteral("widget_icon"));
        widget_icon->setMinimumSize(QSize(32, 32));
        widget_icon->setFont(font);
        widget_icon->setStyleSheet(QLatin1String("#widget_icon{\n"
"border:2px dashed;\n"
"}"));
        gridLayout_2 = new QGridLayout(widget_icon);
        gridLayout_2->setSpacing(6);
        gridLayout_2->setContentsMargins(11, 11, 11, 11);
        gridLayout_2->setObjectName(QStringLiteral("gridLayout_2"));
        widget_BigIcon = new QWidget(widget_icon);
        widget_BigIcon->setObjectName(QStringLiteral("widget_BigIcon"));
        widget_BigIcon->setFont(font);
        gridLayout_3 = new QGridLayout(widget_BigIcon);
        gridLayout_3->setSpacing(6);
        gridLayout_3->setContentsMargins(11, 11, 11, 11);
        gridLayout_3->setObjectName(QStringLiteral("gridLayout_3"));
        label_NoIcon = new MoveLabel(widget_BigIcon);
        label_NoIcon->setObjectName(QStringLiteral("label_NoIcon"));
        label_NoIcon->setFont(font);
        label_NoIcon->setAlignment(Qt::AlignCenter);

        gridLayout_3->addWidget(label_NoIcon, 0, 0, 1, 1);


        gridLayout_2->addWidget(widget_BigIcon, 0, 0, 1, 1);


        verticalLayout->addWidget(widget_icon);

        pushButton_add = new QPushButton(layoutWidget);
        pushButton_add->setObjectName(QStringLiteral("pushButton_add"));
        pushButton_add->setFont(font1);
        pushButton_add->setFocusPolicy(Qt::StrongFocus);

        verticalLayout->addWidget(pushButton_add);

        verticalSpacer_2 = new QSpacerItem(20, 40, QSizePolicy::Minimum, QSizePolicy::Expanding);

        verticalLayout->addItem(verticalSpacer_2);

        pushButton_delete = new QPushButton(layoutWidget);
        pushButton_delete->setObjectName(QStringLiteral("pushButton_delete"));
        pushButton_delete->setFont(font1);
        pushButton_delete->setFocusPolicy(Qt::StrongFocus);

        verticalLayout->addWidget(pushButton_delete);

        verticalSpacer_3 = new QSpacerItem(20, 40, QSizePolicy::Minimum, QSizePolicy::Expanding);

        verticalLayout->addItem(verticalSpacer_3);

        pushButton_deleteAll = new QPushButton(layoutWidget);
        pushButton_deleteAll->setObjectName(QStringLiteral("pushButton_deleteAll"));
        pushButton_deleteAll->setFont(font1);
        pushButton_deleteAll->setFocusPolicy(Qt::StrongFocus);

        verticalLayout->addWidget(pushButton_deleteAll);

        verticalSpacer_4 = new QSpacerItem(20, 40, QSizePolicy::Minimum, QSizePolicy::Expanding);

        verticalLayout->addItem(verticalSpacer_4);

        pushButton_hideShowLeft = new QPushButton(layoutWidget);
        pushButton_hideShowLeft->setObjectName(QStringLiteral("pushButton_hideShowLeft"));
        pushButton_hideShowLeft->setFont(font1);
        pushButton_hideShowLeft->setFocusPolicy(Qt::StrongFocus);

        verticalLayout->addWidget(pushButton_hideShowLeft);

        verticalSpacer_5 = new QSpacerItem(20, 40, QSizePolicy::Minimum, QSizePolicy::Expanding);

        verticalLayout->addItem(verticalSpacer_5);


        gridLayout_Right->addLayout(verticalLayout, 0, 0, 6, 1);

        label_Percent = new MoveLabel(layoutWidget);
        label_Percent->setObjectName(QStringLiteral("label_Percent"));
        label_Percent->setFont(font1);

        gridLayout_Right->addWidget(label_Percent, 0, 3, 2, 1);

        horizontalLayout = new QHBoxLayout();
        horizontalLayout->setSpacing(6);
        horizontalLayout->setObjectName(QStringLiteral("horizontalLayout"));
        pushButton_refreshAll = new QPushButton(layoutWidget);
        pushButton_refreshAll->setObjectName(QStringLiteral("pushButton_refreshAll"));
        pushButton_refreshAll->setFont(font1);
        pushButton_refreshAll->setFocusPolicy(Qt::StrongFocus);

        horizontalLayout->addWidget(pushButton_refreshAll);

        pushButton_refreshSelected = new QPushButton(layoutWidget);
        pushButton_refreshSelected->setObjectName(QStringLiteral("pushButton_refreshSelected"));
        pushButton_refreshSelected->setFont(font1);
        pushButton_refreshSelected->setFocusPolicy(Qt::StrongFocus);

        horizontalLayout->addWidget(pushButton_refreshSelected);

        pushButton_exit = new QPushButton(layoutWidget);
        pushButton_exit->setObjectName(QStringLiteral("pushButton_exit"));
        pushButton_exit->setFont(font1);
        pushButton_exit->setFocusPolicy(Qt::StrongFocus);

        horizontalLayout->addWidget(pushButton_exit);


        gridLayout_Right->addLayout(horizontalLayout, 5, 1, 1, 3);

        lineEdit_MD5 = new QLineEdit(layoutWidget);
        lineEdit_MD5->setObjectName(QStringLiteral("lineEdit_MD5"));
        lineEdit_MD5->setFont(font1);
        lineEdit_MD5->setFocusPolicy(Qt::StrongFocus);

        gridLayout_Right->addWidget(lineEdit_MD5, 1, 2, 1, 1);

        horizontalLayout_2 = new QHBoxLayout();
        horizontalLayout_2->setSpacing(6);
        horizontalLayout_2->setObjectName(QStringLiteral("horizontalLayout_2"));
        label_3 = new MoveLabel(layoutWidget);
        label_3->setObjectName(QStringLiteral("label_3"));
        label_3->setFont(font1);
        label_3->setFocusPolicy(Qt::StrongFocus);

        horizontalLayout_2->addWidget(label_3);

        lineEdit_checkSum = new QLineEdit(layoutWidget);
        lineEdit_checkSum->setObjectName(QStringLiteral("lineEdit_checkSum"));
        lineEdit_checkSum->setFont(font1);
        lineEdit_checkSum->setFocusPolicy(Qt::StrongFocus);

        horizontalLayout_2->addWidget(lineEdit_checkSum);

        label_4 = new MoveLabel(layoutWidget);
        label_4->setObjectName(QStringLiteral("label_4"));
        label_4->setFont(font1);
        label_4->setFocusPolicy(Qt::StrongFocus);

        horizontalLayout_2->addWidget(label_4);

        lineEdit_CRC32 = new QLineEdit(layoutWidget);
        lineEdit_CRC32->setObjectName(QStringLiteral("lineEdit_CRC32"));
        lineEdit_CRC32->setFont(font1);
        lineEdit_CRC32->setFocusPolicy(Qt::StrongFocus);

        horizontalLayout_2->addWidget(lineEdit_CRC32);

        label_6 = new MoveLabel(layoutWidget);
        label_6->setObjectName(QStringLiteral("label_6"));
        label_6->setFont(font1);
        label_6->setFocusPolicy(Qt::StrongFocus);

        horizontalLayout_2->addWidget(label_6);

        lineEdit_Size = new QLineEdit(layoutWidget);
        lineEdit_Size->setObjectName(QStringLiteral("lineEdit_Size"));
        lineEdit_Size->setFont(font1);
        lineEdit_Size->setFocusPolicy(Qt::StrongFocus);

        horizontalLayout_2->addWidget(lineEdit_Size);

        checkBox_autoWidth = new QCheckBox(layoutWidget);
        checkBox_autoWidth->setObjectName(QStringLiteral("checkBox_autoWidth"));
        checkBox_autoWidth->setFont(font);

        horizontalLayout_2->addWidget(checkBox_autoWidth);


        gridLayout_Right->addLayout(horizontalLayout_2, 3, 1, 1, 3);

        label_2 = new MoveLabel(layoutWidget);
        label_2->setObjectName(QStringLiteral("label_2"));
        label_2->setFont(font1);
        label_2->setFocusPolicy(Qt::StrongFocus);

        gridLayout_Right->addWidget(label_2, 1, 1, 1, 1);

        lineEdit_SHA1 = new QLineEdit(layoutWidget);
        lineEdit_SHA1->setObjectName(QStringLiteral("lineEdit_SHA1"));
        lineEdit_SHA1->setFont(font1);
        lineEdit_SHA1->setFocusPolicy(Qt::StrongFocus);

        gridLayout_Right->addWidget(lineEdit_SHA1, 2, 2, 1, 2);

        label = new MoveLabel(layoutWidget);
        label->setObjectName(QStringLiteral("label"));
        label->setFont(font1);
        label->setFocusPolicy(Qt::StrongFocus);

        gridLayout_Right->addWidget(label, 0, 1, 1, 1);

        tableView_records = new QTableView(layoutWidget);
        tableView_records->setObjectName(QStringLiteral("tableView_records"));
        tableView_records->setFont(font1);
        tableView_records->setFocusPolicy(Qt::StrongFocus);
        tableView_records->setStyleSheet(QStringLiteral(""));
        tableView_records->setTabKeyNavigation(false);
        tableView_records->horizontalHeader()->setMinimumSectionSize(30);

        gridLayout_Right->addWidget(tableView_records, 4, 1, 1, 3);

        label_5 = new MoveLabel(layoutWidget);
        label_5->setObjectName(QStringLiteral("label_5"));
        label_5->setFont(font1);
        label_5->setFocusPolicy(Qt::StrongFocus);

        gridLayout_Right->addWidget(label_5, 2, 1, 1, 1);

        splitter->addWidget(layoutWidget);

        gridLayout->addWidget(splitter, 0, 0, 1, 1);

        CheckSumClass->setCentralWidget(centralWidget);
        QWidget::setTabOrder(lineEdit_fileName, lineEdit_MD5);
        QWidget::setTabOrder(lineEdit_MD5, lineEdit_SHA1);
        QWidget::setTabOrder(lineEdit_SHA1, lineEdit_checkSum);
        QWidget::setTabOrder(lineEdit_checkSum, lineEdit_CRC32);
        QWidget::setTabOrder(lineEdit_CRC32, lineEdit_Size);
        QWidget::setTabOrder(lineEdit_Size, tableView_records);
        QWidget::setTabOrder(tableView_records, pushButton_refreshAll);
        QWidget::setTabOrder(pushButton_refreshAll, pushButton_refreshSelected);
        QWidget::setTabOrder(pushButton_refreshSelected, pushButton_exit);
        QWidget::setTabOrder(pushButton_exit, treeView_explorer);
        QWidget::setTabOrder(treeView_explorer, pushButton_refresh);
        QWidget::setTabOrder(pushButton_refresh, pushButton_add);
        QWidget::setTabOrder(pushButton_add, pushButton_delete);
        QWidget::setTabOrder(pushButton_delete, pushButton_deleteAll);
        QWidget::setTabOrder(pushButton_deleteAll, pushButton_hideShowLeft);

        retranslateUi(CheckSumClass);

        QMetaObject::connectSlotsByName(CheckSumClass);
    } // setupUi

    void retranslateUi(QMainWindow *CheckSumClass)
    {
        CheckSumClass->setWindowTitle(QApplication::translate("CheckSumClass", "CheckSum", 0));
        pushButton_refresh->setText(QApplication::translate("CheckSumClass", "Refresh", 0));
        label_NoIcon->setText(QApplication::translate("CheckSumClass", "<font size=\"5\">No Icon</font>", 0));
        pushButton_add->setText(QApplication::translate("CheckSumClass", "Add >>", 0));
        pushButton_delete->setText(QApplication::translate("CheckSumClass", "<<  Delete", 0));
        pushButton_deleteAll->setText(QApplication::translate("CheckSumClass", "<< Delete All", 0));
        pushButton_hideShowLeft->setText(QApplication::translate("CheckSumClass", ">>>", 0));
        label_Percent->setText(QApplication::translate("CheckSumClass", "Completed: 0/0", 0));
        pushButton_refreshAll->setText(QApplication::translate("CheckSumClass", "Refresh All", 0));
        pushButton_refreshSelected->setText(QApplication::translate("CheckSumClass", "Refresh Selected", 0));
        pushButton_exit->setText(QApplication::translate("CheckSumClass", "Exit", 0));
        label_3->setText(QApplication::translate("CheckSumClass", "CheckSum:", 0));
        label_4->setText(QApplication::translate("CheckSumClass", "CRC32:", 0));
        label_6->setText(QApplication::translate("CheckSumClass", "Size:", 0));
        checkBox_autoWidth->setText(QApplication::translate("CheckSumClass", "AutoWidth", 0));
        label_2->setText(QApplication::translate("CheckSumClass", "MD5:", 0));
        label->setText(QApplication::translate("CheckSumClass", "FileName:", 0));
        label_5->setText(QApplication::translate("CheckSumClass", "SHA1:", 0));
    } // retranslateUi

};

namespace Ui {
    class CheckSumClass: public Ui_CheckSumClass {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_CHECKSUM_H

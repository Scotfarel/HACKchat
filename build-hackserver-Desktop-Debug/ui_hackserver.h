/********************************************************************************
** Form generated from reading UI file 'hackserver.ui'
**
** Created by: Qt User Interface Compiler version 5.9.1
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_HACKSERVER_H
#define UI_HACKSERVER_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QGridLayout>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QListWidget>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QMenuBar>
#include <QtWidgets/QPlainTextEdit>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QStatusBar>
#include <QtWidgets/QToolBar>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_hackserver
{
public:
    QWidget *centralWidget;
    QGridLayout *gridLayout;
    QPlainTextEdit *plainTextEdit;
    QPushButton *pushButton;
    QLineEdit *lineEdit;
    QListWidget *users;
    QLabel *label;
    QMenuBar *menuBar;
    QToolBar *mainToolBar;
    QStatusBar *statusBar;

    void setupUi(QMainWindow *hackserver)
    {
        if (hackserver->objectName().isEmpty())
            hackserver->setObjectName(QStringLiteral("hackserver"));
        hackserver->resize(464, 380);
        centralWidget = new QWidget(hackserver);
        centralWidget->setObjectName(QStringLiteral("centralWidget"));
        gridLayout = new QGridLayout(centralWidget);
        gridLayout->setSpacing(6);
        gridLayout->setContentsMargins(11, 11, 11, 11);
        gridLayout->setObjectName(QStringLiteral("gridLayout"));
        plainTextEdit = new QPlainTextEdit(centralWidget);
        plainTextEdit->setObjectName(QStringLiteral("plainTextEdit"));

        gridLayout->addWidget(plainTextEdit, 2, 0, 1, 1);

        pushButton = new QPushButton(centralWidget);
        pushButton->setObjectName(QStringLiteral("pushButton"));

        gridLayout->addWidget(pushButton, 1, 0, 1, 1);

        lineEdit = new QLineEdit(centralWidget);
        lineEdit->setObjectName(QStringLiteral("lineEdit"));

        gridLayout->addWidget(lineEdit, 0, 0, 1, 1);

        users = new QListWidget(centralWidget);
        users->setObjectName(QStringLiteral("users"));

        gridLayout->addWidget(users, 4, 0, 1, 1);

        label = new QLabel(centralWidget);
        label->setObjectName(QStringLiteral("label"));

        gridLayout->addWidget(label, 3, 0, 1, 1);

        hackserver->setCentralWidget(centralWidget);
        menuBar = new QMenuBar(hackserver);
        menuBar->setObjectName(QStringLiteral("menuBar"));
        menuBar->setGeometry(QRect(0, 0, 464, 22));
        hackserver->setMenuBar(menuBar);
        mainToolBar = new QToolBar(hackserver);
        mainToolBar->setObjectName(QStringLiteral("mainToolBar"));
        hackserver->addToolBar(Qt::TopToolBarArea, mainToolBar);
        statusBar = new QStatusBar(hackserver);
        statusBar->setObjectName(QStringLiteral("statusBar"));
        hackserver->setStatusBar(statusBar);

        retranslateUi(hackserver);

        QMetaObject::connectSlotsByName(hackserver);
    } // setupUi

    void retranslateUi(QMainWindow *hackserver)
    {
        hackserver->setWindowTitle(QApplication::translate("hackserver", "hackserver", Q_NULLPTR));
        pushButton->setText(QApplication::translate("hackserver", "PushButton", Q_NULLPTR));
        label->setText(QApplication::translate("hackserver", "Online:", Q_NULLPTR));
    } // retranslateUi

};

namespace Ui {
    class hackserver: public Ui_hackserver {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_HACKSERVER_H

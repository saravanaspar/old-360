/********************************************************************************
** Form generated from reading UI file 'mainwindow.ui'
**
** Created by: Qt User Interface Compiler version 6.10.1
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_MAINWINDOW_H
#define UI_MAINWINDOW_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QGridLayout>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QLabel>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QMenuBar>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QStackedWidget>
#include <QtWidgets/QStatusBar>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>
#include "smartcameraview.h"

QT_BEGIN_NAMESPACE

class Ui_MainWindow
{
public:
    QWidget *centralwidget;
    QGridLayout *gridLayout;
    QHBoxLayout *horizontalLayout;
    QHBoxLayout *horizontalLayout_2;
    QStackedWidget *stackedWidget;
    QWidget *pageGrid;
    QGridLayout *gridLayout_2;
    SmartCameraView *camFront;
    SmartCameraView *camRight;
    SmartCameraView *camLeft;
    SmartCameraView *camBack;
    QWidget *pageWheels;
    QGridLayout *gridLayout_6;
    SmartCameraView *camWheelLeft;
    SmartCameraView *camWheelRight;
    QWidget *pageSky;
    QGridLayout *gridLayout_4;
    SmartCameraView *camSky;
    QWidget *pageBirdsEye;
    QGridLayout *gridLayout_5;
    QLabel *lblStitch;
    QWidget *pageFull;
    QGridLayout *gridLayout_3;
    SmartCameraView *camFullScreen;
    QVBoxLayout *verticalLayout;
    QPushButton *btnNavGrid;
    QPushButton *btnNavWheels;
    QPushButton *btnNavSky;
    QPushButton *btnNavBird;
    QMenuBar *menubar;
    QStatusBar *statusbar;

    void setupUi(QMainWindow *MainWindow)
    {
        if (MainWindow->objectName().isEmpty())
            MainWindow->setObjectName("MainWindow");
        MainWindow->resize(800, 630);
        centralwidget = new QWidget(MainWindow);
        centralwidget->setObjectName("centralwidget");
        gridLayout = new QGridLayout(centralwidget);
        gridLayout->setObjectName("gridLayout");
        horizontalLayout = new QHBoxLayout();
        horizontalLayout->setObjectName("horizontalLayout");

        gridLayout->addLayout(horizontalLayout, 0, 0, 1, 1);

        horizontalLayout_2 = new QHBoxLayout();
        horizontalLayout_2->setObjectName("horizontalLayout_2");
        stackedWidget = new QStackedWidget(centralwidget);
        stackedWidget->setObjectName("stackedWidget");
        pageGrid = new QWidget();
        pageGrid->setObjectName("pageGrid");
        gridLayout_2 = new QGridLayout(pageGrid);
        gridLayout_2->setObjectName("gridLayout_2");
        camFront = new SmartCameraView(pageGrid);
        camFront->setObjectName("camFront");

        gridLayout_2->addWidget(camFront, 0, 0, 1, 1);

        camRight = new SmartCameraView(pageGrid);
        camRight->setObjectName("camRight");

        gridLayout_2->addWidget(camRight, 0, 1, 1, 1);

        camLeft = new SmartCameraView(pageGrid);
        camLeft->setObjectName("camLeft");

        gridLayout_2->addWidget(camLeft, 1, 0, 1, 1);

        camBack = new SmartCameraView(pageGrid);
        camBack->setObjectName("camBack");

        gridLayout_2->addWidget(camBack, 1, 1, 1, 1);

        stackedWidget->addWidget(pageGrid);
        pageWheels = new QWidget();
        pageWheels->setObjectName("pageWheels");
        gridLayout_6 = new QGridLayout(pageWheels);
        gridLayout_6->setObjectName("gridLayout_6");
        camWheelLeft = new SmartCameraView(pageWheels);
        camWheelLeft->setObjectName("camWheelLeft");

        gridLayout_6->addWidget(camWheelLeft, 0, 0, 1, 1);

        camWheelRight = new SmartCameraView(pageWheels);
        camWheelRight->setObjectName("camWheelRight");

        gridLayout_6->addWidget(camWheelRight, 0, 1, 1, 1);

        stackedWidget->addWidget(pageWheels);
        pageSky = new QWidget();
        pageSky->setObjectName("pageSky");
        gridLayout_4 = new QGridLayout(pageSky);
        gridLayout_4->setObjectName("gridLayout_4");
        camSky = new SmartCameraView(pageSky);
        camSky->setObjectName("camSky");

        gridLayout_4->addWidget(camSky, 0, 0, 1, 1);

        stackedWidget->addWidget(pageSky);
        pageBirdsEye = new QWidget();
        pageBirdsEye->setObjectName("pageBirdsEye");
        gridLayout_5 = new QGridLayout(pageBirdsEye);
        gridLayout_5->setObjectName("gridLayout_5");
        lblStitch = new QLabel(pageBirdsEye);
        lblStitch->setObjectName("lblStitch");

        gridLayout_5->addWidget(lblStitch, 0, 0, 1, 1);

        stackedWidget->addWidget(pageBirdsEye);
        pageFull = new QWidget();
        pageFull->setObjectName("pageFull");
        gridLayout_3 = new QGridLayout(pageFull);
        gridLayout_3->setObjectName("gridLayout_3");
        camFullScreen = new SmartCameraView(pageFull);
        camFullScreen->setObjectName("camFullScreen");

        gridLayout_3->addWidget(camFullScreen, 0, 0, 1, 1);

        stackedWidget->addWidget(pageFull);

        horizontalLayout_2->addWidget(stackedWidget);

        verticalLayout = new QVBoxLayout();
        verticalLayout->setObjectName("verticalLayout");
        btnNavGrid = new QPushButton(centralwidget);
        btnNavGrid->setObjectName("btnNavGrid");

        verticalLayout->addWidget(btnNavGrid);

        btnNavWheels = new QPushButton(centralwidget);
        btnNavWheels->setObjectName("btnNavWheels");

        verticalLayout->addWidget(btnNavWheels);

        btnNavSky = new QPushButton(centralwidget);
        btnNavSky->setObjectName("btnNavSky");

        verticalLayout->addWidget(btnNavSky);

        btnNavBird = new QPushButton(centralwidget);
        btnNavBird->setObjectName("btnNavBird");

        verticalLayout->addWidget(btnNavBird);


        horizontalLayout_2->addLayout(verticalLayout);


        gridLayout->addLayout(horizontalLayout_2, 1, 1, 1, 1);

        MainWindow->setCentralWidget(centralwidget);
        menubar = new QMenuBar(MainWindow);
        menubar->setObjectName("menubar");
        menubar->setGeometry(QRect(0, 0, 800, 25));
        MainWindow->setMenuBar(menubar);
        statusbar = new QStatusBar(MainWindow);
        statusbar->setObjectName("statusbar");
        MainWindow->setStatusBar(statusbar);

        retranslateUi(MainWindow);

        QMetaObject::connectSlotsByName(MainWindow);
    } // setupUi

    void retranslateUi(QMainWindow *MainWindow)
    {
        MainWindow->setWindowTitle(QCoreApplication::translate("MainWindow", "MainWindow", nullptr));
        camFront->setText(QCoreApplication::translate("MainWindow", "camFront", nullptr));
        camRight->setText(QCoreApplication::translate("MainWindow", "camRight", nullptr));
        camLeft->setText(QCoreApplication::translate("MainWindow", "camLeft", nullptr));
        camBack->setText(QCoreApplication::translate("MainWindow", "camBack", nullptr));
        camWheelLeft->setText(QCoreApplication::translate("MainWindow", "camWheelLeft", nullptr));
        camWheelRight->setText(QCoreApplication::translate("MainWindow", "camWheelRight", nullptr));
        camSky->setText(QCoreApplication::translate("MainWindow", "camSky", nullptr));
        lblStitch->setText(QCoreApplication::translate("MainWindow", "lblStitch", nullptr));
        camFullScreen->setText(QCoreApplication::translate("MainWindow", "camFullScreen", nullptr));
        btnNavGrid->setText(QCoreApplication::translate("MainWindow", "btnNavGrid", nullptr));
        btnNavWheels->setText(QCoreApplication::translate("MainWindow", "btnNavWheels", nullptr));
        btnNavSky->setText(QCoreApplication::translate("MainWindow", "btnNavSky", nullptr));
        btnNavBird->setText(QCoreApplication::translate("MainWindow", "btnNavBird", nullptr));
    } // retranslateUi

};

namespace Ui {
    class MainWindow: public Ui_MainWindow {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_MAINWINDOW_H

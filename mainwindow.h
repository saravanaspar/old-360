#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTimer>
#include <opencv2/opencv.hpp>
#include "smartcameraview.h"
#include "gyrocontroller.h" // <--- Added Header

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void updateStreams();
    void onCameraTapped(int id);
    void goBackToGrid();
    void showPageGrid();
    void showPageWheels();
    void showPageSky();
    void showPageBird();

private:
    Ui::MainWindow *ui;
    QTimer *timer;
    cv::VideoCapture caps[8]; // Your 8 Cameras
    int activeFullId = -1;
    QWidget *lastPage = nullptr;

    GyroController *gyro; // <--- Added Gyro Object
};
#endif // MAINWINDOW_H

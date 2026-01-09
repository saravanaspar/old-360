#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <opencv2/opencv.hpp>
#include <vector>
#include "smartcameraview.h"
#include "gyrocontroller.h"
#include "cameraworker.h" // <--- Include the new worker

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
    void handleFrame(int id, QImage frame); // <--- New Slot
    void onCameraTapped(int id);
    void goBackToGrid();

    // Navigation
    void showPageGrid();
    void showPageWheels();
    void showPageSky();
    void showPageBird();

private:
    Ui::MainWindow *ui;

    // Manage Workers
    std::vector<QThread*> threads;
    std::vector<CameraWorker*> workers;

    int activeFullId = -1;
    QWidget *lastPage = nullptr;
    GyroController *gyro;
};
#endif // MAINWINDOW_H

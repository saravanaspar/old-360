#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QThread>
#include <QDebug>

// --- YOUR RTSP LINKS (With transport=tcp logic handled in worker) ---
const std::string RTSP_LINKS[] = {
    //"rtsp://192.168.0.242:8554/cam?transport=tcp", // ID 0
    //"rtsp://192.168.0.196:8554/cam?transport=tcp", // ID 1
   // "rtsp://192.168.0.197:8554/cam?transport=tcp", // ID 2
      "rtsp://192.168.0.125:8554/cam?transport=tcp", // ID 3
      //"rtsp://192.168.0.122:8554/cam?transport=tcp", // ID 4
     // "rtsp://192.168.0.172:8554/cam?transport=tcp", // ID 5
     "rtsp://192.168.0.214:8554/cam?transport=tcp", // ID 6
      "rtsp://192.168.0.168:8554/cam?transport=tcp", // ID 7
      //"rtsp://192.168.0.168:8554/cam?transport=tcp"  // ID 8 (Duplicate/Stitch)
};

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    // Setup Widget IDs
    ui->camFront->setCameraId(0);
    ui->camBack->setCameraId(1);
    ui->camLeft->setCameraId(2);
    ui->camRight->setCameraId(3);
    ui->camWheelLeft->setCameraId(4);
    ui->camWheelRight->setCameraId(5);
    ui->camSky->setCameraId(6);
    ui->camFrontUnder->setCameraId(7);

    // Setup Click Connections
    connect(ui->camFront, &SmartCameraView::tapped, this, &MainWindow::onCameraTapped);
    connect(ui->camBack,  &SmartCameraView::tapped, this, &MainWindow::onCameraTapped);
    connect(ui->camLeft,  &SmartCameraView::tapped, this, &MainWindow::onCameraTapped);
    connect(ui->camRight, &SmartCameraView::tapped, this, &MainWindow::onCameraTapped);
    connect(ui->camWheelLeft,  &SmartCameraView::tapped, this, &MainWindow::onCameraTapped);
    connect(ui->camWheelRight, &SmartCameraView::tapped, this, &MainWindow::onCameraTapped);
    connect(ui->camSky, &SmartCameraView::tapped, this, &MainWindow::onCameraTapped);
    connect(ui->camFrontUnder, &SmartCameraView::tapped, this, &MainWindow::onCameraTapped);
    connect(ui->camFullScreen, &SmartCameraView::tapped, this, &MainWindow::goBackToGrid);

    // Setup Navigation
    connect(ui->btnNavGrid, &QPushButton::clicked, this, &MainWindow::showPageGrid);
    connect(ui->btnNavWheels, &QPushButton::clicked, this, &MainWindow::showPageWheels);
    connect(ui->btnNavSky, &QPushButton::clicked, this, &MainWindow::showPageSky);
    connect(ui->btnNavBird, &QPushButton::clicked, this, &MainWindow::showPageBird);

    // Setup Gyro
    gyro = new GyroController(this);
    connect(gyro, &GyroController::tiltChanged, this, [=](float x, float y){
        if (ui->stackedWidget->currentWidget() == ui->pageFull) {
            ui->camFullScreen->applyPan(x, y);
        }
    });
    gyro->start();

    // --- START THREADED WORKERS ---
    for(int i = 0; i < 9; i++) {
        if(RTSP_LINKS[i].empty()) continue;

        QThread* thread = new QThread;
        CameraWorker* worker = new CameraWorker(i, RTSP_LINKS[i]);
        worker->moveToThread(thread);

        connect(worker, &CameraWorker::frameReady, this, &MainWindow::handleFrame);
        connect(thread, &QThread::started, worker, &CameraWorker::process);
        connect(worker, &CameraWorker::finished, thread, &QThread::quit);
        connect(worker, &CameraWorker::finished, worker, &CameraWorker::deleteLater);
        connect(thread, &QThread::finished, thread, &QThread::deleteLater);

        workers.push_back(worker);
        threads.push_back(thread);
        thread->start();
    }
}

MainWindow::~MainWindow() {
    // Stop all threads safely
    for(auto w : workers) w->stop();
    for(auto t : threads) {
        t->quit();
        t->wait();
    }
    delete ui;
}

void MainWindow::handleFrame(int id, QImage frame) {
    SmartCameraView* target = nullptr;

    switch(id) {
        case 0: target = ui->camFront; break;
        case 1: target = ui->camBack; break;
        case 2: target = ui->camLeft; break;
        case 3: target = ui->camRight; break;
        case 4: target = ui->camWheelLeft; break;
        case 5: target = ui->camWheelRight; break;
        case 6: target = ui->camSky; break;
        case 7: target = ui->camFrontUnder; break;
        case 8: target = ui->stitchFront; break; // Example mapping for the duplicate
    }

    // Update Grid View
    if(target && ui->stackedWidget->currentWidget() != ui->pageFull) {
        target->setFrame(frame);
    }

    // Update Full Screen View
    if (ui->stackedWidget->currentWidget() == ui->pageFull && activeFullId == id) {
        ui->camFullScreen->setFrame(frame);
    }
}

void MainWindow::onCameraTapped(int id) {
    lastPage = ui->stackedWidget->currentWidget();
    activeFullId = id;

    // Enable YOLO for this camera to save resources
    // (Only run heavy AI on the camera we are actually looking at closely)
    for(size_t i=0; i<workers.size(); i++) {
        // We match by index in the vector, assuming vector order matches IDs 0-8
        workers[i]->toggleYolo(i == id);
    }

    ui->camFullScreen->resetZoom();
    ui->stackedWidget->setCurrentWidget(ui->pageFull);
}

void MainWindow::goBackToGrid() {
    // Disable YOLO to save battery/performance when in grid mode
    for(auto w : workers) w->toggleYolo(false);

    activeFullId = -1;
    if (lastPage != nullptr) ui->stackedWidget->setCurrentWidget(lastPage);
    else ui->stackedWidget->setCurrentWidget(ui->pageGrid);
}

// Navigation Helper Functions
void MainWindow::showPageGrid()   { ui->stackedWidget->setCurrentWidget(ui->pageGrid); }
void MainWindow::showPageWheels() { ui->stackedWidget->setCurrentWidget(ui->pageWheels); }
void MainWindow::showPageSky()    { ui->stackedWidget->setCurrentWidget(ui->pageSky); }
void MainWindow::showPageBird()   { ui->stackedWidget->setCurrentWidget(ui->pageBirdsEye); }

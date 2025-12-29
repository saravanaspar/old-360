#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QtGlobal>

// =========================================================
// YOUR CAMERA LINKS (8 Cameras)
// =========================================================
const std::string RTSP_LINKS[] = {
    // --- SURROUND VIEW (0-3) ---
    "rtsp://192.168.1.154:8554/cam",   // ID 0: Front
    "rtsp://192.168.1.235:8554/cam",   // ID 1: Back
    "rtsp://192.168.1.222:8554/cam",   // ID 2: Left
    "rtsp://192.168.1.228:8554/cam",   // ID 3: Right

    // --- WHEELS PAGE (4-5 & NEW 7) ---
    "",  // ID 4: Front Left Wheel
    "",  // ID 5: Front Right Wheel

    // --- SKY VIEW (6) ---
    "",   // ID 6: Sky

    // --- NEW: FRONT UNDERCARRIAGE (7) ---
    ""    // ID 7: Front Below (Add your IP here!)
};

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    qputenv("OPENCV_FFMPEG_CAPTURE_OPTIONS", "rtsp_transport;tcp");

    // --- STYLING ---
    this->setStyleSheet(R"(
        QMainWindow { background-color: #121212; }
        QWidget#centralwidget { background-color: #121212; }
        QPushButton {
            background-color: #252526; color: #E0E0E0;
            border: 2px solid #3E3E42; border-radius: 8px;
            padding: 15px; font-size: 16px; font-weight: bold;
            text-align: left; padding-left: 20px;
        }
        QPushButton:hover {
            background-color: #3E3E42; border-color: #007ACC; color: #FFFFFF;
        }
        QPushButton:pressed {
            background-color: #007ACC; color: white; border: 2px solid #005A9E;
        }
        QLabel { color: white; font-family: "Segoe UI", sans-serif; }
    )");

    // --- SETUP IDS ---
    ui->camFront->setCameraId(0);
    ui->camBack->setCameraId(1);
    ui->camLeft->setCameraId(2);
    ui->camRight->setCameraId(3);
    ui->camWheelLeft->setCameraId(4);
    ui->camWheelRight->setCameraId(5);
    ui->camSky->setCameraId(6);
    ui->camFrontUnder->setCameraId(7);

    // --- CONNECT SIGNALS ---
    connect(ui->camFront, &SmartCameraView::tapped, this, &MainWindow::onCameraTapped);
    connect(ui->camBack,  &SmartCameraView::tapped, this, &MainWindow::onCameraTapped);
    connect(ui->camLeft,  &SmartCameraView::tapped, this, &MainWindow::onCameraTapped);
    connect(ui->camRight, &SmartCameraView::tapped, this, &MainWindow::onCameraTapped);
    connect(ui->camWheelLeft,  &SmartCameraView::tapped, this, &MainWindow::onCameraTapped);
    connect(ui->camWheelRight, &SmartCameraView::tapped, this, &MainWindow::onCameraTapped);
    connect(ui->camSky, &SmartCameraView::tapped, this, &MainWindow::onCameraTapped);
    connect(ui->camFrontUnder, &SmartCameraView::tapped, this, &MainWindow::onCameraTapped);
    connect(ui->camFullScreen, &SmartCameraView::tapped, this, &MainWindow::goBackToGrid);

    connect(ui->btnNavGrid, &QPushButton::clicked, this, &MainWindow::showPageGrid);
    connect(ui->btnNavWheels, &QPushButton::clicked, this, &MainWindow::showPageWheels);
    connect(ui->btnNavSky, &QPushButton::clicked, this, &MainWindow::showPageSky);
    connect(ui->btnNavBird, &QPushButton::clicked, this, &MainWindow::showPageBird);

    // --- NEW: GYRO SETUP ---
    gyro = new GyroController(this);
    connect(gyro, &GyroController::tiltChanged, this, [=](float x, float y){
        // Only allow pan when in Full Screen mode
        if (ui->stackedWidget->currentWidget() == ui->pageFull) {
            ui->camFullScreen->applyPan(x, y);
        }
    });
    gyro->start();

    // --- OPEN CAMERAS ---
    for(int i=0; i<8; i++) {
        if (RTSP_LINKS[i].empty()) continue;
        caps[i].open(RTSP_LINKS[i], cv::CAP_FFMPEG);
        caps[i].set(cv::CAP_PROP_BUFFERSIZE, 1);
    }

    timer = new QTimer(this);
    connect(timer, &QTimer::timeout, this, &MainWindow::updateStreams);
    timer->start(33);
}

MainWindow::~MainWindow() { delete ui; }

void MainWindow::onCameraTapped(int id) {
    lastPage = ui->stackedWidget->currentWidget();
    activeFullId = id;
    ui->camFullScreen->resetZoom();
    ui->stackedWidget->setCurrentWidget(ui->pageFull);
}

void MainWindow::goBackToGrid() {
    activeFullId = -1;
    if (lastPage != nullptr) ui->stackedWidget->setCurrentWidget(lastPage);
    else ui->stackedWidget->setCurrentWidget(ui->pageGrid);
}

void MainWindow::showPageGrid()   { ui->stackedWidget->setCurrentWidget(ui->pageGrid); }
void MainWindow::showPageWheels() { ui->stackedWidget->setCurrentWidget(ui->pageWheels); }
void MainWindow::showPageSky()    { ui->stackedWidget->setCurrentWidget(ui->pageSky); }
void MainWindow::showPageBird()   { ui->stackedWidget->setCurrentWidget(ui->pageBirdsEye); }

void MainWindow::updateStreams() {
    cv::Mat frame;
    QWidget* currentPage = ui->stackedWidget->currentWidget();

    auto updateView = [&](int id, SmartCameraView* view) {
        if (!caps[id].isOpened()) {
            view->setText("NO LINK");
            return;
        }
        if(caps[id].read(frame)) {
             cv::cvtColor(frame, frame, cv::COLOR_BGR2RGB);
             QImage qimg((const unsigned char*)frame.data, frame.cols, frame.rows, frame.step, QImage::Format_RGB888);
             view->setFrame(qimg);
        }
    };

    if (currentPage == ui->pageGrid) {
        updateView(0, ui->camFront);
        updateView(1, ui->camBack);
        updateView(2, ui->camLeft);
        updateView(3, ui->camRight);
    }
    else if (currentPage == ui->pageFull && activeFullId != -1) {
        updateView(activeFullId, ui->camFullScreen);
    }
    else if (currentPage == ui->pageWheels) {
        updateView(4, ui->camWheelLeft);
        updateView(5, ui->camWheelRight);
        updateView(7, ui->camFrontUnder);
    }
    else if (currentPage == ui->pageSky) {
        updateView(6, ui->camSky);
    }
    else if (currentPage == ui->pageBirdsEye) {
        updateView(0, ui->stitchFront);
        updateView(1, ui->stitchBack);
        updateView(2, ui->stitchLeft);
        updateView(3, ui->stitchRight);
    }
}

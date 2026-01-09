// ===== FILE: cameraworker.cpp (DIAGNOSTIC VERSION) =====
#include "cameraworker.h"
#include <QThread>
#include <QDebug>

CameraWorker::CameraWorker(int id, const std::string& url, QObject* parent)
    : QObject(parent), m_id(id), m_url(url),
      yolo("yolov5s.engine", "coco.names")
{
    qDebug() << "Worker" << m_id << "Initialized";

    // FORCE ON for testing (Override the UI)
    m_yoloEnabled = true;
}

CameraWorker::~CameraWorker() {
    stop();
}

void CameraWorker::stop() {
    m_stop = true;
}

void CameraWorker::toggleYolo(bool enable) {
    m_yoloEnabled = enable;
}

void CameraWorker::process() {
    // 1. Try to lower latency and corruption
    // "rtsp_transport" "tcp" force-enables TCP which is more stable than UDP
    if (m_url.find("rtsp") != std::string::npos) {
        cap.set(cv::CAP_PROP_FORMAT, -1);
    }

    cap.open(m_url, cv::CAP_FFMPEG);

    if (!cap.isOpened()) {
        qDebug() << "❌ Worker" << m_id << "FAILED to open URL:" << QString::fromStdString(m_url);
        emit finished();
        return;
    }

    cv::Mat frame;
    int frameCount = 0;

    while (!m_stop && cap.read(frame)) {
        frameCount++;

        // --- DIAGNOSTIC PRINT 1: Check for Empty Frames ---
        if (frame.empty()) {
            if (frameCount % 30 == 0)
                qDebug() << "❌ Worker" << m_id << "Frame is EMPTY! (Video corruption)";
            continue;
        }

        // --- DIAGNOSTIC PRINT 2: Check Resolution ---
        // If your camera sends 1920x1080 but AI expects 640x640, we need to know.
        if (frameCount % 60 == 0) {
             qDebug() << "🟢 Worker" << m_id << "Running... Res:" << frame.cols << "x" << frame.rows;
        }

        if (m_yoloEnabled && yolo.isLoaded()) {
            // Run Inference
            std::vector<Detection> detections = yolo.infer(frame);

            // Draw Boxes
            for (const auto& d : detections) {
                cv::rectangle(frame, d.box, cv::Scalar(0, 255, 0), 2);
            }
        }

        // Send to Display
        cv::cvtColor(frame, frame, cv::COLOR_BGR2RGB);
        QImage img(frame.data, frame.cols, frame.rows, frame.step, QImage::Format_RGB888);
        emit frameReady(m_id, img.copy());

        QThread::msleep(10);
    }

    cap.release();
    emit finished();
}

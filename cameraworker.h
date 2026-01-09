#ifndef CAMERAWORKER_H
#define CAMERAWORKER_H

#include <QObject>
#include <QImage>
#include <opencv2/opencv.hpp>
#include <string>

#include "trt_yolo.h"

class CameraWorker : public QObject
{
    Q_OBJECT

public:
    explicit CameraWorker(int id, const std::string& url, QObject* parent = nullptr);
    ~CameraWorker();

public slots:
    void process();
    void stop();
    void toggleYolo(bool enable);

signals:
    void frameReady(int id, QImage frame);
    void finished();

private:
    int m_id;
    std::string m_url;
    bool m_stop{false};
    bool m_yoloEnabled{false};

    cv::VideoCapture cap;
    TrtYolo yolo;   // constructed properly in cpp
};

#endif

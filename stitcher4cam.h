#pragma once
#include <QObject>
#include <opencv2/opencv.hpp>
#include <thread>
#include <atomic>

class Stitcher4Cam : public QObject {
    Q_OBJECT
public:
    explicit Stitcher4Cam(QObject *parent = nullptr);
    ~Stitcher4Cam();

    void start();
    void stop();

signals:
    void frameReady(const cv::Mat &frame);

private:
    void run();
    std::thread worker;
    std::atomic<bool> running{false};
};

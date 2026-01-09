// ===== FILE: trt_yolo.h =====
#ifndef TRT_YOLO_H
#define TRT_YOLO_H

#include <opencv2/opencv.hpp>
#include <opencv2/dnn.hpp> // Required for NMSBoxes
#include <NvInfer.h>
#include <cuda_runtime_api.h>
#include <vector>
#include <string>
#include <memory>

struct Detection {
    int class_id;
    float confidence;
    cv::Rect box;
};

class TrtYolo {
public:
    TrtYolo(const std::string& enginePath, const std::string& classesPath);
    ~TrtYolo();

    bool isLoaded() const;

    // Performs Inference + NMS and returns final clean detections
    std::vector<Detection> infer(const cv::Mat& frame);

private:
    bool loadEngine(const std::string& path);
    void loadClasses(const std::string& path);

    // Helper to find correct layer name (e.g., "output" vs "output0")
    int getBindingIndexSafely(const char* name);

    nvinfer1::IRuntime* runtime{nullptr};
    nvinfer1::ICudaEngine* engine{nullptr};
    nvinfer1::IExecutionContext* context{nullptr};

    cudaStream_t stream{};
    void* buffers[2]{nullptr, nullptr}; // Input and Output GPU buffers

    int inputIndex{-1};
    int outputIndex{-1};

    // Standard YOLOv5 input size (Must match your engine export!)
    static constexpr int inputW = 640;
    static constexpr int inputH = 640;

    // Output Size: 25200 anchors * 85 values (xywh + conf + 80 classes)
    static constexpr int outputSize = 25200 * 85;

    std::vector<std::string> classNames;
};

#endif // TRT_YOLO_H

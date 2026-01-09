// ===== FILE: trt_yolo.cpp (DEBUG VERSION) =====
#include "trt_yolo.h"
#include <fstream>
#include <iostream>
#include <algorithm>
#include <cmath>

using namespace nvinfer1;

class TRTLogger : public ILogger {
public:
    void log(Severity severity, const char* msg) noexcept override {
        if (severity <= Severity::kWARNING)
            std::cout << "[TRT] " << msg << std::endl;
    }
};

static TRTLogger gLogger;

TrtYolo::TrtYolo(const std::string& enginePath, const std::string& classesPath) {
    loadClasses(classesPath);
    loadEngine(enginePath);
}

TrtYolo::~TrtYolo() {
    if (stream) cudaStreamDestroy(stream);
    if (buffers[inputIndex]) cudaFree(buffers[inputIndex]);
    if (buffers[outputIndex]) cudaFree(buffers[outputIndex]);
    if (context) context->destroy();
    if (engine) engine->destroy();
    if (runtime) runtime->destroy();
}

bool TrtYolo::isLoaded() const {
    return engine && context;
}

void TrtYolo::loadClasses(const std::string& path) {
    std::ifstream ifs(path);
    if (!ifs.is_open()) {
        std::cerr << "❌ Error: Could not open class file: " << path << std::endl;
        return;
    }
    std::string line;
    while (std::getline(ifs, line)) {
        if (!line.empty()) classNames.push_back(line);
    }
    std::cout << "✅ Loaded " << classNames.size() << " classes." << std::endl;
}

int TrtYolo::getBindingIndexSafely(const char* name) {
    int idx = engine->getBindingIndex(name);
    if (idx == -1) {
        std::string altName = std::string(name) + "0";
        idx = engine->getBindingIndex(altName.c_str());
    }
    return idx;
}

bool TrtYolo::loadEngine(const std::string& path) {
    std::ifstream file(path, std::ios::binary);
    if (!file) {
        std::cerr << "❌ Engine file not found: " << path << std::endl;
        return false;
    }

    file.seekg(0, file.end);
    size_t size = file.tellg();
    file.seekg(0, file.beg);

    std::vector<char> buffer(size);
    file.read(buffer.data(), size);

    runtime = createInferRuntime(gLogger);
    engine = runtime->deserializeCudaEngine(buffer.data(), size, nullptr);

    if (!engine) {
        std::cerr << "❌ Failed to create engine." << std::endl;
        return false;
    }

    context = engine->createExecutionContext();

    inputIndex = getBindingIndexSafely("images");
    outputIndex = getBindingIndexSafely("output");

    if (inputIndex == -1 || outputIndex == -1) {
        std::cerr << "❌ CRITICAL ERROR: Layer names mismatch!\n";
        return false;
    }

    cudaMalloc(&buffers[inputIndex], 3 * inputW * inputH * sizeof(float));
    cudaMalloc(&buffers[outputIndex], outputSize * sizeof(float));
    cudaStreamCreate(&stream);

    std::cout << "✅ TensorRT engine loaded successfully." << std::endl;
    return true;
}

std::vector<Detection> TrtYolo::infer(const cv::Mat& frame) {
    std::vector<Detection> finalDetections;
    if (!context || frame.empty()) return finalDetections;

    // 1. PREPROCESSING (LETTERBOX)
    int origW = frame.cols;
    int origH = frame.rows;

    float r = std::min((float)inputW / origW, (float)inputH / origH);
    int newUnpadW = (int)std::round(origW * r);
    int newUnpadH = (int)std::round(origH * r);
    int dw = (inputW - newUnpadW) / 2;
    int dh = (inputH - newUnpadH) / 2;

    cv::Mat rgb, resized;
    cv::cvtColor(frame, rgb, cv::COLOR_BGR2RGB);

    if (origW != newUnpadW || origH != newUnpadH)
        cv::resize(rgb, resized, cv::Size(newUnpadW, newUnpadH));
    else
        resized = rgb.clone();

    cv::Mat canvas(inputH, inputW, CV_8UC3, cv::Scalar(114, 114, 114));
    resized.copyTo(canvas(cv::Rect(dw, dh, newUnpadW, newUnpadH)));
    canvas.convertTo(canvas, CV_32F, 1.0 / 255.0);

    // HWC -> CHW
    std::vector<float> input(3 * inputW * inputH);
    int idx = 0;
    for (int c = 0; c < 3; c++) {
        for (int y = 0; y < inputH; y++) {
            for (int x = 0; x < inputW; x++) {
                input[idx++] = canvas.at<cv::Vec3f>(y, x)[c];
            }
        }
    }

    // 2. INFERENCE
    cudaMemcpyAsync(buffers[inputIndex], input.data(), input.size() * sizeof(float), cudaMemcpyHostToDevice, stream);
    context->enqueueV2(buffers, stream, nullptr);

    std::vector<float> output(outputSize);
    cudaMemcpyAsync(output.data(), buffers[outputIndex], output.size() * sizeof(float), cudaMemcpyDeviceToHost, stream);
    cudaStreamSynchronize(stream);

    // 3. POST-PROCESSING
    std::vector<int> classIds;
    std::vector<float> confidences;
    std::vector<cv::Rect> boxes;

    // --- DEBUG: TRACK MAX CONFIDENCE ---
    float highestConfFound = 0.0f;
    // -----------------------------------

    // LOWERED THRESHOLD TO 0.10 (10%)
    float confThreshold = 0.10f;
    float nmsThreshold = 0.45f;

    for (int i = 0; i < 25200; i++) {
        float* data = &output[i * 85];
        float objectness = data[4];

        if (objectness < 0.01f) continue; // Optimization

        float maxClassProb = 0.0f;
        int classId = -1;

        for (int c = 5; c < 85; c++) {
            if (data[c] > maxClassProb) {
                maxClassProb = data[c];
                classId = c - 5;
            }
        }

        float finalConf = objectness * maxClassProb;

        // Track stats for debugging
        if (finalConf > highestConfFound) highestConfFound = finalConf;

        if (finalConf > confThreshold) {
            float cx = data[0] * inputW;
            float cy = data[1] * inputH;
            float w  = data[2] * inputW;
            float h  = data[3] * inputH;

            float x_nopad = cx - dw;
            float y_nopad = cy - dh;

            float x_orig = x_nopad / r;
            float y_orig = y_nopad / r;
            float w_orig = w / r;
            float h_orig = h / r;

            int left = (int)(x_orig - w_orig / 2);
            int top  = (int)(y_orig - h_orig / 2);
            int width = (int)w_orig;
            int height = (int)h_orig;

            boxes.push_back(cv::Rect(left, top, width, height));
            confidences.push_back(finalConf);
            classIds.push_back(classId);
        }
    }

    // --- CRITICAL DEBUG PRINT ---
    // This will tell us if the model is outputting ANYTHING at all.
    static int logCounter = 0;
    if (logCounter++ % 30 == 0) { // Print once every 30 frames to avoid spam
        std::cout << "[YOLO DEBUG] Max Confidence in this frame: " << highestConfFound << std::endl;
    }
    // ----------------------------

    std::vector<int> indices;
    cv::dnn::NMSBoxes(boxes, confidences, confThreshold, nmsThreshold, indices);

    for (int idx : indices) {
        Detection det;
        det.class_id = classIds[idx];
        det.confidence = confidences[idx];
        det.box = boxes[idx] & cv::Rect(0, 0, origW, origH);
        finalDetections.push_back(det);
    }

    return finalDetections;
}

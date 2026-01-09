cmake_minimum_required(VERSION 3.16)
project(VehiclePro7 LANGUAGES CXX)

# ----------------------------------------------------
# C++
# ----------------------------------------------------
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# ----------------------------------------------------
# Qt 5
# ----------------------------------------------------
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)

find_package(Qt5 REQUIRED COMPONENTS Widgets Sensors)

# ----------------------------------------------------
# OpenCV
# ----------------------------------------------------
find_package(OpenCV REQUIRED)
message(STATUS "OpenCV version: ${OpenCV_VERSION}")

# ----------------------------------------------------
# CUDA (Jetson – manual, correct way)
# ----------------------------------------------------
set(CUDA_TOOLKIT_ROOT_DIR /usr/local/cuda)

include_directories(
    /usr/local/cuda/include
)

link_directories(
    /usr/local/cuda/lib64
)

# ----------------------------------------------------
# TensorRT
# ----------------------------------------------------
set(TENSORRT_INCLUDE_DIR /usr/include/aarch64-linux-gnu)
set(TENSORRT_LIBRARY_DIR /usr/lib/aarch64-linux-gnu)

find_library(NVINFER_LIB nvinfer HINTS ${TENSORRT_LIBRARY_DIR})
find_library(NVINFER_PLUGIN_LIB nvinfer_plugin HINTS ${TENSORRT_LIBRARY_DIR})

if (NOT NVINFER_LIB)
    message(FATAL_ERROR "TensorRT (nvinfer) not found")
endif()

# ----------------------------------------------------
# Executable
# ----------------------------------------------------
add_executable(VehiclePro7
    main.cpp

    mainwindow.cpp
    mainwindow.h
    mainwindow.ui

    smartcameraview.cpp
    smartcameraview.h

    cameraworker.cpp
    cameraworker.h

    trt_yolo.cpp
    trt_yolo.h

    gyrocontroller.cpp
    gyrocontroller.h
)

# ----------------------------------------------------
# Include directories
# ----------------------------------------------------
target_include_directories(VehiclePro7 PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${OpenCV_INCLUDE_DIRS}
    ${TENSORRT_INCLUDE_DIR}
    /usr/local/cuda/include
)

# ----------------------------------------------------
# Link libraries
# ----------------------------------------------------
target_link_libraries(VehiclePro7 PRIVATE
    Qt5::Widgets
    Qt5::Sensors
    ${OpenCV_LIBS}
    ${NVINFER_LIB}
    ${NVINFER_PLUGIN_LIB}
    cudart
)

# ----------------------------------------------------
# Runtime files
# ----------------------------------------------------
file(COPY ${CMAKE_SOURCE_DIR}/yolov5s.engine
     DESTINATION ${CMAKE_BINARY_DIR})

file(COPY ${CMAKE_SOURCE_DIR}/coco.names
     DESTINATION ${CMAKE_BINARY_DIR})

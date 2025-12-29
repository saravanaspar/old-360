cmake_minimum_required(VERSION 3.16)

project(VehiclePro7 VERSION 0.1 LANGUAGES CXX)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# --- QT SETUP ---
# ADDED 'Sensors' component here
find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Widgets Sensors)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Widgets Sensors)

# --- OPENCV SETUP (Auto-Detect Platform) ---
if(WIN32)
    # Windows: Use your specific path
    set(OpenCV_DIR "C:/Users/SaravanaKumar/Downloads/opencv/build")
else()
    # Linux (Jetson): Use the system installation
    find_package(OpenCV REQUIRED)
endif()

find_package(OpenCV REQUIRED)

# --- SOURCE FILES ---
# ADDED 'gyrocontroller' files here
set(PROJECT_SOURCES
        main.cpp
        mainwindow.cpp
        mainwindow.h
        mainwindow.ui
        smartcameraview.cpp
        smartcameraview.h
        gyrocontroller.cpp
        gyrocontroller.h
        # resources.qrc
)

if(${QT_VERSION_MAJOR} GREATER_EQUAL 6)
    qt_add_executable(VehiclePro7 MANUAL_FINALIZATION ${PROJECT_SOURCES})
else()
    add_executable(VehiclePro7 ${PROJECT_SOURCES})
endif()

# --- THE FIX: Tell compiler where to find header files ---
target_include_directories(VehiclePro7 PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})

# --- LINK LIBRARIES ---
# ADDED 'Qt::Sensors' library here
target_link_libraries(VehiclePro7 PRIVATE
    Qt${QT_VERSION_MAJOR}::Widgets
    Qt${QT_VERSION_MAJOR}::Sensors
    ${OpenCV_LIBS}
)

if(QT_VERSION_MAJOR EQUAL 6)
    qt_finalize_executable(VehiclePro7)
endif()

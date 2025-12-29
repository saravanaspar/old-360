#include "gyrocontroller.h"
#include <QDebug>

GyroController::GyroController(QObject *parent) : QObject(parent)
{
    sensor = new QRotationSensor(this);

    // Connect the sensor reading signal
    connect(sensor, &QRotationSensor::readingChanged, this, [=](){
        QRotationReading *reading = sensor->reading();

        // Get tilt in degrees (approx)
        float x = reading->x();
        float y = reading->y();

        // Normalize logic:
        // Assuming ~30 degrees is "max tilt" for panning
        // We divide by 30.0 to get a range roughly -1.0 to 1.0
        float normX = x / 30.0f;
        float normY = y / 30.0f;

        // Emit the normalized data to the Main Window
        emit tiltChanged(normX, normY);
    });
}

void GyroController::start() {
    if(!sensor->isActive()) sensor->start();
}

void GyroController::stop() {
    if(sensor->isActive()) sensor->stop();
}

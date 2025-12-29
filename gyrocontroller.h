#ifndef GYROCONTROLLER_H
#define GYROCONTROLLER_H

#include <QObject>
#include <QRotationSensor>

class GyroController : public QObject
{
    Q_OBJECT
public:
    explicit GyroController(QObject *parent = nullptr);
    void start();
    void stop();

signals:
    // Emits values relative to tilt (scaled roughly -1.0 to 1.0)
    void tiltChanged(float xTilt, float yTilt);

private:
    QRotationSensor *sensor;
};

#endif // GYROCONTROLLER_H

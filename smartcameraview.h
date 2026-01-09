#ifndef SMARTCAMERAVIEW_H
#define SMARTCAMERAVIEW_H

#include <QLabel>
#include <QEvent>
#include <QPinchGesture>
#include <QWheelEvent>
#include <QMouseEvent>

class SmartCameraView : public QLabel
{
    Q_OBJECT

public:
    explicit SmartCameraView(QWidget *parent = nullptr);
    void setFrame(const QImage &img);
    void setCameraId(int id) { m_id = id; }
    int  getCameraId() const { return m_id; }
    void resetZoom();
    void applyPan(float xTilt, float yTilt);

signals:
    void tapped(int id);

protected:
    bool event(QEvent *event) override;
    void wheelEvent(QWheelEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;
    void mouseDoubleClickEvent(QMouseEvent *event) override;

private:
    void handlePinch(QPinchGesture *gesture);

    int m_id;
    qreal m_currentScaleFactor;
    QImage m_currentImage;
    bool m_isPinching;
    bool m_isDragging;
    QPoint m_lastMousePos;

    float m_panX;
    float m_panY;
};

#endif // SMARTCAMERAVIEW_H

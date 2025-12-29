#ifndef SMARTCAMERAVIEW_H
#define SMARTCAMERAVIEW_H

#include <QLabel>
#include <QEvent>
#include <QTouchEvent>
#include <QPinchGesture>

class SmartCameraView : public QLabel
{
    Q_OBJECT

public:
    explicit SmartCameraView(QWidget *parent = nullptr);
    void setFrame(const QImage &img);
    void setCameraId(int id) { m_id = id; }
    int  getCameraId() const { return m_id; }
    void resetZoom();

    // --- NEW: Added this function ---
    void applyPan(float xTilt, float yTilt);

signals:
    void tapped(int id);

protected:
    bool event(QEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;

private:
    void handlePinch(QPinchGesture *gesture);
    int m_id;
    qreal m_currentScaleFactor;
    QImage m_currentImage;
    bool m_isPinching;

    // --- NEW: Added these variables ---
    float m_panX;
    float m_panY;
};

#endif // SMARTCAMERAVIEW_H

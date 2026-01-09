#include "smartcameraview.h"
#include <QDebug>
#include <algorithm>

SmartCameraView::SmartCameraView(QWidget *parent)
    : QLabel(parent), m_id(-1), m_currentScaleFactor(1.0),
      m_isPinching(false), m_isDragging(false), m_panX(0.0f), m_panY(0.0f)
{
    setStyleSheet("border: 2px solid #444; background-color: #000; color: #888;");
    setAlignment(Qt::AlignCenter);
    setText("NO SIGNAL");

    setAttribute(Qt::WA_AcceptTouchEvents);
    grabGesture(Qt::PinchGesture);
    setMouseTracking(true);
    setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Ignored);
    setScaledContents(false);
}

bool SmartCameraView::event(QEvent *event) {
    if (event->type() == QEvent::Gesture) {
        if (QGesture *pinch = static_cast<QGestureEvent*>(event)->gesture(Qt::PinchGesture)) {
            handlePinch(static_cast<QPinchGesture *>(pinch));
            return true;
        }
    }
    return QLabel::event(event);
}

void SmartCameraView::handlePinch(QPinchGesture *gesture) {
    m_isPinching = true;
    m_currentScaleFactor *= gesture->scaleFactor();
    m_currentScaleFactor = std::clamp(m_currentScaleFactor, 1.0, 5.0);
    setFrame(m_currentImage);
}

void SmartCameraView::wheelEvent(QWheelEvent *event) {
    m_currentScaleFactor *= (event->angleDelta().y() > 0) ? 1.1 : 0.9;
    m_currentScaleFactor = std::clamp(m_currentScaleFactor, 1.0, 5.0);
    qDebug() << "Zoom Level:" << m_currentScaleFactor;
    setFrame(m_currentImage);
}

void SmartCameraView::mousePressEvent(QMouseEvent *event) {
    if (event->button() == Qt::LeftButton) {
        m_isDragging = true;
        m_lastMousePos = event->pos();
    }
}

void SmartCameraView::mouseMoveEvent(QMouseEvent *event) {
    if (m_isDragging && m_currentScaleFactor > 1.0) {
        int dx = event->pos().x() - m_lastMousePos.x();
        int dy = event->pos().y() - m_lastMousePos.y();
        m_lastMousePos = event->pos();

        float sensitivity = 2.0f / m_currentScaleFactor;
        m_panX -= dx / (float)width() * sensitivity;
        m_panY -= dy / (float)height() * sensitivity;

        m_panX = std::clamp(m_panX, -1.0f, 1.0f);
        m_panY = std::clamp(m_panY, -1.0f, 1.0f);

        setFrame(m_currentImage);
    }
}

void SmartCameraView::mouseReleaseEvent(QMouseEvent *) {
    m_isDragging = false;
    m_isPinching = false;
}

void SmartCameraView::mouseDoubleClickEvent(QMouseEvent *) {
    resetZoom();
}

void SmartCameraView::resetZoom() {
    m_currentScaleFactor = 1.0;
    m_panX = m_panY = 0.0f;
    setFrame(m_currentImage);
}

void SmartCameraView::applyPan(float xTilt, float yTilt) {
    m_panX = std::clamp(xTilt, -1.0f, 1.0f);
    m_panY = std::clamp(-yTilt, -1.0f, 1.0f);
    setFrame(m_currentImage);
}

void SmartCameraView::setFrame(const QImage &img) {
    if (img.isNull()) return;
    m_currentImage = img;

    int w = img.width(), h = img.height();
    int cropW = w / m_currentScaleFactor;
    int cropH = h / m_currentScaleFactor;

    int slackX = w - cropW;
    int slackY = h - cropH;

    int x = (slackX / 2) + m_panX * (slackX / 2);
    int y = (slackY / 2) + m_panY * (slackY / 2);

    x = std::clamp(x, 0, w - cropW);
    y = std::clamp(y, 0, h - cropH);

    setPixmap(QPixmap::fromImage(
        img.copy(x, y, cropW, cropH)
    ).scaled(size(), Qt::KeepAspectRatio, Qt::SmoothTransformation));
}

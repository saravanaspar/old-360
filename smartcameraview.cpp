#include "smartcameraview.h"
#include <QPainter>

SmartCameraView::SmartCameraView(QWidget *parent)
    : QLabel(parent), m_id(-1), m_currentScaleFactor(1.0), m_isPinching(false)
{
    // --- STYLING ---
    // Dark grey border, black background
    this->setStyleSheet("border: 2px solid #444; background-color: #000; color: #888;");
    this->setAlignment(Qt::AlignCenter);
    this->setText("NO SIGNAL");

    // --- GESTURE SETUP ---
    this->setAttribute(Qt::WA_AcceptTouchEvents);
    this->grabGesture(Qt::PinchGesture);

    // --- THE FIX FOR "GROWING" SCREEN ---
    // This tells the layout: "Do not expand to fit the image size.
    // Just accept whatever size the layout gives me."
    this->setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Ignored);

    // Ensure standard scaling is off (we handle it manually in setFrame)
    this->setScaledContents(false);
}

bool SmartCameraView::event(QEvent *event) {
    if (event->type() == QEvent::Gesture) {
        QGestureEvent *gestureEvent = static_cast<QGestureEvent*>(event);
        if (QGesture *pinch = gestureEvent->gesture(Qt::PinchGesture)) {
            handlePinch(static_cast<QPinchGesture *>(pinch));
            return true;
        }
    }
    return QLabel::event(event);
}

void SmartCameraView::handlePinch(QPinchGesture *gesture) {
    m_isPinching = true;
    m_currentScaleFactor *= gesture->scaleFactor();

    // Limits: Max zoom 5x, Min zoom 1x
    if (m_currentScaleFactor < 1.0) m_currentScaleFactor = 1.0;
    if (m_currentScaleFactor > 5.0) m_currentScaleFactor = 5.0;

    setFrame(m_currentImage);
}

void SmartCameraView::mouseReleaseEvent(QMouseEvent *event) {
    if (!m_isPinching) emit tapped(m_id);
    m_isPinching = false;
}

void SmartCameraView::resetZoom() { m_currentScaleFactor = 1.0; }

void SmartCameraView::setFrame(const QImage &img) {
    m_currentImage = img;
    if (m_currentImage.isNull()) return;

    // Calculate the crop area based on zoom factor
    int w = m_currentImage.width();
    int h = m_currentImage.height();
    int cropW = w / m_currentScaleFactor;
    int cropH = h / m_currentScaleFactor;
    int x = (w - cropW) / 2;
    int y = (h - cropH) / 2;

    // 1. Crop the image (Zoom)
    // 2. Scale it to fit the WIDGET size (not the image size)
    this->setPixmap(QPixmap::fromImage(m_currentImage.copy(x, y, cropW, cropH))
                        .scaled(this->size(), Qt::KeepAspectRatio, Qt::SmoothTransformation));
}

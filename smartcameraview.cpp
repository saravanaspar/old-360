#include "smartcameraview.h"
#include <QPainter>
#include <algorithm> // For std::clamp

SmartCameraView::SmartCameraView(QWidget *parent)
    : QLabel(parent), m_id(-1), m_currentScaleFactor(1.0),
      m_isPinching(false), m_panX(0.0f), m_panY(0.0f)
{
    // --- STYLING ---
    this->setStyleSheet("border: 2px solid #444; background-color: #000; color: #888;");
    this->setAlignment(Qt::AlignCenter);
    this->setText("NO SIGNAL");

    // --- GESTURE SETUP ---
    this->setAttribute(Qt::WA_AcceptTouchEvents);
    this->grabGesture(Qt::PinchGesture);

    // --- THE FIX FOR "GROWING" SCREEN ---
    this->setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Ignored);
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

void SmartCameraView::resetZoom() {
    m_currentScaleFactor = 1.0;
    m_panX = 0.0f;
    m_panY = 0.0f;
}

// --- NEW: Handle Tilt Input ---
void SmartCameraView::applyPan(float xTilt, float yTilt) {
    // We update the pan targets based on tilt.
    // We clamp them so you can't pan infinitely away.
    // xTilt affects X-axis, yTilt affects Y-axis.
    // Depending on tablet orientation, you might need to swap X/Y or invert signs here.

    // Invert Y because usually tilting "forward" is negative but screen Y is positive down.
    m_panX = std::clamp(xTilt, -1.0f, 1.0f);
    m_panY = std::clamp(-yTilt, -1.0f, 1.0f);

    // Only update visually if we have an image
    if (!m_currentImage.isNull()) {
        setFrame(m_currentImage);
    }
}

void SmartCameraView::setFrame(const QImage &img) {
    m_currentImage = img;
    if (m_currentImage.isNull()) return;

    // 1. Calculate Crop Size (Zoom)
    int w = m_currentImage.width();
    int h = m_currentImage.height();
    int cropW = w / m_currentScaleFactor;
    int cropH = h / m_currentScaleFactor;

    // 2. Calculate Slack (How much extra image do we have?)
    int slackX = w - cropW;
    int slackY = h - cropH;

    // 3. Calculate Center Position
    // Start at dead center (w - cropW) / 2
    // Add offset based on Pan (-0.5 to 0.5 of the slack)
    int x = (slackX / 2) + (int)(m_panX * (slackX / 2));
    int y = (slackY / 2) + (int)(m_panY * (slackY / 2));

    // 4. Safety Bounds (Ensure we don't crop outside image)
    if (x < 0) x = 0;
    if (y < 0) y = 0;
    if (x + cropW > w) x = w - cropW;
    if (y + cropH > h) y = h - cropH;

    // 5. Render
    this->setPixmap(QPixmap::fromImage(m_currentImage.copy(x, y, cropW, cropH))
                        .scaled(this->size(), Qt::KeepAspectRatio, Qt::SmoothTransformation));
}

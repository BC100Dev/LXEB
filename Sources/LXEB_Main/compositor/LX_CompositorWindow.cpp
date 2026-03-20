#include "LX_CompositorWindow.hpp"
#include "LX_Compositor.hpp"

LXEBCompositorWindow::LXEBCompositorWindow(LXEBCompositor* compositor, QScreen* screen, QWindow* parent)
    : QWindow(parent)
      , m_compositor(compositor) {
    setScreen(screen);
    setFlags(Qt::Window | Qt::FramelessWindowHint | Qt::BypassWindowManagerHint);
    setSurfaceType(QWindow::RasterSurface);

    m_backingStore = new QBackingStore(this);
}

void LXEBCompositorWindow::render() const {
    if (!isExposed())
        return;

    QRect rect(0, 0, width(), height());
    m_backingStore->beginPaint(rect);

    QPainter painter(m_backingStore->paintDevice());
    painter.fillRect(rect, QColor("#1a1a2e"));
    painter.end();

    m_backingStore->endPaint();
    m_backingStore->flush(rect);
}

void LXEBCompositorWindow::exposeEvent(QExposeEvent* event) {
    Q_UNUSED(event)
    if (isExposed()) {
        render();
        emit windowReady();
    }
}

void LXEBCompositorWindow::resizeEvent(QResizeEvent* event) {
    m_backingStore->resize(event->size());
    if (isExposed())
        render();
    emit windowResized(event->size());
}

void LXEBCompositorWindow::closeEvent(QCloseEvent* event) {
    event->ignore();
}

void LXEBCompositorWindow::keyPressEvent(QKeyEvent* event) {
    if (event->modifiers() & Qt::AltModifier && event->key() == Qt::Key_F4) {
        event->accept();
        return;
    }

    QWindow::keyPressEvent(event);
}

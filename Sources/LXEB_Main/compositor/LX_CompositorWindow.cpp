#include "LX_CompositorWindow.hpp"
#include "LX_Compositor.hpp"

#include <QtWaylandCompositor/QWaylandSurface>
#include <QtWaylandCompositor/QWaylandBufferRef>
#include <QOpenGLFunctions>
#include <QOpenGLTexture>
#include <QDebug>

LXEBCompositorWindow::LXEBCompositorWindow(LXEBCompositor* compositor, QScreen* screen, QWindow* parent)
    : QWindow(parent)
      , m_compositor(compositor) {
    setScreen(screen);
    setFlags(Qt::Window | Qt::FramelessWindowHint | Qt::BypassWindowManagerHint);
    setSurfaceType(QWindow::OpenGLSurface);

    m_glContext = new QOpenGLContext(this);
    m_glContext->setFormat(requestedFormat());
    m_glContext->create();
}

void LXEBCompositorWindow::render() {
    if (!isExposed() || !m_glContext)
        return;

    m_glContext->makeCurrent(this);
    QOpenGLFunctions* gl = m_glContext->functions();

    gl->glViewport(0, 0, width(), height());
    gl->glClearColor(0.102f, 0.102f, 0.180f, 1.0f);
    gl->glClear(GL_COLOR_BUFFER_BIT);

    for (LXEBView* view : m_compositor->views()) {
        if (!view->surface() || !view->surface()->hasContent())
            continue;

        QWaylandBufferRef buf = view->currentBuffer();
        if (buf.isNull())
            continue;

        auto* texture = buf.toOpenGLTexture();
        if (!texture)
            continue;

        texture->bind();
        // TODO: fullscreen quad shader
        texture->release();
    }

    m_glContext->swapBuffers(this);
}

void LXEBCompositorWindow::exposeEvent(QExposeEvent* event) {
    Q_UNUSED(event)
    if (isExposed()) {
        render();
        emit windowReady();
    }
}

void LXEBCompositorWindow::resizeEvent(QResizeEvent* event) {
    Q_UNUSED(event)
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

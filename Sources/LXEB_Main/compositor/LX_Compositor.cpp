#include "LX_Compositor.hpp"
#include "LX_CompositorWindow.hpp"

#include <QtWaylandCompositor/QWaylandXdgSurface>
#include <QDebug>

LXEBView::LXEBView(QWaylandSurface* surface, QObject* parent) : QWaylandView(nullptr, parent) {
    setSurface(surface);
    connect(surface, &QWaylandSurface::destroyed,
            this, &LXEBView::surfaceDestroyed);
}

LXEBCompositor::LXEBCompositor(QObject* parent) : QWaylandCompositor(parent) {
}

LXEBCompositor::~LXEBCompositor() = default;

void LXEBCompositor::initialize() {
    setSocketName("wayland-lxeb");

    m_xdgShell = new QWaylandXdgShell(this);

    connect(m_xdgShell, &QWaylandXdgShell::toplevelCreated,
            this, &LXEBCompositor::onXdgToplevelCreated);

    connect(this, &QWaylandCompositor::surfaceCreated,
            this, &LXEBCompositor::onSurfaceCreated);

    for (QScreen *screen : QGuiApplication::screens())
        createOutput(screen);

    connect(static_cast<QGuiApplication *>(qApp),
            &QGuiApplication::screenAdded,
            this,
            [this](QScreen *screen) { createOutput(screen); });

    connect(static_cast<QGuiApplication *>(qApp),
            &QGuiApplication::screenRemoved,
            this,
            [this](QScreen *screen) { removeOutput(screen); });

    QWaylandCompositor::create();

    qDebug() << "[LXEB Compositor] Online —"
        << socketName()
        << "— outputs:" << m_outputWindows.size();
}

void LXEBCompositor::createOutput(QScreen *screen)
{
    auto *window = new LXEBCompositorWindow(this, screen);
    window->setTitle(QStringLiteral("LXEB — %1").arg(screen->name()));
    window->setGeometry(screen->geometry());
    window->setVisibility(QWindow::FullScreen);

    auto *output = new QWaylandOutput(this, window);
    output->setSizeFollowsWindow(true);
    output->setPosition(screen->geometry().topLeft());

    m_outputWindows.append(window);
    m_outputs.append(output);

    window->show();
}

void LXEBCompositor::removeOutput(QScreen* screen) {
    for (int i = 0; i < m_outputWindows.size(); ++i) {
        if (m_outputWindows[i]->screen() == screen) {
            qDebug() << "[LXEB Compositor] Output removed for screen:" << screen->name();
            m_outputWindows[i]->deleteLater();
            m_outputs[i]->deleteLater();
            m_outputWindows.removeAt(i);
            m_outputs.removeAt(i);
            return;
        }
    }
}

void LXEBCompositor::onSurfaceCreated(QWaylandSurface* surface) {
    connect(surface, &QWaylandSurface::destroyed,
            this, [this, surface]() {
                LXEBView* view = viewForSurface(surface);
                if (!view)
                    return;
                m_views.removeOne(view);
                emit surfaceUnmapped(view);
                view->deleteLater();
            });
}

void LXEBCompositor::onXdgToplevelCreated(QWaylandXdgToplevel* toplevel, QWaylandXdgSurface* xdgSurface) {
    Q_UNUSED(toplevel)

    QWaylandSurface* surface = xdgSurface->surface();
    auto* view = new LXEBView(surface, this);
    m_views.append(view);

    connect(surface, &QWaylandSurface::hasContentChanged, this, [this, view, surface]() {
        if (surface->hasContent())
            emit surfaceMapped(view);
        else
            emit surfaceUnmapped(view);
    });

    connect(view, &LXEBView::surfaceDestroyed, this, [this, view]() {
        m_views.removeOne(view);
        emit surfaceUnmapped(view);
        view->deleteLater();
    });
}

LXEBView* LXEBCompositor::viewForSurface(QWaylandSurface* surface) const {
    for (LXEBView* view : m_views) {
        if (view->surface() == surface)
            return view;
    }
    return nullptr;
}

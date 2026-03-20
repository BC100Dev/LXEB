#ifndef LXEB_LX_COMPOSITOR_HPP
#define LXEB_LX_COMPOSITOR_HPP

#include <QtWaylandCompositor/QWaylandCompositor>
#include <QtWaylandCompositor/QWaylandSurface>
#include <QtWaylandCompositor/QWaylandView>
#include <QtWaylandCompositor/QWaylandXdgShell>
#include <QtWaylandCompositor/QWaylandXdgToplevel>
#include <QtWaylandCompositor/QWaylandOutput>
#include <QtWaylandCompositor/QWaylandSeat>

#include <QGuiApplication>
#include <QScreen>
#include <QList>
#include <QObject>
#include <QWindow>

class LXEBCompositorWindow;

class LXEBView : public QWaylandView {
    Q_OBJECT

public:
    explicit LXEBView(QWaylandSurface *surface, QObject *parent = nullptr);
    ~LXEBView() override = default;

    signals:
        void surfaceDestroyed();
};

class LXEBCompositor : public QWaylandCompositor {
    Q_OBJECT

public:
    explicit LXEBCompositor(QObject *parent = nullptr);
    ~LXEBCompositor() override;

    void initialize();

    const QList<LXEBCompositorWindow *> &outputWindows() const { return m_outputWindows; }
    const QList<LXEBView *>             &views()         const { return m_views; }

    signals:
        void surfaceMapped(LXEBView *view);
    void surfaceUnmapped(LXEBView *view);

private slots:
    void onSurfaceCreated(QWaylandSurface *surface);
    void onXdgToplevelCreated(QWaylandXdgToplevel *toplevel, QWaylandXdgSurface *xdgSurface);
    void createOutput(QScreen *screen);
    void removeOutput(QScreen *screen);

private:
    LXEBView *viewForSurface(QWaylandSurface *surface) const;

    QWaylandXdgShell              *m_xdgShell     = nullptr;
    QList<LXEBCompositorWindow *>  m_outputWindows;
    QList<QWaylandOutput *>        m_outputs;
    QList<LXEBView *>              m_views;
};

#endif //LXEB_LX_COMPOSITOR_HPP
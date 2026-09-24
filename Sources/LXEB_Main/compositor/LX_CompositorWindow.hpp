#ifndef LXEB_LX_COMPOSITORWINDOW_HPP
#define LXEB_LX_COMPOSITORWINDOW_HPP

#include <QWindow>
#include <QScreen>
#include <QExposeEvent>
#include <QResizeEvent>
#include <QOpenGLContext>
#include <QOpenGLFunctions>
#include <QtWaylandCompositor/QWaylandBufferRef>

class LXEBCompositor;

class LXEBCompositorWindow : public QWindow {
    Q_OBJECT

public:
    explicit LXEBCompositorWindow(LXEBCompositor* compositor, QScreen* screen, QWindow* parent = nullptr);
    ~LXEBCompositorWindow() override = default;

    void render();

signals:
    void windowReady();
    void windowResized(const QSize& size);

protected:
    void exposeEvent(QExposeEvent* event) override;
    void resizeEvent(QResizeEvent* event) override;
    void closeEvent(QCloseEvent* event) override;
    void keyPressEvent(QKeyEvent* event) override;

private:
    LXEBCompositor* m_compositor = nullptr;
    QOpenGLContext* m_glContext = nullptr;
};

#endif //LXEB_LX_COMPOSITORWINDOW_HPP

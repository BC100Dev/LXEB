#ifndef LXEB_LX_COMPOSITORWINDOW_HPP
#define LXEB_LX_COMPOSITORWINDOW_HPP

#include <QWindow>
#include <QScreen>
#include <QExposeEvent>
#include <QResizeEvent>
#include <QBackingStore>
#include <QPainter>

class LXEBCompositor;

class LXEBCompositorWindow : public QWindow {
    Q_OBJECT

public:
    explicit LXEBCompositorWindow(LXEBCompositor* compositor, QScreen* screen, QWindow* parent = nullptr);
    ~LXEBCompositorWindow() override = default;

    void render() const;

signals:
    void windowReady();
    void windowResized(const QSize& size);

protected:
    void exposeEvent(QExposeEvent* event) override;
    void resizeEvent(QResizeEvent* event) override;
    void closeEvent(QCloseEvent *event) override;
    void keyPressEvent(QKeyEvent *event) override;

private:
    LXEBCompositor* m_compositor = nullptr;
    QBackingStore* m_backingStore = nullptr;
};

#endif //LXEB_LX_COMPOSITORWINDOW_HPP

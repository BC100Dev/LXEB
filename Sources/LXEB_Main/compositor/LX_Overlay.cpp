#include "LX_Overlay.hpp"
#include <QApplication>
#include <QCloseEvent>
#include <QVBoxLayout>
#include <QScreen>

LXEBOverlay::LXEBOverlay(QScreen* screen, QWidget* parent)
    : QWidget(parent) {
    setScreen(screen);
    setWindowFlags(Qt::Window | Qt::FramelessWindowHint
        | Qt::BypassWindowManagerHint | Qt::WindowStaysOnTopHint);
    setAttribute(Qt::WA_TranslucentBackground);

    // Pin to bottom-right corner
    const QRect geo = screen->geometry();
    const int w = 120, h = 40;
    setGeometry(geo.right() - w, geo.bottom() - h, w, h);

    auto* layout = new QVBoxLayout(this);
    layout->setContentsMargins(0, 0, 0, 0);

    m_exitButton = new QPushButton("Exit LXEB", this);
    layout->addWidget(m_exitButton);

    connect(m_exitButton, &QPushButton::clicked, this, []() {
        QApplication::quit();
    });

    setLayout(layout);
}

void LXEBOverlay::closeEvent(QCloseEvent* event) {
    event->ignore();
}

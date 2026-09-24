#ifndef LXEB_LX_OVERLAY_HPP
#define LXEB_LX_OVERLAY_HPP

#include <QWidget>
#include <QPushButton>

class LXEBOverlay : public QWidget {
    Q_OBJECT

public:
    explicit LXEBOverlay(QScreen* screen, QWidget* parent = nullptr);
    ~LXEBOverlay() override = default;

protected:
    void closeEvent(QCloseEvent* event) override;

private:
    QPushButton* m_exitButton = nullptr;
};

#endif //LXEB_LX_OVERLAY_HPP

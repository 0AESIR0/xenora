#ifndef PANEL_H
#define PANEL_H

#include <QWidget>
#include <QPushButton>
#include <QMenu>
#include <QTimer>
#include "taskbar.h"
#include "systemtray.h"
#include "launcher.h"

class Panel : public QWidget
{
    Q_OBJECT

public:
    explicit Panel(QWidget *parent = nullptr);
    ~Panel();

    // Panel konumunu ayarlama (alt, üst, sol, sağ)
    void setPosition(Qt::Edge position);
    void setAutoHide(bool enabled);

protected:
    void paintEvent(QPaintEvent *event) override;
    void resizeEvent(QResizeEvent *event) override;
    void enterEvent(QEvent *event) override;
    void leaveEvent(QEvent *event) override;

private slots:
    void showStartMenu();
    void toggleAutoHide();
    void checkMousePosition();

private:
    void setupUI();
    void setupStartMenu();
    void updatePanelGeometry();

    QPushButton *m_startButton;
    Taskbar *m_taskbar;
    SystemTray *m_systemTray;
    Launcher *m_launcher;
    QMenu *m_startMenu;
    
    Qt::Edge m_position;
    bool m_autoHide;
    QTimer *m_hideTimer;
    int m_panelSize;
    int m_hiddenSize;
};

#endif // PANEL_H

#pragma once

#include <QMainWindow>
#include <QScreen>
#include "desktop/desktopmanager.h"
#include "panel/panel.h"

class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

    void setupDesktop();
    void setupPanel();
    void applyTheme();

private:
    DesktopManager *m_desktopManager;
    Panel *m_panel;

private slots:
    void handleScreenChange();
};

#pragma once

#include <QWidget>
#include <QHBoxLayout>
#include <memory>
#include "SystemTray.h"
#include "../widgets/XenoraButton.h"
#include "../widgets/XenoraPanel.h"

class Panel : public QWidget
{
    Q_OBJECT
    
public:
    explicit Panel(QWidget *parent = nullptr);
    ~Panel();
    
    void refresh();
    void setSystemTray(SystemTray* systemTray);
    
public slots:
    void toggleAppLauncher();
    void handleWindowAdded(class XenoraWindow* window);
    void handleWindowRemoved(class XenoraWindow* window);
    
protected:
    void paintEvent(QPaintEvent *event) override;
    void showEvent(QShowEvent *event) override;
    
private:
    XenoraPanel* m_panel;
    QHBoxLayout* m_layout;
    QWidget* m_startButtonContainer;
    XenoraButton* m_startButton;
    QWidget* m_taskbarContainer;
    QHBoxLayout* m_taskbarLayout;
    QWidget* m_systemTrayContainer;
    QHBoxLayout* m_systemTrayLayout;
    QWidget* m_clockWidget;
    SystemTray* m_systemTray;
    
    void setupUi();
    void setupStartButton();
    void setupTaskbar();
    void setupSystemTrayArea();
    void setupClock();
};

#pragma once

#include <QObject>
#include <QScreen>
#include <memory>
#include "WindowManager.h"
#include "Panel.h"
#include "Desktop.h"
#include "AppLauncher.h"
#include "SystemTray.h"

class DesktopEnvironment : public QObject
{
    Q_OBJECT
    
public:
    explicit DesktopEnvironment(QObject *parent = nullptr);
    ~DesktopEnvironment();
    
    void initialize();
    void show();
    void hide();
    void refresh();
    
    WindowManager* windowManager() const { return m_windowManager.get(); }
    Panel* panel() const { return m_panel.get(); }
    Desktop* desktop() const { return m_desktop.get(); }
    AppLauncher* appLauncher() const { return m_appLauncher.get(); }
    SystemTray* systemTray() const { return m_systemTray.get(); }
    
private slots:
    void handleScreenChanged(QScreen* screen);
    void handleGeometryChanged(const QRect& geometry);
    
private:
    std::unique_ptr<WindowManager> m_windowManager;
    std::unique_ptr<Panel> m_panel;
    std::unique_ptr<Desktop> m_desktop;
    std::unique_ptr<AppLauncher> m_appLauncher;
    std::unique_ptr<SystemTray> m_systemTray;
};

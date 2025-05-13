#include "DesktopEnvironment.h"
#include <QApplication>
#include <QDesktopWidget>
#include <QDebug>

DesktopEnvironment::DesktopEnvironment(QObject *parent)
    : QObject(parent)
{
}

DesktopEnvironment::~DesktopEnvironment()
{
}

void DesktopEnvironment::initialize()
{
    qDebug() << "Initializing XenoraOS Desktop Environment...";
    
    // Create desktop background first (lowest z-level)
    m_desktop = std::make_unique<Desktop>();
    
    // Initialize window manager
    m_windowManager = std::make_unique<WindowManager>();
    m_windowManager->initialize();
    
    // Create panel (taskbar)
    m_panel = std::make_unique<Panel>();
    
    // Create app launcher
    m_appLauncher = std::make_unique<AppLauncher>();
    
    // Create system tray
    m_systemTray = std::make_unique<SystemTray>();
    m_panel->setSystemTray(m_systemTray.get());
    
    // Connect signals/slots
    connect(QApplication::primaryScreen(), &QScreen::geometryChanged,
            this, &DesktopEnvironment::handleGeometryChanged);
    connect(QApplication::instance(), &QApplication::primaryScreenChanged,
            this, &DesktopEnvironment::handleScreenChanged);
    
    qDebug() << "XenoraOS Desktop Environment initialized successfully";
}

void DesktopEnvironment::show()
{
    m_desktop->show();
    m_panel->show();
    
    // Set initial geometries
    handleGeometryChanged(QApplication::primaryScreen()->geometry());
}

void DesktopEnvironment::hide()
{
    m_desktop->hide();
    m_panel->hide();
    m_appLauncher->hide();
}

void DesktopEnvironment::refresh()
{
    m_desktop->refresh();
    m_panel->refresh();
    m_windowManager->refresh();
}

void DesktopEnvironment::handleScreenChanged(QScreen* screen)
{
    handleGeometryChanged(screen->geometry());
}

void DesktopEnvironment::handleGeometryChanged(const QRect& geometry)
{
    m_desktop->setGeometry(geometry);
    
    // Panel at bottom of screen
    QRect panelRect = geometry;
    int panelHeight = 48; // Modern, slightly taller panel
    panelRect.setTop(panelRect.bottom() - panelHeight + 1);
    m_panel->setGeometry(panelRect);
    
    // Notify window manager of available space
    QRect availableSpace = geometry;
    availableSpace.setBottom(availableSpace.bottom() - panelHeight);
    m_windowManager->setAvailableGeometry(availableSpace);
}

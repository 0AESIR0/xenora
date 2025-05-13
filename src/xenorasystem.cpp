#include "xenorasystem.h"
#include <QDebug>

XenoraSystem::XenoraSystem(QObject *parent)
    : QObject(parent),
      m_desktop(nullptr),
      m_panel(nullptr),
      m_windowManager(nullptr),
      m_settingsManager(nullptr)
{
}

XenoraSystem::~XenoraSystem() {
    shutdown();
}

void XenoraSystem::initialize() {
    qDebug() << "Initializing XenoraOS...";
    
    // Create settings manager first as other components depend on it
    m_settingsManager = new SettingsManager(this);
    
    // Create the desktop
    m_desktop = new Desktop(nullptr);
    
    // Create the panel
    m_panel = new Panel(nullptr);
    
    // Create window manager
    m_windowManager = new WindowManager(this);
    
    // Setup connections between components
    setupSystemConnections();
    
    // Show the desktop
    m_desktop->show();
    
    // Show the panel
    m_panel->show();
    
    qDebug() << "XenoraOS initialized successfully";
}

void XenoraSystem::shutdown() {
    qDebug() << "Shutting down XenoraOS...";
    
    if (m_panel) {
        m_panel->hide();
        m_panel->deleteLater();
        m_panel = nullptr;
    }
    
    if (m_desktop) {
        m_desktop->hide();
        m_desktop->deleteLater();
        m_desktop = nullptr;
    }
    
    if (m_windowManager) {
        delete m_windowManager;
        m_windowManager = nullptr;
    }
    
    if (m_settingsManager) {
        delete m_settingsManager;
        m_settingsManager = nullptr;
    }
}

void XenoraSystem::setupSystemConnections() {
    // Connect screen geometry changes
    QScreen *primaryScreen = QApplication::primaryScreen();
    connect(primaryScreen, &QScreen::geometryChanged, 
            this, &XenoraSystem::onScreenGeometryChanged);
    
    // Connect panel to desktop
    connect(m_panel, &Panel::startMenuToggled, 
            m_desktop, &Desktop::onStartMenuToggled);
    
    // Connect settings manager to desktop and panel
    connect(m_settingsManager, &SettingsManager::backgroundChanged,
            m_desktop, &Desktop::setBackground);
}

void XenoraSystem::onScreenGeometryChanged(const QRect &geometry) {
    if (m_desktop) {
        m_desktop->resize(geometry.width(), geometry.height());
    }
    
    if (m_panel) {
        m_panel->updateGeometry(geometry);
    }
}

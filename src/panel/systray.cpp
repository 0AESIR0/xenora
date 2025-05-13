#include "systray.h"
#include <QDebug>

SystemTray::SystemTray(QWidget *parent)
    : QWidget(parent),
      m_layout(nullptr)
{
    setObjectName("SystemTray");
    initializeSystemTray();
    setupDefaultIcons();
}

SystemTray::~SystemTray() {
    // Clean up resources
}

void SystemTray::initializeSystemTray() {
    // Create layout
    m_layout = new QHBoxLayout(this);
    m_layout->setContentsMargins(0, 0, 0, 0);
    m_layout->setSpacing(5);
    
    setLayout(m_layout);
}

void SystemTray::setupDefaultIcons() {
    // Add default system tray icons
    // Network
    addTrayIcon("Network", QIcon::fromTheme("network-wireless"));
    
    // Volume
    addTrayIcon("Volume", QIcon::fromTheme("audio-volume-high"));
    
    // Battery
    addTrayIcon("Battery", QIcon::fromTheme("battery-full"));
}

void SystemTray::addTrayIcon(const QString &name, const QIcon &icon) {
    QPushButton *button = new QPushButton(this);
    button->setObjectName("TrayIcon");
    button->setFixedSize(32, 32);
    button->setIcon(icon);
    button->setIconSize(QSize(20, 20));
    button->setToolTip(name);
    button->setProperty("trayName", name);
    
    connect(button, &QPushButton::clicked, this, &SystemTray::onTrayIconClicked);
    
    m_trayIcons.append(button);
    m_layout->addWidget(button);
}

void SystemTray::onTrayIconClicked() {
    QPushButton *button = qobject_cast<QPushButton*>(sender());
    if (button) {
        QString trayName = button->property("trayName").toString();
        emit trayIconClicked(trayName);
    }
}

#include "SystemTray.h"
#include "../widgets/XenoraButton.h"
#include "../widgets/XenoraPopup.h"

#include <QHBoxLayout>
#include <QIcon>
#include <QDebug>

// SystemTrayIcon implementation
SystemTrayIcon::SystemTrayIcon(QObject *parent)
    : QObject(parent)
{
}

SystemTrayIcon::~SystemTrayIcon()
{
}

// NetworkIcon implementation
NetworkIcon::NetworkIcon(QObject *parent)
    : SystemTrayIcon(parent)
{
    setId("network");
    setTooltip("Network: Connected");
    setIcon(QIcon(":/icons/system/wifi.svg"));
}

void NetworkIcon::activate()
{
    qDebug() << "Network icon clicked";
    
    // Create and show network settings popup
    XenoraPopup *popup = new XenoraPopup(nullptr, "Network Settings");
    popup->setSize(300, 400);
    popup->show();
}

QWidget* NetworkIcon::createWidget()
{
    XenoraButton *button = new XenoraButton();
    button->setIcon(icon());
    button->setButtonStyle(XenoraButton::Icon);
    button->setIconSize(QSize(22, 22));
    button->setFixedSize(36, 36);
    button->setToolTip(tooltip());
    
    connect(button, &XenoraButton::clicked, this, &NetworkIcon::activate);
    
    return button;
}

// VolumeIcon implementation
VolumeIcon::VolumeIcon(QObject *parent)
    : SystemTrayIcon(parent)
{
    setId("volume");
    setTooltip("Volume: 80%");
    setIcon(QIcon(":/icons/system/volume.svg"));
}

void VolumeIcon::activate()
{
    qDebug() << "Volume icon clicked";
    
    // Create and show volume popup
    XenoraPopup *popup = new XenoraPopup(nullptr, "Volume");
    popup->setSize(250, 100);
    
    // TODO: Add volume slider
    
    popup->show();
}

QWidget* VolumeIcon::createWidget()
{
    XenoraButton *button = new XenoraButton();
    button->setIcon(icon());
    button->setButtonStyle(XenoraButton::Icon);
    button->setIconSize(QSize(22, 22));
    button->setFixedSize(36, 36);
    button->setToolTip(tooltip());
    
    connect(button, &XenoraButton::clicked, this, &VolumeIcon::activate);
    
    return button;
}

// BatteryIcon implementation
BatteryIcon::BatteryIcon(QObject *parent)
    : SystemTrayIcon(parent)
{
    setId("battery");
    setTooltip("Battery: 75%");
    setIcon(QIcon(":/icons/system/battery.svg"));
}

void BatteryIcon::activate()
{
    qDebug() << "Battery icon clicked";
    
    // Create and show battery popup
    XenoraPopup *popup = new XenoraPopup(nullptr, "Battery Status");
    popup->setSize(300, 200);
    
    // TODO: Add battery details
    
    popup->show();
}

QWidget* BatteryIcon::createWidget()
{
    XenoraButton *button = new XenoraButton();
    button->setIcon(icon());
    button->setButtonStyle(XenoraButton::Icon);
    button->setIconSize(QSize(22, 22));
    button->setFixedSize(36, 36);
    button->setToolTip(tooltip());
    
    connect(button, &XenoraButton::clicked, this, &BatteryIcon::activate);
    
    return button;
}

// SystemTray implementation
SystemTray::SystemTray(QWidget *parent)
    : QWidget(parent)
{
    setupUi();
    setupDefaultIcons();
}

SystemTray::~SystemTray()
{
    // Delete all system tray icons
    qDeleteAll(m_icons);
    m_icons.clear();
}

void SystemTray::addIcon(SystemTrayIcon *icon)
{
    if (!icon) return;
    
    m_icons.append(icon);
    
    QWidget *iconWidget = icon->createWidget();
    m_layout->addWidget(iconWidget);
}

void SystemTray::removeIcon(const QString &id)
{
    for (int i = 0; i < m_icons.size(); ++i) {
        if (m_icons[i]->id() == id) {
            SystemTrayIcon *icon = m_icons.takeAt(i);
            delete icon;
            break;
        }
    }
}

void SystemTray::openQuickSettings()
{
    // Create and show quick settings panel
    XenoraPopup *popup = new XenoraPopup(nullptr, "Quick Settings");
    popup->setSize(350, 500);
    popup->show();
}

void SystemTray::setupUi()
{
    m_layout = new QHBoxLayout(this);
    m_layout->setContentsMargins(0, 0, 0, 0);
    m_layout->setSpacing(4);
    
    setLayout(m_layout);
}

void SystemTray::setupDefaultIcons()
{
    // Add default system tray icons
    addIcon(new NetworkIcon(this));
    addIcon(new VolumeIcon(this));
    addIcon(new BatteryIcon(this));
}

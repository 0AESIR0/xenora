#include "SystemTray.h"
#include "../widgets/XenoraButton.h"

#include <QHBoxLayout>
#include <QLabel>
#include <QDateTime>
#include <QTimer>
#include <QMenu>
#include <QAction>
#include <QDebug>

SystemTray::SystemTray(QWidget *parent)
    : QWidget(parent)
{
    setupUi();
    setupDefaultIcons();
}

SystemTray::~SystemTray()
{
}

void SystemTray::setupUi()
{
    // Main layout for system tray
    m_layout = new QHBoxLayout(this);
    m_layout->setContentsMargins(0, 0, 0, 0);
    m_layout->setSpacing(4);
    m_layout->setAlignment(Qt::AlignRight | Qt::AlignVCenter);
    
    setLayout(m_layout);
}

void SystemTray::setupDefaultIcons()
{
    // Add some default system tray icons
    addNetworkIcon();
    addVolumeIcon();
    addBatteryIcon();
}

void SystemTray::addNetworkIcon()
{
    XenoraButton *networkBtn = new XenoraButton();
    networkBtn->setButtonStyle(XenoraButton::Icon);
    networkBtn->setIcon(QIcon(":/icons/system/wifi.svg"));
    networkBtn->setIconSize(16);
    networkBtn->setFixedSize(24, 24);
    networkBtn->setToolTip("Connected to WiFi");
    
    // Context menu for network settings
    QMenu *networkMenu = new QMenu(this);
    networkMenu->addAction("Open Network Settings");
    networkMenu->addSeparator();
    networkMenu->addAction("WiFi: Connected");
    networkMenu->addAction("Turn Off WiFi");
    
    connect(networkBtn, &XenoraButton::clicked, [=]() {
        networkMenu->popup(networkBtn->mapToGlobal(
            QPoint(0, networkBtn->height())));
    });
    
    m_layout->addWidget(networkBtn);
    m_icons["network"] = networkBtn;
}

void SystemTray::addVolumeIcon()
{
    XenoraButton *volumeBtn = new XenoraButton();
    volumeBtn->setButtonStyle(XenoraButton::Icon);
    volumeBtn->setIcon(QIcon(":/icons/system/volume.svg"));
    volumeBtn->setIconSize(16);
    volumeBtn->setFixedSize(24, 24);
    volumeBtn->setToolTip("Volume: 75%");
    
    // Context menu for volume
    QMenu *volumeMenu = new QMenu(this);
    volumeMenu->addAction("Open Sound Settings");
    volumeMenu->addSeparator();
    
    // Volume slider could be added here in a real implementation
    volumeMenu->addAction("Mute");
    
    connect(volumeBtn, &XenoraButton::clicked, [=]() {
        volumeMenu->popup(volumeBtn->mapToGlobal(
            QPoint(0, volumeBtn->height())));
    });
    
    m_layout->addWidget(volumeBtn);
    m_icons["volume"] = volumeBtn;
}

void SystemTray::addBatteryIcon()
{
    XenoraButton *batteryBtn = new XenoraButton();
    batteryBtn->setButtonStyle(XenoraButton::Icon);
    batteryBtn->setIcon(QIcon(":/icons/system/battery.svg"));
    batteryBtn->setIconSize(16);
    batteryBtn->setFixedSize(24, 24);
    batteryBtn->setToolTip("Battery: 80%");
    
    // Context menu for battery
    QMenu *batteryMenu = new QMenu(this);
    batteryMenu->addAction("80% available (charging)");
    batteryMenu->addSeparator();
    batteryMenu->addAction("Power Settings");
    
    connect(batteryBtn, &XenoraButton::clicked, [=]() {
        batteryMenu->popup(batteryBtn->mapToGlobal(
            QPoint(0, batteryBtn->height())));
    });
    
    m_layout->addWidget(batteryBtn);
    m_icons["battery"] = batteryBtn;
}

void SystemTray::addIcon(const QString &id, const QIcon &icon, const QString &tooltip)
{
    // Don't add if it already exists
    if (m_icons.contains(id)) {
        return;
    }
    
    XenoraButton *btn = new XenoraButton();
    btn->setButtonStyle(XenoraButton::Icon);
    btn->setIcon(icon);
    btn->setIconSize(16);
    btn->setFixedSize(24, 24);
    btn->setToolTip(tooltip);
    
    m_layout->addWidget(btn);
    m_icons[id] = btn;
    
    emit iconAdded(id);
}

void SystemTray::removeIcon(const QString &id)
{
    if (m_icons.contains(id)) {
        QWidget *widget = m_icons[id];
        m_layout->removeWidget(widget);
        widget->deleteLater();
        m_icons.remove(id);
        
        emit iconRemoved(id);
    }
}

void SystemTray::setIconTooltip(const QString &id, const QString &tooltip)
{
    if (m_icons.contains(id)) {
        m_icons[id]->setToolTip(tooltip);
    }
}

void SystemTray::setIconClickHandler(const QString &id, const std::function<void()> &handler)
{
    if (m_icons.contains(id)) {
        XenoraButton *btn = qobject_cast<XenoraButton*>(m_icons[id]);
        if (btn) {
            // Disconnect any previous connections
            disconnect(btn, &XenoraButton::clicked, nullptr, nullptr);
            
            // Connect new handler
            connect(btn, &XenoraButton::clicked, handler);
        }
    }
}

QWidget* SystemTray::getIconWidget(const QString &id)
{
    return m_icons.value(id, nullptr);
}

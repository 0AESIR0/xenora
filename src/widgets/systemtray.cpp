#include "widgets/systemtray.h"

#include <QMenu>
#include <QAction>
#include <QSlider>

SystemTrayButton::SystemTrayButton(const QIcon &icon, QWidget *parent)
    : QPushButton(parent)
{
    setIcon(icon);
    setIconSize(QSize(18, 18));
    setFixedSize(36, 36);
    
    setStyleSheet("QPushButton { background-color: rgba(70, 70, 90, 150); border-radius: 18px; }"
                 "QPushButton:hover { background-color: rgba(80, 80, 110, 180); }");
}

SystemTray::SystemTray(QWidget *parent)
    : QWidget(parent)
{
    m_layout = new QHBoxLayout(this);
    m_layout->setContentsMargins(0, 0, 0, 0);
    m_layout->setSpacing(5);
    
    setLayout(m_layout);
    
    // Sistem tepsisi ikonları
    addWifiButton();
    addVolumeButton();
    addBatteryButton();
    addPowerButton();
}

SystemTray::~SystemTray()
{
}

void SystemTray::addWifiButton()
{
    SystemTrayButton *wifiButton = new SystemTrayButton(QIcon::fromTheme("network-wireless"), this);
    m_layout->addWidget(wifiButton);
    m_buttons.append(wifiButton);
    
    QMenu *wifiMenu = new QMenu(this);
    wifiMenu->setStyleSheet("QMenu { background-color: rgba(40, 40, 60, 240); border: 1px solid rgba(80, 80, 100, 100); border-radius: 10px; }"
                           "QMenu::item { padding: 8px 20px; color: white; }"
                           "QMenu::item:selected { background-color: rgba(80, 80, 120, 200); }");
    
    wifiMenu->addAction("Wi-Fi Ağları");
    wifiMenu->addAction("Bağlantı Ayarları");
    
    connect(wifiButton, &QPushButton::clicked, [wifiButton, wifiMenu]() {
        wifiMenu->exec(wifiButton->mapToGlobal(QPoint(0, wifiButton->height())));
    });
}

void SystemTray::addVolumeButton()
{
    SystemTrayButton *volumeButton = new SystemTrayButton(QIcon::fromTheme("audio-volume-high"), this);
    m_layout->addWidget(volumeButton);
    m_buttons.append(volumeButton);
    
    QMenu *volumeMenu = new QMenu(this);
    volumeMenu->setStyleSheet("QMenu { background-color: rgba(40, 40, 60, 240); border: 1px solid rgba(80, 80, 100, 100); border-radius: 10px; padding: 10px; }"
                             "QMenu::item { padding: 8px 20px; color: white; }"
                             "QMenu::item:selected { background-color: rgba(80, 80, 120, 200); }");
    
    QSlider *volumeSlider = new QSlider(Qt::Horizontal, volumeMenu);
    volumeSlider->setMinimum(0);
    volumeSlider->setMaximum(100);
    volumeSlider->setValue(50);
    volumeSlider->setFixedWidth(150);
    volumeSlider->setStyleSheet("QSlider::groove:horizontal { background: rgba(80, 80, 100, 150); height: 8px; border-radius: 4px; }"
                               "QSlider::handle:horizontal { background: white; width: 18px; margin: -5px 0; border-radius: 9px; }");
    
    QWidgetAction *sliderAction = new QWidgetAction(volumeMenu);
    sliderAction->setDefaultWidget(volumeSlider);
    volumeMenu->addAction(sliderAction);
    
    connect(volumeButton, &QPushButton::clicked, [volumeButton, volumeMenu]() {
        volumeMenu->exec(volumeButton->mapToGlobal(QPoint(0, volumeButton->height())));
    });
}

void SystemTray::addBatteryButton()
{
    SystemTrayButton *batteryButton = new SystemTrayButton(QIcon::fromTheme("battery-full"), this);
    m_layout->addWidget(batteryButton);
    m_buttons.append(batteryButton);
    
    // Sadece görsel amaçlı
}

void SystemTray::addPowerButton()
{
    SystemTrayButton *powerButton = new SystemTrayButton(QIcon(":/images/icons/power.png"), this);
    m_layout->addWidget(powerButton);
    m_buttons.append(powerButton);
    
    QMenu *powerMenu = new QMenu(this);
    powerMenu->setStyleSheet("QMenu { background-color: rgba(40, 40, 60, 240); border: 1px solid rgba(80, 80, 100, 100); border-radius: 10px; }"
                            "QMenu::item { padding: 8px 20px; color: white; }"
                            "QMenu::item:selected { background-color: rgba(80, 80, 120, 200); }");
    
    powerMenu->addAction("Kilitle");
    powerMenu->addAction("Oturumu Kapat");
    powerMenu->addAction("Yeniden Başlat");
    
    QAction *shutdownAction = new QAction("Kapat", powerMenu);
    connect(shutdownAction, &QAction::triggered, []() {
        QApplication::quit();
    });
    powerMenu->addAction(shutdownAction);
    
    connect(powerButton, &QPushButton::clicked, [powerButton, powerMenu]() {
        powerMenu->exec(powerButton->mapToGlobal(QPoint(0, powerButton->height())));
    });
}

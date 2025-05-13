#include "systemtray.h"
#include <QHBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QTime>
#include <QTimer>
#include <QMenu>
#include <QAction>

SystemTray::SystemTray(QWidget *parent)
    : QWidget(parent)
{
    setupTray();
}

SystemTray::~SystemTray()
{
}

void SystemTray::setupTray()
{
    QHBoxLayout *layout = new QHBoxLayout(this);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->setSpacing(10);
    
    // Ses ayarları butonu
    m_volumeButton = new QPushButton(this);
    m_volumeButton->setIcon(QIcon::fromTheme("audio-volume-medium"));
    m_volumeButton->setFlat(true);
    m_volumeButton->setToolTip("Ses Ayarları");
    connect(m_volumeButton, &QPushButton::clicked, this, &SystemTray::showVolumeControl);
    
    // Ağ ayarları butonu
    m_networkButton = new QPushButton(this);
    m_networkButton->setIcon(QIcon::fromTheme("network-wireless"));
    m_networkButton->setFlat(true);
    m_networkButton->setToolTip("Ağ Ayarları");
    connect(m_networkButton, &QPushButton::clicked, this, &SystemTray::showNetworkSettings);
    
    // Batarya durumu (dizüstü bilgisayarlar için)
    m_batteryButton = new QPushButton(this);
    m_batteryButton->setIcon(QIcon::fromTheme("battery-good-charging"));
    m_batteryButton->setFlat(true);
    m_batteryButton->setToolTip("Batarya Durumu");
    connect(m_batteryButton, &QPushButton::clicked, this, &SystemTray::showBatteryInfo);
    
    // Saat göstergesi
    m_clockLabel = new QLabel(this);
    m_clockLabel->setAlignment(Qt::AlignCenter);
    updateClock();
    
    // Saat güncelleme zamanlayıcısı
    QTimer *timer = new QTimer(this);
    connect(timer, &QTimer::timeout, this, &SystemTray::updateClock);
    timer->start(1000);
    
    // Bileşenleri düzene ekle
    layout->addWidget(m_volumeButton);
    layout->addWidget(m_networkButton);
    layout->addWidget(m_batteryButton);
    layout->addWidget(m_clockLabel);
    
    setLayout(layout);
}

void SystemTray::updateClock()
{
    QTime time = QTime::currentTime();
    QString text = time.toString("hh:mm");
    m_clockLabel->setText(text);
}

void SystemTray::showVolumeControl()
{
    // Basit ses kontrol menüsü
    QMenu *menu = new QMenu(this);
    menu->setAttribute(Qt::WA_DeleteOnClose);
    
    QAction *muteAction = menu->addAction("Sessiz");
    muteAction->setCheckable(true);
    
    menu->addSeparator();
    QAction *settingsAction = menu->addAction("Ses Ayarları...");
    
    menu->exec(mapToGlobal(m_volumeButton->pos() + QPoint(0, -menu->sizeHint().height())));
}

void SystemTray::showNetworkSettings()
{
    // Ağ ayarları menüsü
    QMenu *menu = new QMenu(this);
    menu->setAttribute(Qt::WA_DeleteOnClose);
    
    QAction *wifiAction = menu->addAction("Wi-Fi");
    wifiAction->setCheckable(true);
    wifiAction->setChecked(true);
    
    menu->addSeparator();
    QAction *settingsAction = menu->addAction("Ağ Ayarları...");
    
    menu->exec(mapToGlobal(m_networkButton->pos() + QPoint(0, -menu->sizeHint().height())));
}

void SystemTray::showBatteryInfo()
{
    // Batarya bilgisi menüsü
    QMenu *menu = new QMenu(this);
    menu->setAttribute(Qt::WA_DeleteOnClose);
    
    QAction *infoAction = menu->addAction("Batarya: %75");
    infoAction->setEnabled(false);
    
    menu->addSeparator();
    QAction *settingsAction = menu->addAction("Güç Ayarları...");
    
    menu->exec(mapToGlobal(m_batteryButton->pos() + QPoint(0, -menu->sizeHint().height())));
}

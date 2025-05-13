#include "startmenu.h"
#include <QPainter>
#include <QApplication>
#include <QScreen>
#include <QDebug>
#include <QDir>

StartMenu::StartMenu(QWidget *parent)
    : QWidget(parent, Qt::FramelessWindowHint | Qt::WindowStaysOnTopHint),
      m_mainLayout(nullptr),
      m_userSection(nullptr),
      m_appsSection(nullptr),
      m_powerSection(nullptr),
      m_userAvatar(nullptr),
      m_userName(nullptr),
      m_settingsButton(nullptr),
      m_powerButton(nullptr)
{
    setAttribute(Qt::WA_TranslucentBackground);
    setObjectName("StartMenu");
    
    // Set fixed size for the start menu
    setFixedSize(350, 500);
    
    initializeMenu();
}

StartMenu::~StartMenu() {
    // Clean up resources
}

void StartMenu::initializeMenu() {
    // Create main layout
    m_mainLayout = new QVBoxLayout(this);
    m_mainLayout->setContentsMargins(10, 10, 10, 10);
    m_mainLayout->setSpacing(10);
    
    // Create sections
    createUserSection();
    createAppsSection();
    createPowerSection();
    
    // Add sections to main layout
    m_mainLayout->addWidget(m_userSection);
    m_mainLayout->addWidget(m_appsSection, 1);
    m_mainLayout->addWidget(m_powerSection);
    
    // Set the layout
    setLayout(m_mainLayout);
}

void StartMenu::createUserSection() {
    m_userSection = new QWidget(this);
    m_userSection->setObjectName("UserSection");
    
    QHBoxLayout *layout = new QHBoxLayout(m_userSection);
    layout->setContentsMargins(10, 10, 10, 10);
    
    // User avatar
    m_userAvatar = new QLabel(m_userSection);
    m_userAvatar->setObjectName("UserAvatar");
    m_userAvatar->setFixedSize(48, 48);
    m_userAvatar->setScaledContents(true);
    
    // User name
    m_userName = new QLabel("Xenora User", m_userSection);
    m_userName->setObjectName("UserName");
    
    // Add to layout
    layout->addWidget(m_userAvatar);
    layout->addWidget(m_userName, 1);
    
    m_userSection->setLayout(layout);
}

void StartMenu::createAppsSection() {
    m_appsSection = new QWidget(this);
    m_appsSection->setObjectName("AppsSection");
    
    QVBoxLayout *layout = new QVBoxLayout(m_appsSection);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->setSpacing(5);
    
    QString iconBasePath = QDir::currentPath() + "/resources/icons/";
    
    // Add app buttons
    addAppButton("Home", iconBasePath + "home.png");
    addAppButton("Documents", iconBasePath + "documents.png");
    addAppButton("Pictures", iconBasePath + "pictures.png");
    addAppButton("Music", iconBasePath + "music.png");
    addAppButton("Videos", iconBasePath + "videos.png");
    addAppButton("Apps", iconBasePath + "apps.png");
    
    // Add settings button
    m_settingsButton = new QPushButton("Settings", m_appsSection);
    m_settingsButton->setObjectName("SettingsButton");
    m_settingsButton->setIcon(QIcon(iconBasePath + "settings.png"));
    m_settingsButton->setIconSize(QSize(24, 24));
    connect(m_settingsButton, &QPushButton::clicked, this, &StartMenu::onSettingsButtonClicked);
    
    layout->addWidget(m_settingsButton);
    layout->addStretch(1);
    
    m_appsSection->setLayout(layout);
}

void StartMenu::createPowerSection() {
    m_powerSection = new QWidget(this);
    m_powerSection->setObjectName("PowerSection");
    
    QHBoxLayout *layout = new QHBoxLayout(m_powerSection);
    layout->setContentsMargins(0, 0, 0, 0);
    
    // Power button
    m_powerButton = new QPushButton("Power", m_powerSection);
    m_powerButton->setObjectName("PowerButton");
    m_powerButton->setIcon(QIcon(QDir::currentPath() + "/resources/icons/power.png"));
    m_powerButton->setIconSize(QSize(24, 24));
    connect(m_powerButton, &QPushButton::clicked, this, &StartMenu::onPowerButtonClicked);
    
    layout->addStretch(1);
    layout->addWidget(m_powerButton);
    
    m_powerSection->setLayout(layout);
}

void StartMenu::addAppButton(const QString &name, const QString &iconPath) {
    QString finalIconPath = iconPath;
    if (!QFile::exists(finalIconPath)) {
        // Fallback to resources folder
        finalIconPath = QDir::currentPath() + "/resources/icons/" + QFileInfo(iconPath).fileName();
    }
    
    QPushButton *button = new QPushButton(name, m_appsSection);
    button->setObjectName("AppButton");
    button->setIcon(QIcon(finalIconPath));
    button->setIconSize(QSize(24, 24));
    button->setProperty("appName", name);
    
    connect(button, &QPushButton::clicked, this, &StartMenu::onAppButtonClicked);
    
    m_appButtons.append(button);
    m_appsSection->layout()->addWidget(button);
}

void StartMenu::onAppButtonClicked() {
    QPushButton *button = qobject_cast<QPushButton*>(sender());
    if (button) {
        QString appName = button->property("appName").toString();
        emit appLaunched(appName);
        hide(); // Hide start menu after launching an app
    }
}

void StartMenu::onSettingsButtonClicked() {
    emit appLaunched("Settings");
    hide();
}

void StartMenu::onPowerButtonClicked() {
    emit powerAction("PowerMenu");
    hide();
}

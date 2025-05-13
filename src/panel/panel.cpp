#include "panel.h"
#include <QScreen>
#include <QApplication>
#include <QDebug>
#include <QPainter>
#include <QDateTime>
#include <QTimer>
#include <QDir>

Panel::Panel(QWidget *parent)
    : QWidget(parent, Qt::FramelessWindowHint | Qt::WindowStaysOnTopHint),
      m_panelWidget(nullptr),
      m_startButton(nullptr),
      m_clockLabel(nullptr),
      m_systemTray(nullptr),
      m_layout(nullptr),
      m_startMenuVisible(false)
{
    setAttribute(Qt::WA_TranslucentBackground);
    setWindowTitle("XenoraOS Panel");
    
    initializePanel();
    
    // Position panel at the bottom of the screen
    updateGeometry(QApplication::primaryScreen()->geometry());
}

Panel::~Panel() {
    // Clean up resources
}

void Panel::initializePanel() {
    // Create main layout
    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(0, 0, 0, 0);
    mainLayout->setSpacing(0);
    
    // Create panel widget
    m_panelWidget = new QWidget(this);
    m_panelWidget->setObjectName("XenoraPanel");
    
    // Create panel layout
    m_layout = new QHBoxLayout(m_panelWidget);
    m_layout->setContentsMargins(10, 5, 10, 5);
    m_layout->setSpacing(10);
    
    // Create panel elements
    setupStartButton();
    
    // Create system tray
    m_systemTray = new SystemTray(m_panelWidget);
    
    // Create clock
    setupClock();
    
    // Add elements to panel layout
    m_layout->addWidget(m_startButton);
    m_layout->addStretch(1);
    m_layout->addWidget(m_systemTray);
    m_layout->addWidget(m_clockLabel);
    
    // Add panel widget to main layout
    mainLayout->addWidget(m_panelWidget);
    
    // Set the layout for this widget
    setLayout(mainLayout);
}

void Panel::setupStartButton() {
    m_startButton = new QPushButton(m_panelWidget);
    m_startButton->setObjectName("StartButton");
    m_startButton->setFixedSize(40, 40);
    m_startButton->setIcon(QIcon(QDir::currentPath() + "/resources/icons/start.png"));
    m_startButton->setIconSize(QSize(24, 24));
    m_startButton->setToolTip("Start Menu");
    
    connect(m_startButton, &QPushButton::clicked, this, &Panel::onStartButtonClicked);
}

void Panel::setupClock() {
    m_clockLabel = new QLabel(m_panelWidget);
    m_clockLabel->setObjectName("ClockLabel");
    m_clockLabel->setAlignment(Qt::AlignCenter);
    
    // Update clock immediately
    updateClock();
    
    // Update clock every second
    QTimer *timer = new QTimer(this);
    connect(timer, &QTimer::timeout, this, &Panel::updateClock);
    timer->start(1000);
}

void Panel::updateGeometry(const QRect &screenGeometry) {
    // Panel height is 50 pixels
    int panelHeight = 50;
    
    // Set panel width to screen width
    int panelWidth = screenGeometry.width();
    
    // Position panel at the bottom of the screen
    int panelX = screenGeometry.x();
    int panelY = screenGeometry.height() - panelHeight;
    
    // Set geometry
    setGeometry(panelX, panelY, panelWidth, panelHeight);
}

void Panel::onStartButtonClicked() {
    m_startMenuVisible = !m_startMenuVisible;
    emit startMenuToggled(m_startMenuVisible);
}

void Panel::updateClock() {
    QDateTime now = QDateTime::currentDateTime();
    m_clockLabel->setText(now.toString("hh:mm:ss"));
}

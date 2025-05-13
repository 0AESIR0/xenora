#include "Panel.h"
#include "StyleManager.h"
#include "../widgets/XenoraWindow.h"

#include <QLabel>
#include <QTime>
#include <QTimer>
#include <QPainter>
#include <QGraphicsDropShadowEffect>
#include <QDebug>

Panel::Panel(QWidget *parent)
    : QWidget(parent),
      m_systemTray(nullptr)
{
    setAttribute(Qt::WA_TranslucentBackground);
    setWindowFlags(Qt::FramelessWindowHint | Qt::WindowStaysOnTopHint);
    
    setupUi();
    
    // Set up a timer to update the clock
    QTimer *timer = new QTimer(this);
    connect(timer, &QTimer::timeout, [this]() {
        QTime time = QTime::currentTime();
        QLabel *clockLabel = qobject_cast<QLabel*>(m_clockWidget->children().at(0));
        if (clockLabel) {
            clockLabel->setText(time.toString("HH:mm"));
        }
    });
    timer->start(1000); // Update every second
}

Panel::~Panel()
{
}

void Panel::refresh()
{
    // Update panel styling
    m_panel->setBackgroundColor(StyleManager::panelColor());
    m_panel->setCornerRadius(StyleManager::cornerRadius());
    
    // Force repaint
    update();
}

void Panel::setSystemTray(SystemTray* systemTray)
{
    m_systemTray = systemTray;
    if (m_systemTray && m_systemTrayLayout) {
        m_systemTrayLayout->addWidget(m_systemTray);
    }
}

void Panel::toggleAppLauncher()
{
    // This will be connected to the actual app launcher in the desktop environment
    qDebug() << "Toggle app launcher";
}

void Panel::handleWindowAdded(XenoraWindow* window)
{
    if (!window) return;
    
    // Create a taskbar button for this window
    XenoraButton *taskButton = new XenoraButton();
    taskButton->setButtonStyle(XenoraButton::Panel);
    taskButton->setText(window->title());
    taskButton->setMinimumWidth(150);
    taskButton->setMaximumWidth(200);
    
    // Connect window signals to update the taskbar button
    connect(window, &XenoraWindow::titleChanged, taskButton, &XenoraButton::setText);
    
    // Connect button click to focus the window
    connect(taskButton, &XenoraButton::clicked, [window]() {
        if (window->isMinimized()) {
            window->showNormal();
        } else {
            window->activateWindow();
            window->raise();
        }
    });
    
    // Store the window pointer as property on the button
    taskButton->setProperty("window_ptr", QVariant::fromValue(static_cast<void*>(window)));
    
    // Add to taskbar
    m_taskbarLayout->addWidget(taskButton);
}

void Panel::handleWindowRemoved(XenoraWindow* window)
{
    // Find and remove the corresponding taskbar button
    for(int i = 0; i < m_taskbarLayout->count(); ++i) {
        QWidget *widget = m_taskbarLayout->itemAt(i)->widget();
        if (widget) {
            XenoraButton *button = qobject_cast<XenoraButton*>(widget);
            if (button) {
                void* ptr = button->property("window_ptr").value<void*>();
                if (ptr == static_cast<void*>(window)) {
                    m_taskbarLayout->removeWidget(button);
                    button->deleteLater();
                    break;
                }
            }
        }
    }
}

void Panel::paintEvent(QPaintEvent *event)
{
    // Let the base class handle the painting
    QWidget::paintEvent(event);
}

void Panel::showEvent(QShowEvent *event)
{
    QWidget::showEvent(event);
    
    // Refresh the panel when shown
    refresh();
}

void Panel::setupUi()
{
    // Main layout
    m_layout = new QHBoxLayout(this);
    m_layout->setContentsMargins(10, 5, 10, 5);
    m_layout->setSpacing(8);
    
    // Create the actual panel widget with blur effects
    m_panel = new XenoraPanel(this);
    m_panel->setBackgroundColor(StyleManager::panelColor());
    m_panel->setCornerRadius(StyleManager::cornerRadius());
    m_panel->setBlurEnabled(true);
    m_panel->setBlurRadius(30);
    
    // Add shadow effect
    QGraphicsDropShadowEffect *shadow = new QGraphicsDropShadowEffect(this);
    shadow->setBlurRadius(20);
    shadow->setColor(QColor(0, 0, 0, 60));
    shadow->setOffset(0, 2);
    m_panel->setGraphicsEffect(shadow);
    
    // Setup components
    setupStartButton();
    setupTaskbar();
    setupSystemTrayArea();
    setupClock();
    
    // Set panel to fill the entire widget
    m_panel->setGeometry(this->rect());
    
    // Connect resize events to update panel size
    connect(this, &Panel::resized, [this]() {
        m_panel->setGeometry(this->rect());
    });
}

void Panel::setupStartButton()
{
    m_startButtonContainer = new QWidget();
    QHBoxLayout *startLayout = new QHBoxLayout(m_startButtonContainer);
    startLayout->setContentsMargins(0, 0, 0, 0);
    
    m_startButton = new XenoraButton();
    m_startButton->setButtonStyle(XenoraButton::Icon);
    m_startButton->setIcon(QIcon(":/icons/system/start.svg"));
    m_startButton->setIconSize(24);
    m_startButton->setFixedSize(36, 36);
    
    // Connect start button to app launcher
    connect(m_startButton, &XenoraButton::clicked, this, &Panel::toggleAppLauncher);
    
    startLayout->addWidget(m_startButton);
    m_layout->addWidget(m_startButtonContainer);
}

void Panel::setupTaskbar()
{
    m_taskbarContainer = new QWidget();
    m_taskbarContainer->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
    
    m_taskbarLayout = new QHBoxLayout(m_taskbarContainer);
    m_taskbarLayout->setContentsMargins(0, 0, 0, 0);
    m_taskbarLayout->setSpacing(4);
    m_taskbarLayout->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
    
    m_layout->addWidget(m_taskbarContainer, 1);
}

void Panel::setupSystemTrayArea()
{
    m_systemTrayContainer = new QWidget();
    
    m_systemTrayLayout = new QHBoxLayout(m_systemTrayContainer);
    m_systemTrayLayout->setContentsMargins(0, 0, 0, 0);
    m_systemTrayLayout->setSpacing(4);
    
    // Add system tray if available
    if (m_systemTray) {
        m_systemTrayLayout->addWidget(m_systemTray);
    }
    
    m_layout->addWidget(m_systemTrayContainer);
}

void Panel::setupClock()
{
    m_clockWidget = new QWidget();
    QHBoxLayout *clockLayout = new QHBoxLayout(m_clockWidget);
    clockLayout->setContentsMargins(5, 0, 5, 0);
    
    QLabel *clockLabel = new QLabel(QTime::currentTime().toString("HH:mm"));
    clockLabel->setAlignment(Qt::AlignCenter);
    clockLabel->setStyleSheet("font-size: 14px; font-weight: bold;");
    
    clockLayout->addWidget(clockLabel);
    m_layout->addWidget(m_clockWidget);
}

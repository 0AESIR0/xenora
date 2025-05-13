#include "Panel.h"
#include "../widgets/XenoraButton.h"
#include "../widgets/XenoraWindow.h"
#include "StyleManager.h"

#include <QPainter>
#include <QTime>
#include <QTimer>
#include <QLabel>
#include <QDebug>

Panel::Panel(QWidget *parent)
    : QWidget(parent),
      m_systemTray(nullptr)
{
    setWindowFlags(Qt::FramelessWindowHint | Qt::WindowStaysOnTopHint);
    setAttribute(Qt::WA_TranslucentBackground);
    
    setupUi();
    
    // Update the clock every second
    QTimer *timer = new QTimer(this);
    connect(timer, &QTimer::timeout, [this]() {
        QTime currentTime = QTime::currentTime();
        m_clockWidget->setText(currentTime.toString("HH:mm"));
    });
    timer->start(1000);
}

Panel::~Panel()
{
}

void Panel::refresh()
{
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
    // Emit a signal to the app launcher or handle it directly
    qDebug() << "App launcher toggle requested";
    
    // Will be connected to App Launcher's show/hide method
    emit m_startButton->toggled(m_startButton->isChecked());
}

void Panel::handleWindowAdded(XenoraWindow* window)
{
    if (!window) return;
    
    // Create task button for the window
    XenoraButton *taskButton = new XenoraButton(this);
    taskButton->setButtonStyle(XenoraButton::Panel);
    taskButton->setText(window->title());
    taskButton->setProperty("windowId", QVariant::fromValue(static_cast<void*>(window)));
    
    // Add to taskbar
    m_taskbarLayout->addWidget(taskButton);
    
    // Connect click to window focus
    connect(taskButton, &XenoraButton::clicked, [window]() {
        window->show();
        window->raise();
        window->activateWindow();
    });
}

void Panel::handleWindowRemoved(XenoraWindow* window)
{
    if (!window) return;
    
    // Find and remove the task button for this window
    for (int i = 0; i < m_taskbarLayout->count(); ++i) {
        QWidget* widget = m_taskbarLayout->itemAt(i)->widget();
        XenoraButton* button = qobject_cast<XenoraButton*>(widget);
        
        if (button && button->property("windowId").value<void*>() == window) {
            m_taskbarLayout->removeWidget(button);
            delete button;
            break;
        }
    }
}

void Panel::paintEvent(QPaintEvent *event)
{
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);
    
    // Draw panel background with blur and transparency
    QColor panelColor = StyleManager::currentTheme().panelColor;
    painter.setPen(Qt::NoPen);
    painter.setBrush(panelColor);
    
    // Draw panel as rounded rectangle
    int cornerRadius = StyleManager::cornerRadius();
    QRect panelRect = rect();
    
    // Only round the top corners
    QPainterPath path;
    path.setFillRule(Qt::WindingFill);
    path.addRoundedRect(panelRect, cornerRadius, cornerRadius);
    
    painter.drawPath(path);
    
    QWidget::paintEvent(event);
}

void Panel::showEvent(QShowEvent *event)
{
    QWidget::showEvent(event);
    
    // Make sure the panel is on top
    raise();
}

void Panel::setupUi()
{
    m_panel = new XenoraPanel(this);
    m_panel->setBackgroundColor(StyleManager::currentTheme().panelColor);
    m_panel->setCornerRadius(StyleManager::cornerRadius());
    m_panel->setBlurEnabled(true);
    
    // Main layout
    m_layout = new QHBoxLayout(this);
    m_layout->setContentsMargins(8, 4, 8, 4);
    m_layout->setSpacing(6);
    
    setupStartButton();
    setupTaskbar();
    setupSystemTrayArea();
    setupClock();
    
    m_layout->addWidget(m_startButtonContainer);
    m_layout->addWidget(m_taskbarContainer, 1);
    m_layout->addWidget(m_systemTrayContainer);
    m_layout->addWidget(m_clockWidget);
    
    setLayout(m_layout);
}

void Panel::setupStartButton()
{
    m_startButtonContainer = new QWidget(this);
    QHBoxLayout *startLayout = new QHBoxLayout(m_startButtonContainer);
    startLayout->setContentsMargins(0, 0, 0, 0);
    
    m_startButton = new XenoraButton(this);
    m_startButton->setIcon(QIcon(":/icons/system/start.svg"));
    m_startButton->setIconSize(QSize(24, 24));
    m_startButton->setFixedSize(40, 40);
    m_startButton->setButtonStyle(XenoraButton::Panel);
    m_startButton->setCheckable(true);
    
    connect(m_startButton, &XenoraButton::clicked, this, &Panel::toggleAppLauncher);
    
    startLayout->addWidget(m_startButton);
    m_startButtonContainer->setLayout(startLayout);
}

void Panel::setupTaskbar()
{
    m_taskbarContainer = new QWidget(this);
    m_taskbarLayout = new QHBoxLayout(m_taskbarContainer);
    m_taskbarLayout->setContentsMargins(0, 0, 0, 0);
    m_taskbarLayout->setSpacing(4);
    m_taskbarLayout->addStretch(1); // Align tasks to the left
    
    m_taskbarContainer->setLayout(m_taskbarLayout);
}

void Panel::setupSystemTrayArea()
{
    m_systemTrayContainer = new QWidget(this);
    m_systemTrayLayout = new QHBoxLayout(m_systemTrayContainer);
    m_systemTrayLayout->setContentsMargins(0, 0, 0, 0);
    m_systemTrayLayout->setSpacing(4);
    
    m_systemTrayContainer->setLayout(m_systemTrayLayout);
}

void Panel::setupClock()
{
    m_clockWidget = new QLabel(this);
    m_clockWidget->setAlignment(Qt::AlignCenter);
    m_clockWidget->setText(QTime::currentTime().toString("HH:mm"));
    m_clockWidget->setMinimumWidth(70);
    
    // Set style for clock
    m_clockWidget->setStyleSheet(
        "font-size: 14px;"
        "color: " + StyleManager::currentTheme().textColor.name() + ";"
        "padding: 0 8px;"
    );
}

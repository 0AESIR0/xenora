#include "panel/panel.h"

#include <QPainter>
#include <QMouseEvent>
#include <QMenu>
#include <QAction>
#include <QProcess>
#include <QTimer>
#include <QDateTime>
#include <QGraphicsDropShadowEffect>

Panel::Panel(QWidget *parent)
    : QWidget(parent)
{
    // Panel stili
    setFixedHeight(height());
    
    // Gölge efekti
    QGraphicsDropShadowEffect *shadowEffect = new QGraphicsDropShadowEffect(this);
    shadowEffect->setBlurRadius(10);
    shadowEffect->setColor(QColor(0, 0, 0, 160));
    shadowEffect->setOffset(0, 0);
    setGraphicsEffect(shadowEffect);
    
    // Panel düzeni
    m_layout = new QHBoxLayout(this);
    m_layout->setContentsMargins(5, 2, 5, 2);
    m_layout->setSpacing(4);
    
    // Başlat düğmesi
    m_startButton = new QPushButton(this);
    m_startButton->setIcon(QIcon(":/images/icons/home.png"));
    m_startButton->setIconSize(QSize(32, 32));
    m_startButton->setFixedSize(40, 40);
    m_startButton->setStyleSheet("QPushButton { background-color: rgba(60, 60, 80, 180); border-radius: 20px; }");
    connect(m_startButton, &QPushButton::clicked, this, &Panel::startMenuClicked);
    
    // Görev çubuğu
    m_taskbar = new Taskbar(this);
    
    // Sistem tepsisi
    m_systemTray = new SystemTray(this);
    
    // Saat
    m_clockLabel = new QLabel(this);
    m_clockLabel->setAlignment(Qt::AlignCenter);
    m_clockLabel->setStyleSheet("color: white; font-size: 12px;");
    updateClock();
    
    QTimer *timer = new QTimer(this);
    connect(timer, &QTimer::timeout, this, &Panel::clockTick);
    timer->start(1000);
    
    // Düzene ekle
    m_layout->addWidget(m_startButton);
    m_layout->addSpacing(5);
    m_layout->addWidget(m_taskbar, 1);
    m_layout->addWidget(m_systemTray);
    m_layout->addWidget(m_clockLabel);
    
    setLayout(m_layout);
}

Panel::~Panel()
{
}

void Panel::paintEvent(QPaintEvent *event)
{
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);
    
    // Panel arkaplanı
    QLinearGradient gradient(0, 0, 0, height());
    gradient.setColorAt(0, QColor(60, 60, 80, 220));
    gradient.setColorAt(1, QColor(40, 40, 60, 220));
    
    painter.setPen(Qt::NoPen);
    painter.setBrush(gradient);
    painter.drawRoundedRect(rect(), 15, 15);
}

void Panel::mousePressEvent(QMouseEvent *event)
{
    QWidget::mousePressEvent(event);
}

void Panel::createStartMenu()
{
    QMenu *menu = new QMenu(this);
    menu->setStyleSheet("QMenu { background-color: rgba(40, 40, 60, 240); border: 1px solid rgba(80, 80, 100, 100); border-radius: 10px; }"
                       "QMenu::item { padding: 8px 20px; color: white; }"
                       "QMenu::item:selected { background-color: rgba(80, 80, 120, 200); }");
    
    QAction *terminalAction = new QAction("Terminal", menu);
    connect(terminalAction, &QAction::triggered, [this]() {
        QProcess::startDetached("x-terminal-emulator", QStringList());
    });
    
    QAction *settingsAction = new QAction("Ayarlar", menu);
    
    QAction *logoutAction = new QAction("Çıkış", menu);
    connect(logoutAction, &QAction::triggered, []() {
        QApplication::quit();
    });
    
    menu->addAction(terminalAction);
    menu->addAction(settingsAction);
    menu->addSeparator();
    menu->addAction(logoutAction);
    
    menu->exec(mapToGlobal(m_startButton->pos() + QPoint(0, m_startButton->height())));
    menu->deleteLater();
}

void Panel::startMenuClicked()
{
    createStartMenu();
}

void Panel::clockTick()
{
    updateClock();
}

void Panel::updateClock()
{
    QDateTime now = QDateTime::currentDateTime();
    m_clockLabel->setText(now.toString("HH:mm"));
}

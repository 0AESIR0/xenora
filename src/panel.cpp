#include "panel.h"
#include <QPainter>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QAction>
#include <QApplication>
#include <QProcess> // QProcess sınıfını içe aktarın
#include <QScreen>
#include <QPropertyAnimation>
#include <QMouseEvent>
#include <QGraphicsDropShadowEffect>

Panel::Panel(QWidget *parent)
    : QWidget(parent)
    , m_position(Qt::BottomEdge)
    , m_autoHide(false)
    , m_panelSize(48)
    , m_hiddenSize(5)
{
    setAutoFillBackground(false);
    setAttribute(Qt::WA_TranslucentBackground);
    setMouseTracking(true);
    
    // Gölge efekti ekle
    QGraphicsDropShadowEffect* shadowEffect = new QGraphicsDropShadowEffect(this);
    shadowEffect->setBlurRadius(20);
    shadowEffect->setColor(QColor(0, 0, 0, 160));
    shadowEffect->setOffset(0, 0);
    setGraphicsEffect(shadowEffect);
    
    // Panel otomatik gizleme işlemi için zamanlayıcı
    m_hideTimer = new QTimer(this);
    m_hideTimer->setInterval(500);
    connect(m_hideTimer, &QTimer::timeout, this, &Panel::checkMousePosition);
    
    setupUI();
    setupStartMenu();
    updatePanelGeometry();
}

Panel::~Panel()
{
}

void Panel::setupUI()
{
    // Panel bileşenleri
    QHBoxLayout *layout = new QHBoxLayout(this);
    layout->setContentsMargins(5, 0, 5, 0);
    layout->setSpacing(5);
    
    // Başlat butonu
    m_startButton = new QPushButton("", this);
    m_startButton->setIcon(QIcon(":/resources/start-icon.png"));
    m_startButton->setIconSize(QSize(32, 32));
    m_startButton->setFixedSize(m_panelSize - 8, m_panelSize - 8);
    m_startButton->setToolTip("XenoraOS Başlat Menüsü");
    connect(m_startButton, &QPushButton::clicked, this, &Panel::showStartMenu);
    
    // Başlatıcı (Hızlı erişim uygulamaları)
    m_launcher = new Launcher(this);
    
    // Görev çubuğu
    m_taskbar = new Taskbar(this);
    
    // Sistem tepsisi
    m_systemTray = new SystemTray(this);
    
    // Bileşenleri düzene ekle
    layout->addWidget(m_startButton);
    layout->addWidget(m_launcher);
    layout->addWidget(m_taskbar, 1);
    layout->addWidget(m_systemTray);
    
    setLayout(layout);
}

void Panel::setupStartMenu()
{
    m_startMenu = new QMenu(this);
    m_startMenu->setAttribute(Qt::WA_TranslucentBackground);
    m_startMenu->setStyleSheet(
        "QMenu { background-color: rgba(20, 30, 60, 0.95); border-radius: 10px; border: 1px solid rgba(120, 80, 255, 0.5); }"
        "QMenu::item { padding: 8px 25px 8px 35px; color: #e0e5ff; }"
        "QMenu::item:selected { background-color: rgba(120, 80, 255, 0.3); border-radius: 5px; }"
        "QMenu::separator { height: 1px; background-color: rgba(120, 80, 255, 0.5); margin: 5px 10px; }"
    );
    
    // Uygulama kategorileri
    QMenu *appsMenu = new QMenu("Uygulamalar", m_startMenu);
    appsMenu->setIcon(QIcon::fromTheme("applications-system"));
    appsMenu->addAction(QIcon::fromTheme("system-file-manager"), "Dosya Yöneticisi");
    appsMenu->addAction(QIcon::fromTheme("utilities-terminal"), "Terminal");
    appsMenu->addAction(QIcon::fromTheme("web-browser"), "Web Tarayıcı");
    appsMenu->addAction(QIcon::fromTheme("accessories-text-editor"), "Metin Düzenleyici");
    
    // Ana menü öğeleri
    QAction *userAction = m_startMenu->addAction(QIcon::fromTheme("user-info"), "Kullanıcı: " + qgetenv("USER"));
    userAction->setEnabled(false);
    
    m_startMenu->addMenu(appsMenu);
    m_startMenu->addAction(QIcon::fromTheme("preferences-system"), "Ayarlar");
    m_startMenu->addAction(QIcon::fromTheme("system-search"), "Ara");
    
    m_startMenu->addSeparator();
    
    QAction *autoHideAction = m_startMenu->addAction("Paneli Otomatik Gizle");
    autoHideAction->setCheckable(true);
    autoHideAction->setChecked(m_autoHide);
    connect(autoHideAction, &QAction::toggled, this, &Panel::toggleAutoHide);
    
    m_startMenu->addSeparator();
    
    QAction *lockAction = m_startMenu->addAction(QIcon::fromTheme("system-lock-screen"), "Ekranı Kilitle");
    QAction *logoutAction = m_startMenu->addAction(QIcon::fromTheme("system-log-out"), "Oturumu Kapat");
    QAction *powerAction = m_startMenu->addAction(QIcon::fromTheme("system-shutdown"), "Kapat");
    
    // Çıkış ve kapatma işlemleri
    connect(powerAction, &QAction::triggered, []() {
        QProcess::startDetached("systemctl", QStringList() << "poweroff");
    });
    
    connect(logoutAction, &QAction::triggered, []() {
        QProcess::startDetached("pkill", QStringList() << "-KILL" << "xenora-ui");
    });
}

void Panel::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event);
    
    QPainter painter(this);
    
    // Yarı şeffaf efekt için
    painter.setRenderHint(QPainter::Antialiasing);
    
    // Panel arkaplanı
    QColor backgroundColor(20, 30, 60, 230);
    QLinearGradient gradient;
    
    if (m_position == Qt::BottomEdge || m_position == Qt::TopEdge) {
        gradient = QLinearGradient(0, 0, width(), 0);
    } else {
        gradient = QLinearGradient(0, 0, 0, height());
    }
    
    gradient.setColorAt(0.0, backgroundColor);
    gradient.setColorAt(0.5, backgroundColor.lighter(120));
    gradient.setColorAt(1.0, backgroundColor);
    
    painter.setPen(Qt::NoPen);
    painter.setBrush(gradient);
    
    // Kenarları yuvarlak panel
    painter.drawRoundedRect(rect(), 10, 10);
    
    // Neon çizgi efekti
    painter.setPen(QPen(QColor(120, 80, 255, 180), 2));
    
    if (m_position == Qt::BottomEdge) {
        painter.drawLine(10, 0, width() - 10, 0);
    } else if (m_position == Qt::TopEdge) {
        painter.drawLine(10, height() - 1, width() - 10, height() - 1);
    } else if (m_position == Qt::LeftEdge) {
        painter.drawLine(width() - 1, 10, width() - 1, height() - 10);
    } else if (m_position == Qt::RightEdge) {
        painter.drawLine(0, 10, 0, height() - 10);
    }
}

void Panel::showStartMenu()
{
    QPoint pos;
    
    if (m_position == Qt::BottomEdge) {
        pos = mapToGlobal(QPoint(m_startButton->x(), -m_startMenu->sizeHint().height()));
    } else if (m_position == Qt::TopEdge) {
        pos = mapToGlobal(QPoint(m_startButton->x(), height()));
    } else if (m_position == Qt::LeftEdge) {
        pos = mapToGlobal(QPoint(width(), m_startButton->y()));
    } else {
        pos = mapToGlobal(QPoint(-m_startMenu->sizeHint().width(), m_startButton->y()));
    }
    
    m_startMenu->popup(pos);
}

void Panel::setPosition(Qt::Edge position)
{
    if (m_position != position) {
        m_position = position;
        updatePanelGeometry();
        update();
    }
}

void Panel::updatePanelGeometry()
{
    QScreen *screen = QGuiApplication::primaryScreen();
    QRect screenGeometry = screen->geometry();
    
    if (m_position == Qt::BottomEdge) {
        setGeometry(0, screenGeometry.height() - m_panelSize, screenGeometry.width(), m_panelSize);
    } else if (m_position == Qt::TopEdge) {
        setGeometry(0, 0, screenGeometry.width(), m_panelSize);
    } else if (m_position == Qt::LeftEdge) {
        setGeometry(0, 0, m_panelSize, screenGeometry.height());
        
        // Sol kenarda dikey düzen kullan
        QVBoxLayout *layout = new QVBoxLayout(this);
        layout->setContentsMargins(0, 5, 0, 5);
        layout->setSpacing(5);
        layout->addWidget(m_startButton);
        layout->addWidget(m_launcher);
        layout->addWidget(m_taskbar, 1);
        layout->addWidget(m_systemTray);
        setLayout(layout);
    } else if (m_position == Qt::RightEdge) {
        setGeometry(screenGeometry.width() - m_panelSize, 0, m_panelSize, screenGeometry.height());
        
        // Sağ kenarda dikey düzen kullan
        QVBoxLayout *layout = new QVBoxLayout(this);
        layout->setContentsMargins(0, 5, 0, 5);
        layout->setSpacing(5);
        layout->addWidget(m_startButton);
        layout->addWidget(m_launcher);
        layout->addWidget(m_taskbar, 1);
        layout->addWidget(m_systemTray);
        setLayout(layout);
    }
}

void Panel::setAutoHide(bool enabled)
{
    m_autoHide = enabled;
    
    if (m_autoHide) {
        m_hideTimer->start();
    } else {
        m_hideTimer->stop();
        
        // Paneli tam görünür yap
        QPropertyAnimation *animation = new QPropertyAnimation(this, "geometry");
        animation->setDuration(250);
        animation->setEasingCurve(QEasingCurve::OutCubic);
        
        QRect endGeometry = geometry();
        if (m_position == Qt::BottomEdge) {
            endGeometry.setY(parentWidget()->height() - m_panelSize);
        } else if (m_position == Qt::TopEdge) {
            endGeometry.setY(0);
        } else if (m_position == Qt::LeftEdge) {
            endGeometry.setX(0);
        } else if (m_position == Qt::RightEdge) {
            endGeometry.setX(parentWidget()->width() - m_panelSize);
        }
        
        animation->setEndValue(endGeometry);
        animation->start(QPropertyAnimation::DeleteWhenStopped);
    }
}

void Panel::toggleAutoHide()
{
    setAutoHide(!m_autoHide);
}

void Panel::checkMousePosition()
{
    if (!m_autoHide) return;
    
    QPoint globalPos = QCursor::pos();
    QRect panelRect = QRect(mapToGlobal(rect().topLeft()), mapToGlobal(rect().bottomRight()));
    
    // Fare panel üzerinde değilse ve panel görünüyorsa gizle
    if (!panelRect.contains(globalPos) && !m_startMenu->isVisible()) {
        QPropertyAnimation *animation = new QPropertyAnimation(this, "geometry");
        animation->setDuration(250);
        animation->setEasingCurve(QEasingCurve::OutCubic);
        
        QRect endGeometry = geometry();
        if (m_position == Qt::BottomEdge) {
            endGeometry.setY(parentWidget()->height() - m_hiddenSize);
        } else if (m_position == Qt::TopEdge) {
            endGeometry.setY(-m_panelSize + m_hiddenSize);
        } else if (m_position == Qt::LeftEdge) {
            endGeometry.setX(-m_panelSize + m_hiddenSize);
        } else if (m_position == Qt::RightEdge) {
            endGeometry.setX(parentWidget()->width() - m_hiddenSize);
        }
        
        animation->setEndValue(endGeometry);
        animation->start(QPropertyAnimation::DeleteWhenStopped);
    }
}

void Panel::enterEvent(QEvent *event)
{
    if (m_autoHide) {
        QPropertyAnimation *animation = new QPropertyAnimation(this, "geometry");
        animation->setDuration(250);
        animation->setEasingCurve(QEasingCurve::OutCubic);
        
        QRect endGeometry = geometry();
        if (m_position == Qt::BottomEdge) {
            endGeometry.setY(parentWidget()->height() - m_panelSize);
        } else if (m_position == Qt::TopEdge) {
            endGeometry.setY(0);
        } else if (m_position == Qt::LeftEdge) {
            endGeometry.setX(0);
        } else if (m_position == Qt::RightEdge) {
            endGeometry.setX(parentWidget()->width() - m_panelSize);
        }
        
        animation->setEndValue(endGeometry);
        animation->start(QPropertyAnimation::DeleteWhenStopped);
    }
    
    QWidget::enterEvent(event);
}

void Panel::leaveEvent(QEvent *event)
{
    if (m_autoHide) {
        // Fare panel dışına çıktığında otomatik gizleme zamanlayıcısını başlat
        m_hideTimer->start();
    }
    
    QWidget::leaveEvent(event);
}

void Panel::resizeEvent(QResizeEvent *event)
{
    Q_UNUSED(event);
    update();
}

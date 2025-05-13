#include "desktop.h"
#include <QPainter>
#include <QScreen>
#include <QApplication>
#include <QDebug>
#include <QVBoxLayout>
#include <QMouseEvent>
#include <QDir>

Desktop::Desktop(QWidget *parent)
    : QWidget(parent, Qt::FramelessWindowHint | Qt::WindowStaysOnBottomHint),
      m_iconView(nullptr),
      m_startMenu(nullptr)
{
    initializeDesktop();
    setBackground(":/backgrounds/background.jpg");
}

Desktop::~Desktop() {
    // Clean up resources
}

void Desktop::initializeDesktop() {
    // Set size to screen size
    QScreen *screen = QApplication::primaryScreen();
    setGeometry(screen->geometry());
    
    // Set window attributes
    setAttribute(Qt::WA_TranslucentBackground);
    setWindowTitle("XenoraOS Desktop");
    
    // Create a layout
    QVBoxLayout *layout = new QVBoxLayout(this);
    layout->setContentsMargins(0, 0, 0, 0);
    
    // Create desktop icon view
    m_iconView = new DesktopIconView(this);
    layout->addWidget(m_iconView);
    
    // Create start menu (initially hidden)
    m_startMenu = new StartMenu(nullptr); // No parent so it can be a separate window
    m_startMenu->hide();
    
    // Set up desktop icons
    setupDesktopIcons();
}

void Desktop::setBackground(const QString &path) {
    QString bgPath = path;
    QPixmap newBg;
    
    // Belirtilen yolu dene
    if (!bgPath.isEmpty()) {
        newBg = QPixmap(bgPath);
    }
    
    // Resources dizinindeki arkaplanı dene
    if (newBg.isNull()) {
        bgPath = QDir::currentPath() + "/resources/background.jpg";
        newBg = QPixmap(bgPath);
    }
    
    // Hala bulunamadıysa, dinamik olarak oluştur
    if (newBg.isNull()) {
        qWarning() << "Failed to load background image, creating dynamic background";
        newBg = QPixmap(size());
        QPainter painter(&newBg);
        QLinearGradient gradient(0, 0, width(), height());
        gradient.setColorAt(0, QColor(25, 25, 40));
        gradient.setColorAt(1, QColor(50, 50, 80));
        painter.fillRect(rect(), gradient);
    } else {
        qDebug() << "Background loaded successfully from:" << bgPath;
    }
    
    m_background = newBg;
    update();
}

void Desktop::paintEvent(QPaintEvent *event) {
    QPainter painter(this);
    
    // Draw the background image
    if (!m_background.isNull()) {
        painter.drawPixmap(rect(), m_background, m_background.rect());
    } else {
        // Fallback if background is not loaded
        QLinearGradient gradient(0, 0, width(), height());
        gradient.setColorAt(0, QColor(25, 25, 40));
        gradient.setColorAt(1, QColor(50, 50, 80));
        painter.fillRect(rect(), gradient);
    }
    
    QWidget::paintEvent(event);
}

void Desktop::resizeEvent(QResizeEvent *event) {
    if (!m_background.isNull()) {
        // Scale background to new size
        m_background = m_background.scaled(size(), Qt::KeepAspectRatioByExpanding, Qt::SmoothTransformation);
    }
    
    QWidget::resizeEvent(event);
}

void Desktop::mousePressEvent(QMouseEvent *event) {
    // Hide start menu if visible and clicked elsewhere
    if (m_startMenu && m_startMenu->isVisible()) {
        m_startMenu->hide();
    }
    
    QWidget::mousePressEvent(event);
}

void Desktop::onStartMenuToggled(bool visible) {
    if (visible) {
        if (!m_startMenu->isVisible()) {
            // Position the start menu at the bottom-left corner above the panel
            QPoint pos = QPoint(0, height() - m_startMenu->height() - 50); // 50 is approx panel height
            m_startMenu->move(pos);
            m_startMenu->show();
            m_startMenu->raise();
        }
    } else {
        m_startMenu->hide();
    }
}

void Desktop::setupDesktopIcons() {
    // Klasör bazlı yollar
    QString iconPath = QDir::currentPath() + "/resources/icons/";
    
    // Add some default desktop icons
    m_iconView->addIcon("Home", iconPath + "home.png");
    m_iconView->addIcon("Documents", iconPath + "documents.png");
    m_iconView->addIcon("Pictures", iconPath + "pictures.png");
    m_iconView->addIcon("Music", iconPath + "music.png");
    m_iconView->addIcon("Settings", iconPath + "settings.png");
}

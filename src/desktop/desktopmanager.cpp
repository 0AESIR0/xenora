#include "desktop/desktopmanager.h"
#include <QPainter>
#include <QMouseEvent>
#include <QContextMenuEvent>
#include <QAction>
#include <QProcess>
#include <QFileDialog>
#include <QStandardPaths>
#include <QDebug>

DesktopManager::DesktopManager(QWidget *parent)
    : QWidget(parent)
{
    // Masaüstü arkaplanını yükle
    setWallpaper(":/images/background.jpg");
    
    // Sağ tık menüsünü oluştur
    createContextMenu();
    
    // Tüm masaüstü tıklamalarını yakala
    setMouseTracking(true);
}

DesktopManager::~DesktopManager()
{
    delete m_contextMenu;
}

void DesktopManager::setWallpaper(const QString &path)
{
    m_wallpaper = QPixmap(path);
    update();
}

void DesktopManager::paintEvent(QPaintEvent *event)
{
    QPainter painter(this);
    
    // Arkaplanı çiz
    if (!m_wallpaper.isNull()) {
        painter.drawPixmap(rect(), m_wallpaper.scaled(size(), Qt::IgnoreAspectRatio, Qt::SmoothTransformation));
    } else {
        // Varsayılan arkaplan
        QLinearGradient gradient(0, 0, width(), height());
        gradient.setColorAt(0, QColor(50, 50, 80));
        gradient.setColorAt(1, QColor(20, 20, 30));
        painter.fillRect(rect(), gradient);
    }
}

void DesktopManager::mousePressEvent(QMouseEvent *event)
{
    m_lastPos = event->pos();
    QWidget::mousePressEvent(event);
}

void DesktopManager::mouseReleaseEvent(QMouseEvent *event)
{
    QWidget::mouseReleaseEvent(event);
}

void DesktopManager::contextMenuEvent(QContextMenuEvent *event)
{
    m_contextMenu->exec(event->globalPos());
}

void DesktopManager::createContextMenu()
{
    m_contextMenu = new QMenu(this);
    
    QAction *wallpaperAction = new QAction("Duvar Kağıdını Değiştir", this);
    connect(wallpaperAction, &QAction::triggered, this, &DesktopManager::changeWallpaper);
    
    QAction *terminalAction = new QAction("Terminal Aç", this);
    connect(terminalAction, &QAction::triggered, this, &DesktopManager::openTerminal);
    
    QAction *settingsAction = new QAction("Ayarlar", this);
    connect(settingsAction, &QAction::triggered, this, &DesktopManager::openSettings);
    
    m_contextMenu->addAction(wallpaperAction);
    m_contextMenu->addAction(terminalAction);
    m_contextMenu->addSeparator();
    m_contextMenu->addAction(settingsAction);
}

void DesktopManager::changeWallpaper()
{
    QString filePath = QFileDialog::getOpenFileName(this, 
        "Duvar Kağıdı Seç", 
        QStandardPaths::writableLocation(QStandardPaths::PicturesLocation),
        "Resimler (*.png *.jpg *.jpeg *.bmp)");
        
    if (!filePath.isEmpty()) {
        setWallpaper(filePath);
    }
}

void DesktopManager::openTerminal()
{
    QProcess::startDetached("x-terminal-emulator", QStringList());
}

void DesktopManager::openSettings()
{
    // Burada ayarlar uygulamasını başlatabilirsiniz
    qDebug() << "Ayarlar açılıyor...";
}

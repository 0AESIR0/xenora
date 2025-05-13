#include "desktopmanager.h"
#include "desktopicon.h" // DesktopIcon sınıfını içe aktarın
#include <QPainter>
#include <QDir>
#include <QStandardPaths>
#include <QResizeEvent>
#include <QDebug> // QDebug başlık dosyasını ekleyin

DesktopManager::DesktopManager(QWidget *parent)
    : QWidget(parent)
{
    // Masaüstü klasörünü izle
    QString desktopPath = QStandardPaths::writableLocation(QStandardPaths::DesktopLocation);
    m_fileWatcher.addPath(desktopPath);
    
    connect(&m_fileWatcher, &QFileSystemWatcher::directoryChanged,
            this, &DesktopManager::refreshDesktopIcons);
    
    loadIcons();
}

DesktopManager::~DesktopManager()
{
}

void DesktopManager::setWallpaper(const QString &path)
{
    m_wallpaperPath = path;
    
    if (path.isEmpty()) {
        // Eğer yol boşsa, null pixmap olarak işaretle
        m_wallpaper = QPixmap();
    } else {
        // Duvar kağıdını yükle ve kontrol et
        m_wallpaper = QPixmap(path);
        if (m_wallpaper.isNull()) {
            qWarning() << "Duvar kağıdı yüklenemedi:" << path;
        } else if (size().isValid()) {
            // Sadece geçerli bir pixmap ve boyut varsa ölçeklendir
            m_wallpaper = m_wallpaper.scaled(size(), Qt::IgnoreAspectRatio, Qt::SmoothTransformation);
        }
    }
    
    update();
}

QString DesktopManager::currentWallpaper() const
{
    return m_wallpaperPath;
}

void DesktopManager::refreshDesktopIcons()
{
    // Mevcut simgeleri temizle
    qDeleteAll(m_icons);
    m_icons.clear();
    
    // Yeniden yükle
    loadIcons();
    update();
}

void DesktopManager::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event);
    
    QPainter painter(this);
    
    // Duvar kağıdını çiz
    if (!m_wallpaper.isNull()) {
        painter.drawPixmap(rect(), m_wallpaper, m_wallpaper.rect());
    } else {
        // Duvar kağıdı yoksa koyu arka plan kullan
        painter.fillRect(rect(), QColor(40, 40, 40));
    }
}

void DesktopManager::resizeEvent(QResizeEvent *event)
{
    Q_UNUSED(event);
    
    // Duvar kağıdını yeniden boyutlandır
    if (!m_wallpaperPath.isEmpty()) {
        m_wallpaper = QPixmap(m_wallpaperPath).scaled(size(), Qt::IgnoreAspectRatio, Qt::SmoothTransformation);
    }
    
    // Simgeleri yeniden düzenle
    arrangeIcons();
}

void DesktopManager::loadIcons()
{
    // Önceki simgeleri temizle
    qDeleteAll(m_icons);
    m_icons.clear();
    
    // Masaüstü klasörünü bul
    QString desktopPath = QStandardPaths::writableLocation(QStandardPaths::DesktopLocation);
    QDir desktopDir(desktopPath);
    
    // Masaüstündeki dosya ve klasörleri ara
    QFileInfoList entries = desktopDir.entryInfoList(QDir::AllEntries | QDir::NoDotAndDotDot);
    
    // Simgeleri oluştur ve yerleştir
    int row = 0;
    int col = 0;
    int iconSize = 90;
    int maxColumns = (width() - 20) / iconSize;
    
    for (const QFileInfo &fileInfo : entries) {
        DesktopIcon *icon = new DesktopIcon(fileInfo, this);
        icon->move(10 + col * iconSize, 10 + row * iconSize);
        icon->show();
        m_icons.append(icon);
        
        // Sonraki sütun veya satıra geç
        col++;
        if (col >= maxColumns) {
            col = 0;
            row++;
        }
    }
}

void DesktopManager::arrangeIcons()
{
    // Simgeleri grid düzeninde yerleştir
    int row = 0;
    int col = 0;
    int iconSize = 90;
    int maxColumns = (width() - 20) / iconSize;
    
    for (DesktopIcon *icon : m_icons) {
        icon->move(10 + col * iconSize, 10 + row * iconSize);
        
        // Sonraki sütun veya satıra geç
        col++;
        if (col >= maxColumns) {
            col = 0;
            row++;
        }
    }
}

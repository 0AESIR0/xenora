#include "xenorasystem.h"
#include <QDebug>

XenoraSystem::XenoraSystem(QObject *parent)
    : QObject(parent),
      m_desktop(nullptr),
      m_panel(nullptr),
      m_windowManager(nullptr),
      m_settingsManager(nullptr)
{
}

XenoraSystem::~XenoraSystem() {
    shutdown();
}

void XenoraSystem::initialize() {
    qDebug() << "Initializing XenoraOS...";
    
    // Artık resources/ klasörü yerine doğrudan burada oluşturacağız
    // resource dosyalarını kontrol etmek zorunda değiliz
    createFallbackResources();
    
    // Create settings manager first as other components depend on it
    m_settingsManager = new SettingsManager(this);
    
    // Create the desktop
    m_desktop = new Desktop(nullptr);
    
    // Create the panel
    m_panel = new Panel(nullptr);
    
    // Create window manager
    m_windowManager = new WindowManager(this);
    
    // Setup connections between components
    setupSystemConnections();
    
    // Show the desktop
    m_desktop->show();
    
    // Show the panel
    m_panel->show();
    
    qDebug() << "XenoraOS initialized successfully";
}

void XenoraSystem::shutdown() {
    qDebug() << "Shutting down XenoraOS...";
    
    if (m_panel) {
        m_panel->hide();
        m_panel->deleteLater();
        m_panel = nullptr;
    }
    
    if (m_desktop) {
        m_desktop->hide();
        m_desktop->deleteLater();
        m_desktop = nullptr;
    }
    
    if (m_windowManager) {
        delete m_windowManager;
        m_windowManager = nullptr;
    }
    
    if (m_settingsManager) {
        delete m_settingsManager;
        m_settingsManager = nullptr;
    }
}

void XenoraSystem::setupSystemConnections() {
    // Connect screen geometry changes
    QScreen *primaryScreen = QApplication::primaryScreen();
    connect(primaryScreen, &QScreen::geometryChanged, 
            this, &XenoraSystem::onScreenGeometryChanged);
    
    // Connect panel to desktop
    connect(m_panel, &Panel::startMenuToggled, 
            m_desktop, &Desktop::onStartMenuToggled);
    
    // Connect settings manager to desktop and panel
    connect(m_settingsManager, &SettingsManager::backgroundChanged,
            m_desktop, &Desktop::setBackground);
}

void XenoraSystem::onScreenGeometryChanged(const QRect &geometry) {
    if (m_desktop) {
        m_desktop->resize(geometry.width(), geometry.height());
    }
    
    if (m_panel) {
        m_panel->updateGeometry(geometry);
    }
}

void XenoraSystem::createFallbackResources() {
    // Temporary directory for fallback resources
    QString tempDir = QDir::tempPath() + "/xenora-resources";
    QDir().mkpath(tempDir);
    QDir().mkpath(tempDir + "/icons");
    QDir().mkpath(tempDir + "/fonts");
    QDir().mkpath(tempDir + "/style");
    
    // Create a fallback background image
    QPixmap bgPixmap(1920, 1080);
    QPainter bgPainter(&bgPixmap);
    QLinearGradient bgGradient(0, 0, 1920, 1080);
    bgGradient.setColorAt(0, QColor(25, 25, 50));
    bgGradient.setColorAt(0.5, QColor(50, 50, 100));
    bgGradient.setColorAt(1, QColor(25, 25, 70));
    bgPainter.fillRect(bgPixmap.rect(), bgGradient);
    
    // Add some random stars/dots for a space-like theme
    bgPainter.setPen(Qt::white);
    QRandomGenerator *rng = QRandomGenerator::global();
    for (int i = 0; i < 500; i++) {
        int x = rng->bounded(1920);
        int y = rng->bounded(1080);
        int size = rng->bounded(1, 3);
        bgPainter.drawEllipse(x, y, size, size);
    }
    
    // Add some futuristic elements
    bgPainter.setPen(QPen(QColor(100, 200, 255, 100), 2));
    for (int i = 0; i < 20; i++) {
        int x = rng->bounded(1920);
        int y = rng->bounded(1080);
        int radius = rng->bounded(50, 400);
        bgPainter.drawEllipse(QPoint(x, y), radius, radius);
    }
    
    bgPainter.setPen(QPen(QColor(150, 150, 255, 50), 1));
    for (int i = 0; i < 30; i++) {
        int x1 = rng->bounded(1920);
        int y1 = rng->bounded(1080);
        int x2 = rng->bounded(1920);
        int y2 = rng->bounded(1080);
        bgPainter.drawLine(x1, y1, x2, y2);
    }
    
    bgPixmap.save(tempDir + "/background.jpg", "JPG");
    
    // Create fallback icons (simple colored shapes)
    QStringList iconNames = {"start", "settings", "power", "home", "documents", 
                            "pictures", "music", "videos", "apps"};
    QList<QColor> iconColors = {
        QColor(50, 150, 255),   // start
        QColor(100, 100, 255),  // settings
        QColor(255, 100, 100),  // power
        QColor(100, 200, 100),  // home
        QColor(200, 200, 100),  // documents
        QColor(200, 100, 200),  // pictures
        QColor(100, 200, 200),  // music
        QColor(255, 150, 50),   // videos
        QColor(150, 150, 255)   // apps
    };
    
    for (int i = 0; i < iconNames.size(); i++) {
        QPixmap iconPixmap(128, 128);
        iconPixmap.fill(Qt::transparent);
        QPainter iconPainter(&iconPixmap);
        iconPainter.setRenderHint(QPainter::Antialiasing);
        
        // Custom shape based on icon type
        QColor iconColor = iconColors[i];
        iconPainter.setPen(Qt::NoPen);
        iconPainter.setBrush(iconColor);
        
        if (iconNames[i] == "start") {
            // Start icon (X shape)
            QPolygon poly;
            poly << QPoint(64, 30) << QPoint(98, 64) << QPoint(64, 98) << QPoint(30, 64);
            iconPainter.drawPolygon(poly);
        } else if (iconNames[i] == "settings") {
            // Settings icon (gear)
            iconPainter.drawEllipse(34, 34, 60, 60);
            iconPainter.setBrush(Qt::transparent);
            iconPainter.setPen(QPen(Qt::white, 3));
            iconPainter.drawEllipse(44, 44, 40, 40);
        } else if (iconNames[i] == "power") {
            // Power icon (circle with line)
            iconPainter.drawEllipse(34, 34, 60, 60);
            iconPainter.setPen(QPen(Qt::white, 4));
            iconPainter.drawLine(64, 40, 64, 70);
        } else {
            // Generic icon (rounded rectangle)
            iconPainter.drawRoundedRect(24, 24, 80, 80, 15, 15);
            
            // Add a simple icon identifier
            iconPainter.setPen(QPen(Qt::white, 2));
            iconPainter.setFont(QFont("Arial", 12, QFont::Bold));
            iconPainter.drawText(iconPixmap.rect(), Qt::AlignCenter, 
                                QString(iconNames[i][0]).toUpper());
        }
        
        iconPixmap.save(tempDir + "/icons/" + iconNames[i] + ".png", "PNG");
    }
    
    // Set environment variable to point to our fallback resources
    qputenv("QT_QPA_RESOURCE_PREFIX", tempDir.toUtf8());
}

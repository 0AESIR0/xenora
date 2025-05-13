#include <QApplication>
#include <QSurfaceFormat>
#include <QFontDatabase>
#include <QFile>
#include <QDir>
#include <QDebug>
#include "desktopenv/DesktopEnvironment.h"
#include "desktopenv/StyleManager.h"

int main(int argc, char *argv[])
{
    // Enable high DPI scaling
    QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
    QCoreApplication::setAttribute(Qt::AA_UseHighDpiPixmaps);
    
    QApplication app(argc, argv);
    app.setApplicationName("XenoraOS");
    app.setApplicationVersion("0.1");
    app.setOrganizationName("XenoraOS");
    
    // Setup OpenGL for smooth rendering
    QSurfaceFormat format;
    format.setSamples(8);
    format.setSwapInterval(1);
    QSurfaceFormat::setDefaultFormat(format);
    
    // Check resources
    qDebug() << "Resource paths:";
    qDebug() << QDir(":/").entryList();
    qDebug() << "Checking for default wallpaper:" << QFile::exists(":/backgrounds/default.jpg");
    qDebug() << "Checking for icons:" << QFile::exists(":/icons/system/close.svg");
    
    // Load custom fonts
    int fontId = QFontDatabase::addApplicationFont(":/fonts/XenoraOS-Regular.ttf");
    if (fontId != -1) {
        qDebug() << "Font loaded successfully:" << QFontDatabase::applicationFontFamilies(fontId);
        app.setFont(QFont(QFontDatabase::applicationFontFamilies(fontId).at(0), 10));
    } else {
        qWarning() << "Failed to load custom font, using system font";
        app.setFont(QFont("Sans Serif", 10));
    }
    
    // Setup global style
    StyleManager::applyStyle(&app);
    
    qDebug() << "Starting XenoraOS GUI...";
    
    // Initialize and start desktop environment
    DesktopEnvironment desktop;
    desktop.initialize();
    desktop.show();
    
    return app.exec();
}

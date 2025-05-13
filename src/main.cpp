#include <QApplication>
#include <QSurfaceFormat>
#include <QFontDatabase>
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
    
    // Load custom fonts
    QFontDatabase::addApplicationFont(":/fonts/XenoraOS-Regular.ttf");
    app.setFont(QFont("XenoraOS", 10));
    
    // Setup global style
    StyleManager::applyStyle(&app);
    
    // Initialize and start desktop environment
    DesktopEnvironment desktop;
    desktop.initialize();
    desktop.show();
    
    return app.exec();
}

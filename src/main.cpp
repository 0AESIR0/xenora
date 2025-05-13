#include "mainwindow.h"
#include <QApplication>
#include <QCommandLineParser>
#include <QSettings>
#include <QDir>

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    app.setApplicationName("XenoraOS");
    app.setApplicationVersion("0.1");
    app.setOrganizationName("XenoraOS");
    
    // Komut satırı seçenekleri
    QCommandLineParser parser;
    parser.setApplicationDescription("XenoraOS - Modern Linux Desktop Environment");
    parser.addHelpOption();
    parser.addVersionOption();
    parser.process(app);
    
    // Ayarları yükle
    QSettings settings("XenoraOS", "desktop");
    
    // Ana pencereyi oluştur ve başlat
    MainWindow mainWindow;
    mainWindow.showFullScreen();
    
    return app.exec();
}

#include <QApplication>
#include <QFile>
#include <QDir>
#include <QStandardPaths>
#include "xenorasystem.h"

// Kaynakların bulunduğundan emin olacak yardımcı fonksiyon
void ensureResourcesExist() {
    // Gerekli dizinleri oluştur
    QString resDir = QDir::currentPath() + "/resources";
    QDir().mkpath(resDir);
    QDir().mkpath(resDir + "/style");
    QDir().mkpath(resDir + "/icons");
    
    // Dummy icon oluştur
    if (!QFile::exists(resDir + "/icons/dummy.png")) {
        QPixmap pixmap(64, 64);
        pixmap.fill(Qt::blue);
        pixmap.save(resDir + "/icons/dummy.png");
    }
    
    // Arka plan oluştur
    if (!QFile::exists(resDir + "/background.jpg")) {
        QPixmap bg(1920, 1080);
        bg.fill(QColor(25, 25, 40));
        bg.save(resDir + "/background.jpg");
    }
    
    // Style dosyasını oluştur
    if (!QFile::exists(resDir + "/style/xenora-style.qss")) {
        QFile styleFile(resDir + "/style/xenora-style.qss");
        if (styleFile.open(QIODevice::WriteOnly | QIODevice::Text)) {
            QTextStream out(&styleFile);
            out << "/* XenoraOS Style */\n";
            out << "QWidget { background-color: rgba(25, 25, 40, 0.8); color: white; }\n";
            styleFile.close();
        }
    }
}

int main(int argc, char *argv[]) {
    // Set application attributes before creating QApplication
    QApplication::setAttribute(Qt::AA_UseHighDpiPixmaps);
    QApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
    
    QApplication app(argc, argv);
    app.setApplicationName("XenoraOS");
    app.setApplicationVersion("1.0.0");
    
    // Kaynakların var olduğundan emin ol
    ensureResourcesExist();
    
    // Load stylesheet
    QFile styleFile("resources/style/xenora-style.qss");
    if (styleFile.open(QFile::ReadOnly)) {
        QString style = QLatin1String(styleFile.readAll());
        app.setStyleSheet(style);
        styleFile.close();
    }

    // Create the main system
    XenoraSystem xenoraSystem;
    xenoraSystem.initialize();

    return app.exec();
}

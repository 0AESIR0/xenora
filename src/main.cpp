#include <QApplication>
#include <QFile>
#include <QFontDatabase>
#include <QScreen>
#include "xenorasystem.h"

int main(int argc, char *argv[]) {
    // Set application attributes before creating QApplication
    QApplication::setAttribute(Qt::AA_UseHighDpiPixmaps);
    QApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
    
    QApplication app(argc, argv);
    app.setApplicationName("XenoraOS");
    app.setApplicationVersion("1.0.0");
    
    // Load custom font
    int fontId = QFontDatabase::addApplicationFont(":/fonts/XenoraOS-Regular.ttf");
    if (fontId != -1) {
        QString fontFamily = QFontDatabase::applicationFontFamilies(fontId).at(0);
        QFont font(fontFamily);
        app.setFont(font);
    }

    // Load stylesheet
    QFile styleFile(":/style/xenora-style.qss");
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

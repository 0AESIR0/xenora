#include <QApplication>
#include <QFile>
#include <QDir>
#include <QDebug>
#include <QPainter>
#include <QRandomGenerator>
#include <QTextStream>
#include <QLinearGradient>
#include <QColor>
#include "xenorasystem.h"

// Kaynakların bulunduğundan emin olacak yardımcı fonksiyon
void ensureResourcesExist() {
    // Gerekli dizinleri oluştur
    QString resDir = QDir::currentPath() + "/resources";
    QDir().mkpath(resDir);
    QDir().mkpath(resDir + "/style");
    QDir().mkpath(resDir + "/icons");
    
    // Style dosyasını oluştur
    QString styleFile = resDir + "/style/xenora-style.qss";
    if (!QFile::exists(styleFile)) {
        QFile file(styleFile);
        if (file.open(QIODevice::WriteOnly | QIODevice::Text)) {
            QTextStream out(&file);
            out << "/* XenoraOS Style */\n";
            out << "QWidget { background-color: #202035; color: white; }\n";
            file.close();
            qDebug() << "Created style file:" << styleFile;
        }
    }
    
    // Arkaplan oluştur
    QString bgFile = resDir + "/background.jpg";
    if (!QFile::exists(bgFile)) {
        QPixmap bg(1920, 1080);
        QPainter painter(&bg);
        QLinearGradient gradient(0, 0, 1920, 1080);
        gradient.setColorAt(0, QColor(25, 25, 40));
        gradient.setColorAt(1, QColor(40, 40, 80));
        painter.fillRect(bg.rect(), gradient);
        
        // Uzaya benzer efekt için noktalar ekle
        painter.setPen(Qt::white);
        for (int i = 0; i < 500; i++) {
            int x = QRandomGenerator::global()->bounded(1920);
            int y = QRandomGenerator::global()->bounded(1080);
            painter.drawPoint(x, y);
        }
        
        bg.save(bgFile, "JPG");
        qDebug() << "Created background image:" << bgFile;
    }
    
    // İkonları oluştur
    QStringList iconNames = {"start", "settings", "power", "home", "documents", 
                            "pictures", "music", "videos", "apps"};
    
    for (const QString &iconName : iconNames) {
        QString iconFile = resDir + "/icons/" + iconName + ".png";
        if (!QFile::exists(iconFile)) {
            QPixmap icon(64, 64);
            icon.fill(Qt::transparent);
            QPainter painter(&icon);
            
            // Basit yuvarlatılmış ikon
            painter.setRenderHint(QPainter::Antialiasing);
            painter.setPen(Qt::NoPen);
            
            // Her ikon için farklı renk
            if (iconName == "start") painter.setBrush(QColor(60, 100, 200));
            else if (iconName == "settings") painter.setBrush(QColor(60, 160, 200));
            else if (iconName == "power") painter.setBrush(QColor(200, 60, 60));
            else if (iconName == "home") painter.setBrush(QColor(60, 180, 120));
            else painter.setBrush(QColor(120, 120, 180));
            
            painter.drawRoundedRect(4, 4, 56, 56, 10, 10);
            
            // İkon tanımlayıcı ekle
            painter.setPen(Qt::white);
            painter.setFont(QFont("Arial", 24, QFont::Bold));
            painter.drawText(icon.rect(), Qt::AlignCenter, 
                           QString(iconName.at(0)).toUpper());
            
            icon.save(iconFile, "PNG");
            qDebug() << "Created icon:" << iconFile;
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
    QFile styleFile(QDir::currentPath() + "/resources/style/xenora-style.qss");
    if (styleFile.open(QFile::ReadOnly)) {
        QString style = QLatin1String(styleFile.readAll());
        app.setStyleSheet(style);
        styleFile.close();
        qDebug() << "Loaded stylesheet successfully";
    } else {
        qWarning() << "Failed to load stylesheet:" << styleFile.fileName();
    }

    // Create the main system
    XenoraSystem xenoraSystem;
    xenoraSystem.initialize();

    return app.exec();
}

#include "mainwindow.h"
#include <QApplication>
#include <QDesktopWidget>
#include <QFile>
#include <QScreen>
#include <QDebug>
#include <QVBoxLayout>
#include <QMenu>
#include <QAction>
#include <QFileDialog>
#include <QMessageBox>
#include <QProcess>
#include <QGraphicsBlurEffect>
#include <QPropertyAnimation>
#include <QGraphicsOpacityEffect>
#include <QParallelAnimationGroup>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , m_settings("XenoraOS", "desktop")
{
    // Pencere ayarları
    setWindowFlags(Qt::FramelessWindowHint | Qt::WindowStaysOnBottomHint);
    setAttribute(Qt::WA_X11NetWmWindowTypeDesktop);
    setAttribute(Qt::WA_TranslucentBackground);
    
    // Ekran boyutunu al
    QRect screenGeometry = QGuiApplication::primaryScreen()->geometry();
    setGeometry(screenGeometry);
    
    // Tema verilerini yükle
    loadThemeData();
    
    // Bileşenleri oluştur
    createDesktop();
    createPanel();
    
    // Efektleri uygula
    applyVisualEffects();
    
    // Ayarları yükle
    loadSettings();
}

MainWindow::~MainWindow()
{
    saveSettings();
}

void MainWindow::createDesktop()
{
    m_desktopManager = new DesktopManager(this);
    setCentralWidget(m_desktopManager);
    
    // Sağ tık menüsü
    m_desktopManager->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(m_desktopManager, &DesktopManager::customContextMenuRequested,
            this, &MainWindow::handleDesktopMenu);
}

void MainWindow::createPanel()
{
    m_panel = new Panel(this);
    m_panel->setGeometry(0, height() - 40, width(), 40);
}

void MainWindow::handleDesktopMenu(const QPoint &pos)
{
    QMenu menu;
    QAction *wallpaperAction = menu.addAction("Duvar Kağıdını Değiştir");
    QAction *settingsAction = menu.addAction("Ayarlar");
    menu.addSeparator();
    QAction *logoutAction = menu.addAction("Oturumu Kapat");
    
    QAction *selectedAction = menu.exec(mapToGlobal(pos));
    
    if (selectedAction == wallpaperAction) {
        changeWallpaper();
    } else if (selectedAction == settingsAction) {
        openSettings();
    } else if (selectedAction == logoutAction) {
        logout();
    }
}

void MainWindow::changeWallpaper()
{
    QString filePath = QFileDialog::getOpenFileName(this, "Duvar Kağıdı Seç",
                                                  QDir::homePath(),
                                                  "Görüntü Dosyaları (*.png *.jpg *.jpeg)");
    if (!filePath.isEmpty()) {
        m_desktopManager->setWallpaper(filePath);
        m_settings.setValue("wallpaper", filePath);
    }
}

void MainWindow::openSettings()
{
    // Ayarlar penceresi (henüz uygulanmadı)
    QMessageBox::information(this, "Ayarlar", "Ayarlar penceresi yakında eklenecek.");
}

void MainWindow::logout()
{
    QProcess::startDetached("pkill", QStringList() << "-KILL" << "xenora-ui");
}

void MainWindow::loadSettings()
{
    QString wallpaper = m_settings.value("wallpaper", ":/resources/default-wallpaper.jpg").toString();
    
    // Duvar kağıdı kontrolü ekle
    QFileInfo fileInfo(wallpaper);
    if (fileInfo.exists() && fileInfo.isFile()) {
        m_desktopManager->setWallpaper(wallpaper);
    } else {
        // Varsayılan duvar kağıdı yoksa düz renk kullan
        qWarning() << "Duvar kağıdı bulunamadı:" << wallpaper;
        m_desktopManager->setWallpaper("");
    }
}

void MainWindow::saveSettings()
{
    m_settings.sync();
}

void MainWindow::loadThemeData()
{
    // Varsayılan Cosmic Horizon teması
    m_themeData.primaryColor = QColor(20, 30, 60);
    m_themeData.accentColor = QColor(120, 80, 255);
    m_themeData.textColor = QColor(220, 230, 255);
    m_themeData.glowColor = QColor(80, 100, 255, 150);
    m_themeData.transparencyLevel = 0.85;
    m_themeData.blurRadius = 15;
    m_themeData.animationSpeed = 300; // ms
    
    // Tema adını ayarlardan al
    QString themeName = m_settings.value("theme/name", "CosmicHorizon").toString();
    
    // Eğer ayarlarda farklı bir tema varsa, onu yükle
    if (themeName != "CosmicHorizon") {
        loadThemeByName(themeName);
    }
}

void MainWindow::loadThemeByName(const QString &themeName)
{
    // Farklı tema ayarlarını yükle
    if (themeName == "NebulaGlow") {
        m_themeData.primaryColor = QColor(40, 10, 60);
        m_themeData.accentColor = QColor(200, 100, 220);
        m_themeData.textColor = QColor(240, 220, 255);
        m_themeData.glowColor = QColor(180, 80, 220, 150);
    } else if (themeName == "QuantumDark") {
        m_themeData.primaryColor = QColor(10, 12, 15);
        m_themeData.accentColor = QColor(0, 150, 200);
        m_themeData.textColor = QColor(200, 210, 215);
        m_themeData.glowColor = QColor(0, 130, 180, 120);
    } else if (themeName == "BioLuminescence") {
        m_themeData.primaryColor = QColor(10, 40, 30);
        m_themeData.accentColor = QColor(40, 230, 140);
        m_themeData.textColor = QColor(220, 255, 230);
        m_themeData.glowColor = QColor(30, 210, 120, 150);
    } else if (themeName == "Holographic") {
        m_themeData.primaryColor = QColor(15, 25, 35);
        m_themeData.accentColor = QColor(80, 200, 255);
        m_themeData.textColor = QColor(210, 230, 255);
        m_themeData.glowColor = QColor(60, 180, 240, 180);
        m_themeData.transparencyLevel = 0.7;
        m_themeData.blurRadius = 25;
    }
}

void MainWindow::applyVisualEffects()
{
    // Masaüstüne bulanıklık efekti ekle
    QGraphicsBlurEffect *blurEffect = new QGraphicsBlurEffect(this);
    blurEffect->setBlurRadius(m_themeData.blurRadius);
    blurEffect->setBlurHints(QGraphicsBlurEffect::QualityHint);
    m_desktopManager->setGraphicsEffect(blurEffect);
    
    // Panel için gölge ve parlaklık efektleri
    QGraphicsDropShadowEffect *shadowEffect = new QGraphicsDropShadowEffect(this);
    shadowEffect->setBlurRadius(15);
    shadowEffect->setColor(m_themeData.glowColor);
    shadowEffect->setOffset(0, 0);
    m_panel->setGraphicsEffect(shadowEffect);
    
    // Başlangıç animasyonu ekle
    QGraphicsOpacityEffect *opacityEffect = new QGraphicsOpacityEffect(this);
    opacityEffect->setOpacity(0);
    centralWidget()->setGraphicsEffect(opacityEffect);
    
    QPropertyAnimation *animation = new QPropertyAnimation(opacityEffect, "opacity", this);
    animation->setDuration(m_themeData.animationSpeed * 2);
    animation->setStartValue(0.0);
    animation->setEndValue(1.0);
    animation->setEasingCurve(QEasingCurve::OutCubic);
    animation->start(QPropertyAnimation::DeleteWhenStopped);
}

void MainWindow::changeTheme(const QString &themeName)
{
    // Tema değiştirme işlevi
    if (themeName != m_settings.value("theme/name", "CosmicHorizon").toString()) {
        m_settings.setValue("theme/name", themeName);
        loadThemeByName(themeName);
        applyVisualEffects();
        
        // Temayı değiştirdikten sonra ekranı güncelle
        update();
        m_panel->update();
    }
}

void MainWindow::showThemeSelector()
{
    // Tema seçici menüyü göster
    QMenu menu(this);
    menu.setTitle("Tema Seç");
    
    QAction *cosmicAction = menu.addAction("Cosmic Horizon");
    QAction *nebulaAction = menu.addAction("Nebula Glow");
    QAction *quantumAction = menu.addAction("Quantum Dark");
    QAction *bioAction = menu.addAction("BioLuminescence");
    QAction *holoAction = menu.addAction("Holographic");
    
    // Mevcut temayı işaretle
    QString currentTheme = m_settings.value("theme/name", "CosmicHorizon").toString();
    
    if (currentTheme == "CosmicHorizon") cosmicAction->setChecked(true);
    else if (currentTheme == "NebulaGlow") nebulaAction->setChecked(true);
    else if (currentTheme == "QuantumDark") quantumAction->setChecked(true);
    else if (currentTheme == "BioLuminescence") bioAction->setChecked(true);
    else if (currentTheme == "Holographic") holoAction->setChecked(true);
    
    // Sinyal bağlantıları
    connect(cosmicAction, &QAction::triggered, [this](){ changeTheme("CosmicHorizon"); });
    connect(nebulaAction, &QAction::triggered, [this](){ changeTheme("NebulaGlow"); });
    connect(quantumAction, &QAction::triggered, [this](){ changeTheme("QuantumDark"); });
    connect(bioAction, &QAction::triggered, [this](){ changeTheme("BioLuminescence"); });
    connect(holoAction, &QAction::triggered, [this](){ changeTheme("Holographic"); });
    
    // Menüyü göster
    menu.exec(QCursor::pos());
}

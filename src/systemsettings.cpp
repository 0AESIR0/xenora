#include "systemsettings.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QHeaderView>
#include <QTabWidget>
#include <QFileDialog>
#include <QMessageBox>
#include <QScreen>
#include <QApplication>
#include <QPainter>
#include <QProcess>
#include <QDebug>

SystemSettings::SystemSettings(QWidget *parent)
    : QMainWindow(parent)
    , m_settings("XenoraOS", "settings")
    , m_needsRestart(false)
{
    setWindowTitle("XenoraOS Sistem Ayarları");
    setWindowIcon(QIcon::fromTheme("preferences-system"));
    
    // Ana pencere boyutu
    int width = 900;
    int height = 600;
    resize(width, height);
    
    // Ekran ortasında konumlandır
    QScreen *screen = QGuiApplication::primaryScreen();
    QRect screenGeometry = screen->geometry();
    int x = (screenGeometry.width() - width) / 2;
    int y = (screenGeometry.height() - height) / 2;
    move(x, y);
    
    setupUI();
    loadSettings();
}

SystemSettings::~SystemSettings()
{
    saveSettings();
}

void SystemSettings::setupUI()
{
    // Ana widget
    QWidget *centralWidget = new QWidget(this);
    setCentralWidget(centralWidget);
    
    // Ana düzen
    QHBoxLayout *mainLayout = new QHBoxLayout(centralWidget);
    mainLayout->setContentsMargins(10, 10, 10, 10);
    mainLayout->setSpacing(10);
    
    // Kategori listesi
    setupCategoryList();
    mainLayout->addWidget(m_categoryList, 1);
    
    // Sayfa yığını
    m_pageStack = new QStackedWidget(this);
    mainLayout->addWidget(m_pageStack, 3);
    
    // Ayar sayfaları
    setupAppearancePage();
    setupDesktopPage();
    setupPanelPage();
    setupSystemPage();
    
    // Uygula butonu
    QHBoxLayout *buttonLayout = new QHBoxLayout();
    buttonLayout->setContentsMargins(0, 10, 0, 0);
    buttonLayout->addStretch();
    
    QPushButton *applyButton = new QPushButton("Uygula", this);
    connect(applyButton, &QPushButton::clicked, this, &SystemSettings::applySettings);
    buttonLayout->addWidget(applyButton);
    
    // Ana düzen içine buton düzenini ekle
    QVBoxLayout *rightLayout = new QVBoxLayout();
    rightLayout->addWidget(m_pageStack);
    rightLayout->addLayout(buttonLayout);
    
    mainLayout->addLayout(rightLayout, 3);
}

void SystemSettings::setupCategoryList()
{
    m_categoryList = new QTreeWidget(this);
    m_categoryList->setHeaderHidden(true);
    m_categoryList->setIconSize(QSize(24, 24));
    m_categoryList->setRootIsDecorated(false);
    m_categoryList->setUniformRowHeights(true);
    m_categoryList->setFrameShape(QFrame::NoFrame);
    
    // Kategori öğeleri
    QTreeWidgetItem *appearanceItem = new QTreeWidgetItem(m_categoryList);
    appearanceItem->setText(0, "Görünüm");
    appearanceItem->setIcon(0, QIcon::fromTheme("preferences-desktop-theme"));
    
    QTreeWidgetItem *desktopItem = new QTreeWidgetItem(m_categoryList);
    desktopItem->setText(0, "Masaüstü");
    desktopItem->setIcon(0, QIcon::fromTheme("preferences-desktop-wallpaper"));
    
    QTreeWidgetItem *panelItem = new QTreeWidgetItem(m_categoryList);
    panelItem->setText(0, "Panel");
    panelItem->setIcon(0, QIcon::fromTheme("user-desktop"));
    
    QTreeWidgetItem *systemItem = new QTreeWidgetItem(m_categoryList);
    systemItem->setText(0, "Sistem");
    systemItem->setIcon(0, QIcon::fromTheme("preferences-system"));
    
    // İlk öğeyi seç
    m_categoryList->setCurrentItem(appearanceItem);
    
    // Sinyal bağlantısı
    connect(m_categoryList, &QTreeWidget::currentItemChanged, this, &SystemSettings::categoryChanged);
}

void SystemSettings::setupAppearancePage()
{
    m_appearancePage = new QWidget(this);
    m_pageStack->addWidget(m_appearancePage);
    
    QVBoxLayout *layout = new QVBoxLayout(m_appearancePage);
    
    // Tema seçimi
    QGroupBox *themeGroup = new QGroupBox("Tema", m_appearancePage);
    QVBoxLayout *themeLayout = new QVBoxLayout(themeGroup);
    
    m_themeCombo = new QComboBox(themeGroup);
    m_themeCombo->addItem("Cosmic Horizon", "CosmicHorizon");
    m_themeCombo->addItem("Nebula Glow", "NebulaGlow");
    m_themeCombo->addItem("Quantum Dark", "QuantumDark");
    m_themeCombo->addItem("BioLuminescence", "BioLuminescence");
    m_themeCombo->addItem("Holographic", "Holographic");
    
    themeLayout->addWidget(new QLabel("Tema Seçin:"));
    themeLayout->addWidget(m_themeCombo);
    
    connect(m_themeCombo, QOverload<int>::of(&QComboBox::currentIndexChanged), this, &SystemSettings::selectTheme);
    
    // Duvar kağıdı seçimi
    QGroupBox *wallpaperGroup = new QGroupBox("Duvar Kağıdı", m_appearancePage);
    QVBoxLayout *wallpaperLayout = new QVBoxLayout(wallpaperGroup);
    
    m_wallpaperPreview = new QLabel(wallpaperGroup);
    m_wallpaperPreview->setMinimumSize(320, 180);
    m_wallpaperPreview->setMaximumSize(320, 180);
    m_wallpaperPreview->setScaledContents(true);
    m_wallpaperPreview->setFrameShape(QFrame::Box);
    
    m_wallpaperButton = new QPushButton("Duvar Kağıdı Seç", wallpaperGroup);
    connect(m_wallpaperButton, &QPushButton::clicked, this, &SystemSettings::changeWallpaper);
    
    wallpaperLayout->addWidget(m_wallpaperPreview);
    wallpaperLayout->addWidget(m_wallpaperButton);
    
    // Efektler
    QGroupBox *effectsGroup = new QGroupBox("Görsel Efektler", m_appearancePage);
    QVBoxLayout *effectsLayout = new QVBoxLayout(effectsGroup);
    
    m_enableEffectsCheck = new QCheckBox("Görsek Efektleri Etkinleştir", effectsGroup);
    m_enableBlurCheck = new QCheckBox("Bulanıklık Efektini Etkinleştir", effectsGroup);
    
    QLabel *transparencyLabel = new QLabel("Şeffaflık:");
    m_transparencySlider = new QSlider(Qt::Horizontal, effectsGroup);
    m_transparencySlider->setRange(0, 100);
    m_transparencySlider->setValue(85);
    
    QLabel *animationLabel = new QLabel("Animasyon Hızı:");
    m_animationSpeedSlider = new QSlider(Qt::Horizontal, effectsGroup);
    m_animationSpeedSlider->setRange(50, 500);
    m_animationSpeedSlider->setValue(300);
    
    effectsLayout->addWidget(m_enableEffectsCheck);
    effectsLayout->addWidget(m_enableBlurCheck);
    effectsLayout->addWidget(transparencyLabel);
    effectsLayout->addWidget(m_transparencySlider);
    effectsLayout->addWidget(animationLabel);
    effectsLayout->addWidget(m_animationSpeedSlider);
    
    // Sayfaya ekle
    layout->addWidget(themeGroup);
    layout->addWidget(wallpaperGroup);
    layout->addWidget(effectsGroup);
    layout->addStretch();
}

void SystemSettings::setupDesktopPage()
{
    m_desktopPage = new QWidget(this);
    m_pageStack->addWidget(m_desktopPage);
    
    QVBoxLayout *layout = new QVBoxLayout(m_desktopPage);
    
    // Masaüstü simgeleri
    QGroupBox *iconsGroup = new QGroupBox("Masaüstü Simgeleri", m_desktopPage);
    QVBoxLayout *iconsLayout = new QVBoxLayout(iconsGroup);
    
    QCheckBox *showIconsCheck = new QCheckBox("Masaüstü Simgelerini Göster", iconsGroup);
    showIconsCheck->setChecked(true);
    
    QLabel *iconSizeLabel = new QLabel("Simge Boyutu:");
    QSlider *iconSizeSlider = new QSlider(Qt::Horizontal, iconsGroup);
    iconSizeSlider->setRange(32, 96);
    iconSizeSlider->setValue(48);
    
    iconsLayout->addWidget(showIconsCheck);
    iconsLayout->addWidget(iconSizeLabel);
    iconsLayout->addWidget(iconSizeSlider);
    
    // Masaüstü efektleri
    QGroupBox *effectsGroup = new QGroupBox("Masaüstü Efektleri", m_desktopPage);
    QVBoxLayout *effectsLayout = new QVBoxLayout(effectsGroup);
    
    QCheckBox *dynamicWallpaperCheck = new QCheckBox("Dinamik Duvar Kağıdı", effectsGroup);
    QCheckBox *interactiveWallpaperCheck = new QCheckBox("Etkileşimli Duvar Kağıdı Efektleri", effectsGroup);
    QCheckBox *showShadowsCheck = new QCheckBox("Pencere Gölgelerini Göster", effectsGroup);
    
    effectsLayout->addWidget(dynamicWallpaperCheck);
    effectsLayout->addWidget(interactiveWallpaperCheck);
    effectsLayout->addWidget(showShadowsCheck);
    
    // Sayfaya ekle
    layout->addWidget(iconsGroup);
    layout->addWidget(effectsGroup);
    layout->addStretch();
}

void SystemSettings::setupPanelPage()
{
    m_panelPage = new QWidget(this);
    m_pageStack->addWidget(m_panelPage);
    
    QVBoxLayout *layout = new QVBoxLayout(m_panelPage);
    
    // Panel konumu
    QGroupBox *positionGroup = new QGroupBox("Panel Konumu", m_panelPage);
    QVBoxLayout *positionLayout = new QVBoxLayout(positionGroup);
    
    m_panelPositionCombo = new QComboBox(positionGroup);
    m_panelPositionCombo->addItem("Alt", "bottom");
    m_panelPositionCombo->addItem("Üst", "top");
    m_panelPositionCombo->addItem("Sol", "left");
    m_panelPositionCombo->addItem("Sağ", "right");
    
    connect(m_panelPositionCombo, QOverload<int>::of(&QComboBox::currentIndexChanged), this, &SystemSettings::changePanelPosition);
    
    QLabel *panelSizeLabel = new QLabel("Panel Boyutu:");
    m_panelSizeSpin = new QSpinBox(positionGroup);
    m_panelSizeSpin->setRange(24, 72);
    m_panelSizeSpin->setValue(48);
    m_panelSizeSpin->setSuffix(" px");
    
    positionLayout->addWidget(new QLabel("Konum:"));
    positionLayout->addWidget(m_panelPositionCombo);
    positionLayout->addWidget(panelSizeLabel);
    positionLayout->addWidget(m_panelSizeSpin);
    
    // Panel davranışı
    QGroupBox *behaviorGroup = new QGroupBox("Panel Davranışı", m_panelPage);
    QVBoxLayout *behaviorLayout = new QVBoxLayout(behaviorGroup);
    
    m_panelAutoHideCheck = new QCheckBox("Otomatik Gizle", behaviorGroup);
    connect(m_panelAutoHideCheck, &QCheckBox::toggled, this, &SystemSettings::togglePanelAutoHide);
    
    QCheckBox *showWindowsListCheck = new QCheckBox("Pencere Listesini Göster", behaviorGroup);
    showWindowsListCheck->setChecked(true);
    
    QCheckBox *groupWindowsCheck = new QCheckBox("Benzer Pencereleri Grupla", behaviorGroup);
    groupWindowsCheck->setChecked(true);
    
    behaviorLayout->addWidget(m_panelAutoHideCheck);
    behaviorLayout->addWidget(showWindowsListCheck);
    behaviorLayout->addWidget(groupWindowsCheck);
    
    // Sayfaya ekle
    layout->addWidget(positionGroup);
    layout->addWidget(behaviorGroup);
    layout->addStretch();
}

void SystemSettings::setupSystemPage()
{
    m_systemPage = new QWidget(this);
    m_pageStack->addWidget(m_systemPage);
    
    QVBoxLayout *layout = new QVBoxLayout(m_systemPage);
    
    // Genel sistem ayarları
    QGroupBox *generalGroup = new QGroupBox("Genel", m_systemPage);
    QVBoxLayout *generalLayout = new QVBoxLayout(generalGroup);
    
    m_startupNotificationsCheck = new QCheckBox("Başlangıç Bildirimlerini Göster", generalGroup);
    m_startupNotificationsCheck->setChecked(true);
    
    generalLayout->addWidget(m_startupNotificationsCheck);
    
    // Güç yönetimi
    QGroupBox *powerGroup = new QGroupBox("Güç Yönetimi", m_systemPage);
    QVBoxLayout *powerLayout = new QVBoxLayout(powerGroup);
    
    QLabel *powerProfileLabel = new QLabel("Güç Profili:");
    m_powerProfileCombo = new QComboBox(powerGroup);
    m_powerProfileCombo->addItem("Dengeli", "balanced");
    m_powerProfileCombo->addItem("Performans", "performance");
    m_powerProfileCombo->addItem("Güç Tasarrufu", "powersave");
    
    QCheckBox *dimScreenCheck = new QCheckBox("Boştayken Ekranı Karart", powerGroup);
    dimScreenCheck->setChecked(true);
    
    QLabel *dimAfterLabel = new QLabel("Karartma Süresi:");
    QSpinBox *dimAfterSpin = new QSpinBox(powerGroup);
    dimAfterSpin->setRange(1, 60);
    dimAfterSpin->setValue(5);
    dimAfterSpin->setSuffix(" dakika");
    
    powerLayout->addWidget(powerProfileLabel);
    powerLayout->addWidget(m_powerProfileCombo);
    powerLayout->addWidget(dimScreenCheck);
    powerLayout->addWidget(dimAfterLabel);
    powerLayout->addWidget(dimAfterSpin);
    
    // Hakkında
    QGroupBox *aboutGroup = new QGroupBox("Hakkında", m_systemPage);
    QVBoxLayout *aboutLayout = new QVBoxLayout(aboutGroup);
    
    QLabel *versionLabel = new QLabel("XenoraOS GUI v1.0", aboutGroup);
    QLabel *copyrightLabel = new QLabel("© 2023 XenoraOS Projesi", aboutGroup);
    
    QPushButton *checkUpdatesButton = new QPushButton("Güncellemeleri Kontrol Et", aboutGroup);
    
    aboutLayout->addWidget(versionLabel);
    aboutLayout->addWidget(copyrightLabel);
    aboutLayout->addWidget(checkUpdatesButton);
    
    // Sayfaya ekle
    layout->addWidget(generalGroup);
    layout->addWidget(powerGroup);
    layout->addWidget(aboutGroup);
    layout->addStretch();
}

void SystemSettings::categoryChanged(QTreeWidgetItem *current, QTreeWidgetItem *previous)
{
    Q_UNUSED(previous);
    
    if (!current) return;
    
    int index = m_categoryList->indexOfTopLevelItem(current);
    m_pageStack->setCurrentIndex(index);
}

void SystemSettings::loadSettings()
{
    // Görünüm ayarları
    QString theme = m_settings.value("theme/name", "CosmicHorizon").toString();
    int themeIndex = m_themeCombo->findData(theme);
    if (themeIndex >= 0) {
        m_themeCombo->setCurrentIndex(themeIndex);
    }
    
    m_wallpaperPath = m_settings.value("desktop/wallpaper", ":/resources/default-wallpaper.jpg").toString();
    QPixmap wallpaperPixmap(m_wallpaperPath);
    if (!wallpaperPixmap.isNull()) {
        m_wallpaperPreview->setPixmap(wallpaperPixmap.scaled(m_wallpaperPreview->size(), Qt::KeepAspectRatio, Qt::SmoothTransformation));
    } else {
        // Duvar kağıdı yüklenemezse gri arkaplan göster
        QPixmap emptyPixmap(m_wallpaperPreview->size());
        emptyPixmap.fill(QColor(80, 80, 80));
        m_wallpaperPreview->setPixmap(emptyPixmap);
    }
    
    m_enableEffectsCheck->setChecked(m_settings.value("effects/enabled", true).toBool());
    m_enableBlurCheck->setChecked(m_settings.value("effects/blur", true).toBool());
    m_transparencySlider->setValue(m_settings.value("effects/transparency", 85).toInt());
    m_animationSpeedSlider->setValue(m_settings.value("effects/animationSpeed", 300).toInt());
    
    // Panel ayarları
    QString panelPosition = m_settings.value("panel/position", "bottom").toString();
    int posIndex = m_panelPositionCombo->findData(panelPosition);
    if (posIndex >= 0) {
        m_panelPositionCombo->setCurrentIndex(posIndex);
    }
    
    m_panelSizeSpin->setValue(m_settings.value("panel/size", 48).toInt());
    m_panelAutoHideCheck->setChecked(m_settings.value("panel/autoHide", false).toBool());
    
    // Sistem ayarları
    m_startupNotificationsCheck->setChecked(m_settings.value("system/startupNotifications", true).toBool());
    
    QString powerProfile = m_settings.value("power/profile", "balanced").toString();
    int powerIndex = m_powerProfileCombo->findData(powerProfile);
    if (powerIndex >= 0) {
        m_powerProfileCombo->setCurrentIndex(powerIndex);
    }
}

void SystemSettings::saveSettings()
{
    // Görünüm ayarları
    m_settings.setValue("theme/name", m_themeCombo->currentData().toString());
    m_settings.setValue("desktop/wallpaper", m_wallpaperPath);
    m_settings.setValue("effects/enabled", m_enableEffectsCheck->isChecked());
    m_settings.setValue("effects/blur", m_enableBlurCheck->isChecked());
    m_settings.setValue("effects/transparency", m_transparencySlider->value());
    m_settings.setValue("effects/animationSpeed", m_animationSpeedSlider->value());
    
    // Panel ayarları
    m_settings.setValue("panel/position", m_panelPositionCombo->currentData().toString());
    m_settings.setValue("panel/size", m_panelSizeSpin->value());
    m_settings.setValue("panel/autoHide", m_panelAutoHideCheck->isChecked());
    
    // Sistem ayarları
    m_settings.setValue("system/startupNotifications", m_startupNotificationsCheck->isChecked());
    m_settings.setValue("power/profile", m_powerProfileCombo->currentData().toString());
    
    m_settings.sync();
}

void SystemSettings::changeWallpaper()
{
    QString filePath = QFileDialog::getOpenFileName(this, "Duvar Kağıdı Seç",
                                                  QDir::homePath(),
                                                  "Resim Dosyaları (*.png *.jpg *.jpeg *.bmp)");
    if (!filePath.isEmpty()) {
        m_wallpaperPath = filePath;
        QPixmap wallpaperPixmap(filePath);
        if (!wallpaperPixmap.isNull()) {
            m_wallpaperPreview->setPixmap(wallpaperPixmap.scaled(m_wallpaperPreview->size(), Qt::KeepAspectRatio, Qt::SmoothTransformation));
        }
    }
}

void SystemSettings::selectTheme(int index)
{
    Q_UNUSED(index);
    // Tema değişikliği, kaydetme ve uygulama
}

void SystemSettings::changePanelPosition(int index)
{
    Q_UNUSED(index);
    // Panel konumu değişikliği için ayarları güncelle
}

void SystemSettings::togglePanelAutoHide(bool checked)
{
    Q_UNUSED(checked);
    // Panel otomatik gizleme ayarını güncelle
}

void SystemSettings::applySettings()
{
    saveSettings();
    
    // Ayarları varsayılan olarak kabul et
    bool result = QMessageBox::question(this, "Ayarları Uygula",
                                     "Değişiklikler kaydedildi. Yeni ayarları uygulamak için XenoraOS yeniden başlatılmalı.\n\nŞimdi yeniden başlatmak istiyor musunuz?") == QMessageBox::Yes;
    
    if (result) {
        // XenoraOS UI'yı yeniden başlat - güncellenmiş metod kullanımı
        QProcess::startDetached("xenora-ui", QStringList());
        QApplication::quit();
    }
}

void SystemSettings::applyAppearanceSettings()
{
    // Görünüm ayarlarını uygula
}

void SystemSettings::applyDesktopSettings()
{
    // Masaüstü ayarlarını uygula
}

void SystemSettings::applyPanelSettings()
{
    // Panel ayarlarını uygula
}

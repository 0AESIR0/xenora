#include "SettingsWindow.h"
#include "StyleManager.h"
#include "../widgets/XenoraButton.h"

#include <QLabel>
#include <QComboBox>
#include <QSlider>
#include <QCheckBox>
#include <QGroupBox>
#include <QRadioButton>
#include <QFileDialog>
#include <QColorDialog>
#include <QFormLayout>
#include <QScrollArea>
#include <QScreen>
#include <QApplication>
#include <QDebug>

SettingsWindow::SettingsWindow(QWidget *parent)
    : QWidget(parent)
{
    setWindowTitle("XenoraOS Settings");
    
    // Set fixed size for settings window
    resize(900, 600);
    
    // Frameless window with custom styling
    setWindowFlags(Qt::FramelessWindowHint | Qt::Dialog);
    setAttribute(Qt::WA_TranslucentBackground);
    
    setupUi();
    loadSettings();
}

SettingsWindow::~SettingsWindow()
{
}

void SettingsWindow::show()
{
    // Center on screen
    QRect screenGeometry = QApplication::primaryScreen()->availableGeometry();
    move(
        screenGeometry.center().x() - width() / 2,
        screenGeometry.center().y() - height() / 2
    );
    
    QWidget::show();
    
    // Set the first category as active initially
    m_categoriesList->setCurrentRow(0);
}

void SettingsWindow::handleCategorySelection(int row)
{
    if (row >= 0) {
        m_pagesWidget->setCurrentIndex(row);
    }
}

void SettingsWindow::applySettings()
{
    saveSettings();
    close();
}

void SettingsWindow::cancelSettings()
{
    loadSettings();
    close();
}

void SettingsWindow::restoreDefaults()
{
    // Reset to default settings
    StyleManager::setTheme(StyleManager::DarkTheme);
    
    // Reload the UI
    loadSettings();
}

void SettingsWindow::changeTheme(int index)
{
    switch (index) {
        case 0: // Light
            StyleManager::setTheme(StyleManager::LightTheme);
            break;
        case 1: // Dark
            StyleManager::setTheme(StyleManager::DarkTheme);
            break;
        case 2: // System
            StyleManager::setTheme(StyleManager::SystemTheme);
            break;
    }
    
    emit themeChanged(index);
}

void SettingsWindow::changeAccentColor(const QColor& color)
{
    emit accentColorChanged(color);
}

void SettingsWindow::browseWallpaper()
{
    QString filePath = QFileDialog::getOpenFileName(
        this,
        "Select Desktop Background",
        QDir::homePath(),
        "Images (*.png *.jpg *.jpeg *.bmp *.gif)"
    );
    
    if (!filePath.isEmpty()) {
        emit desktopBackgroundChanged(filePath);
    }
}

void SettingsWindow::setupUi()
{
    m_mainLayout = new QHBoxLayout(this);
    m_mainLayout->setSpacing(0);
    m_mainLayout->setContentsMargins(0, 0, 0, 0);
    
    // Create and setup the categories list
    m_categoriesList = new QListWidget(this);
    m_categoriesList->setFixedWidth(220);
    m_categoriesList->setFrameShape(QFrame::NoFrame);
    m_categoriesList->setStyleSheet(
        "QListWidget {"
        "   background-color: " + StyleManager::currentTheme().backgroundColor.name() + ";"
        "   border-right: 1px solid " + StyleManager::currentTheme().highlightColor.name() + ";"
        "   border-top-left-radius: 8px;"
        "   border-bottom-left-radius: 8px;"
        "}"
        "QListWidget::item {"
        "   padding: 10px 20px;"
        "   border: none;"
        "}"
        "QListWidget::item:selected {"
        "   background-color: " + StyleManager::currentTheme().accentColor.name() + ";"
        "   color: white;"
        "   border-radius: 4px;"
        "}"
    );
    
    // Settings pages container
    m_pagesWidget = new QStackedWidget(this);
    m_pagesWidget->setStyleSheet(
        "background-color: " + StyleManager::currentTheme().foregroundColor.name() + ";"
        "border-top-right-radius: 8px;"
        "border-bottom-right-radius: 8px;"
    );
    
    // Bottom buttons container
    m_buttonsContainer = new QWidget(this);
    QHBoxLayout* buttonsLayout = new QHBoxLayout(m_buttonsContainer);
    buttonsLayout->setContentsMargins(20, 10, 20, 20);
    buttonsLayout->setSpacing(10);
    
    m_defaultsButton = new XenoraButton("Restore Defaults", this);
    m_cancelButton = new XenoraButton("Cancel", this);
    m_applyButton = new XenoraButton("Apply", this);
    
    m_defaultsButton->setButtonStyle(XenoraButton::Secondary);
    m_cancelButton->setButtonStyle(XenoraButton::Secondary);
    m_applyButton->setButtonStyle(XenoraButton::Primary);
    
    buttonsLayout->addWidget(m_defaultsButton);
    buttonsLayout->addStretch();
    buttonsLayout->addWidget(m_cancelButton);
    buttonsLayout->addWidget(m_applyButton);
    
    // Add categories and pages
    setupCategories();
    
    // Main layout arrangement
    QVBoxLayout* rightLayout = new QVBoxLayout();
    rightLayout->addWidget(m_pagesWidget, 1);
    rightLayout->addWidget(m_buttonsContainer);
    rightLayout->setContentsMargins(0, 0, 0, 0);
    rightLayout->setSpacing(0);
    
    QWidget* rightContainer = new QWidget(this);
    rightContainer->setLayout(rightLayout);
    
    m_mainLayout->addWidget(m_categoriesList);
    m_mainLayout->addWidget(rightContainer, 1);
    
    setLayout(m_mainLayout);
    
    // Connect signals
    connect(m_categoriesList, &QListWidget::currentRowChanged, 
            this, &SettingsWindow::handleCategorySelection);
    connect(m_applyButton, &XenoraButton::clicked, 
            this, &SettingsWindow::applySettings);
    connect(m_cancelButton, &XenoraButton::clicked, 
            this, &SettingsWindow::cancelSettings);
    connect(m_defaultsButton, &XenoraButton::clicked, 
            this, &SettingsWindow::restoreDefaults);
}

void SettingsWindow::setupCategories()
{
    // Add categories to the list
    m_categoriesList->addItem("Appearance");
    m_categoriesList->addItem("Desktop");
    m_categoriesList->addItem("Taskbar");
    m_categoriesList->addItem("System");
    
    // Create and add settings pages
    m_pagesWidget->addWidget(createAppearancePage());
    m_pagesWidget->addWidget(createDesktopPage());
    m_pagesWidget->addWidget(createTaskbarPage());
    m_pagesWidget->addWidget(createSystemPage());
}

QWidget* SettingsWindow::createAppearancePage()
{
    QWidget* page = new QWidget(this);
    QVBoxLayout* layout = new QVBoxLayout(page);
    layout->setContentsMargins(30, 30, 30, 30);
    layout->setSpacing(20);
    
    QLabel* titleLabel = new QLabel("Appearance", page);
    titleLabel->setStyleSheet("font-size: 24px; font-weight: bold;");
    
    QGroupBox* themeGroup = new QGroupBox("Theme", page);
    QVBoxLayout* themeLayout = new QVBoxLayout(themeGroup);
    
    QComboBox* themeSelector = new QComboBox(themeGroup);
    themeSelector->addItem("Light");
    themeSelector->addItem("Dark");
    themeSelector->addItem("System Default");
    
    QLabel* accentLabel = new QLabel("Accent Color:", themeGroup);
    XenoraButton* accentButton = new XenoraButton(themeGroup);
    accentButton->setButtonStyle(XenoraButton::Secondary);
    accentButton->setText("Choose Color");
    accentButton->setFixedWidth(150);
    
    QHBoxLayout* accentLayout = new QHBoxLayout();
    accentLayout->addWidget(accentLabel);
    accentLayout->addWidget(accentButton);
    accentLayout->addStretch();
    
    themeLayout->addWidget(themeSelector);
    themeLayout->addLayout(accentLayout);
    
    QGroupBox* fontGroup = new QGroupBox("Font Settings", page);
    QFormLayout* fontLayout = new QFormLayout(fontGroup);
    
    QComboBox* fontSelector = new QComboBox(fontGroup);
    fontSelector->addItem("XenoraOS");
    fontSelector->addItem("System Default");
    
    QComboBox* fontSizeSelector = new QComboBox(fontGroup);
    fontSizeSelector->addItem("Small");
    fontSizeSelector->addItem("Medium");
    fontSizeSelector->addItem("Large");
    
    fontLayout->addRow("Font Family:", fontSelector);
    fontLayout->addRow("Font Size:", fontSizeSelector);
    
    layout->addWidget(titleLabel);
    layout->addWidget(themeGroup);
    layout->addWidget(fontGroup);
    layout->addStretch();
    
    connect(themeSelector, QOverload<int>::of(&QComboBox::currentIndexChanged), 
            this, &SettingsWindow::changeTheme);
    
    connect(accentButton, &XenoraButton::clicked, [this]() {
        QColor color = QColorDialog::getColor(
            StyleManager::accentColor(), 
            this, 
            "Select Accent Color",
            QColorDialog::ShowAlphaChannel
        );
        
        if (color.isValid()) {
            changeAccentColor(color);
        }
    });
    
    return page;
}

QWidget* SettingsWindow::createDesktopPage()
{
    QWidget* page = new QWidget(this);
    QVBoxLayout* layout = new QVBoxLayout(page);
    layout->setContentsMargins(30, 30, 30, 30);
    layout->setSpacing(20);
    
    QLabel* titleLabel = new QLabel("Desktop", page);
    titleLabel->setStyleSheet("font-size: 24px; font-weight: bold;");
    
    QGroupBox* backgroundGroup = new QGroupBox("Desktop Background", page);
    QVBoxLayout* bgLayout = new QVBoxLayout(backgroundGroup);
    
    QLabel* previewLabel = new QLabel(backgroundGroup);
    previewLabel->setFixedSize(320, 180);
    previewLabel->setStyleSheet("background-color: gray; border: 1px solid " + 
                              StyleManager::currentTheme().highlightColor.name() + ";");
    previewLabel->setAlignment(Qt::AlignCenter);
    previewLabel->setText("Wallpaper Preview");
    
    XenoraButton* browseButton = new XenoraButton("Browse...", backgroundGroup);
    browseButton->setButtonStyle(XenoraButton::Secondary);
    browseButton->setFixedWidth(150);
    
    QHBoxLayout* browseLayout = new QHBoxLayout();
    browseLayout->addWidget(browseButton);
    browseLayout->addStretch();
    
    bgLayout->addWidget(previewLabel);
    bgLayout->addLayout(browseLayout);
    
    QGroupBox* iconGroup = new QGroupBox("Desktop Icons", page);
    QVBoxLayout* iconLayout = new QVBoxLayout(iconGroup);
    
    QCheckBox* showIconsCheck = new QCheckBox("Show desktop icons", iconGroup);
    QCheckBox* showRecycleBinCheck = new QCheckBox("Show Recycle Bin on desktop", iconGroup);
    
    iconLayout->addWidget(showIconsCheck);
    iconLayout->addWidget(showRecycleBinCheck);
    
    layout->addWidget(titleLabel);
    layout->addWidget(backgroundGroup);
    layout->addWidget(iconGroup);
    layout->addStretch();
    
    connect(browseButton, &XenoraButton::clicked, this, &SettingsWindow::browseWallpaper);
    
    return page;
}

QWidget* SettingsWindow::createTaskbarPage()
{
    QWidget* page = new QWidget(this);
    QVBoxLayout* layout = new QVBoxLayout(page);
    layout->setContentsMargins(30, 30, 30, 30);
    layout->setSpacing(20);
    
    QLabel* titleLabel = new QLabel("Taskbar", page);
    titleLabel->setStyleSheet("font-size: 24px; font-weight: bold;");
    
    QGroupBox* positionGroup = new QGroupBox("Taskbar Position", page);
    QVBoxLayout* posLayout = new QVBoxLayout(positionGroup);
    
    QRadioButton* bottomRadio = new QRadioButton("Bottom", positionGroup);
    QRadioButton* topRadio = new QRadioButton("Top", positionGroup);
    QRadioButton* leftRadio = new QRadioButton("Left", positionGroup);
    QRadioButton* rightRadio = new QRadioButton("Right", positionGroup);
    
    bottomRadio->setChecked(true); // Default position
    
    posLayout->addWidget(bottomRadio);
    posLayout->addWidget(topRadio);
    posLayout->addWidget(leftRadio);
    posLayout->addWidget(rightRadio);
    
    QGroupBox* behaviorGroup = new QGroupBox("Behavior", page);
    QVBoxLayout* behaviorLayout = new QVBoxLayout(behaviorGroup);
    
    QCheckBox* autoHideCheck = new QCheckBox("Auto-hide taskbar", behaviorGroup);
    QCheckBox* showLabelsCheck = new QCheckBox("Show application labels", behaviorGroup);
    QCheckBox* groupAppsCheck = new QCheckBox("Group similar applications", behaviorGroup);
    
    behaviorLayout->addWidget(autoHideCheck);
    behaviorLayout->addWidget(showLabelsCheck);
    behaviorLayout->addWidget(groupAppsCheck);
    
    layout->addWidget(titleLabel);
    layout->addWidget(positionGroup);
    layout->addWidget(behaviorGroup);
    layout->addStretch();
    
    return page;
}

QWidget* SettingsWindow::createSystemPage()
{
    QWidget* page = new QWidget(this);
    QVBoxLayout* layout = new QVBoxLayout(page);
    layout->setContentsMargins(30, 30, 30, 30);
    layout->setSpacing(20);
    
    QLabel* titleLabel = new QLabel("System", page);
    titleLabel->setStyleSheet("font-size: 24px; font-weight: bold;");
    
    QGroupBox* performanceGroup = new QGroupBox("Performance Settings", page);
    QVBoxLayout* perfLayout = new QVBoxLayout(performanceGroup);
    
    QCheckBox* animationsCheck = new QCheckBox("Enable animations", performanceGroup);
    QCheckBox* transparencyCheck = new QCheckBox("Enable transparency effects", performanceGroup);
    QCheckBox* blurCheck = new QCheckBox("Enable background blur", performanceGroup);
    
    animationsCheck->setChecked(true);
    transparencyCheck->setChecked(true);
    blurCheck->setChecked(true);
    
    perfLayout->addWidget(animationsCheck);
    perfLayout->addWidget(transparencyCheck);
    perfLayout->addWidget(blurCheck);
    
    QGroupBox* startupGroup = new QGroupBox("Startup & Shutdown", page);
    QVBoxLayout* startupLayout = new QVBoxLayout(startupGroup);
    
    QCheckBox* startupCheck = new QCheckBox("Start XenoraOS at login", startupGroup);
    QCheckBox* restoreCheck = new QCheckBox("Restore previous session", startupGroup);
    
    startupLayout->addWidget(startupCheck);
    startupLayout->addWidget(restoreCheck);
    
    layout->addWidget(titleLabel);
    layout->addWidget(performanceGroup);
    layout->addWidget(startupGroup);
    layout->addStretch();
    
    return page;
}

void SettingsWindow::loadSettings()
{
    // Load current settings into UI
    // This would normally load from a config file or settings storage
    
    // For now, just initialize with default values
}

void SettingsWindow::saveSettings()
{
    // Save settings from UI to storage
    // This would normally save to a config file or settings storage
    
    qDebug() << "Settings applied";
}

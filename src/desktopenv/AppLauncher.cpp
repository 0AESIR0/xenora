#include "AppLauncher.h"
#include "StyleManager.h"
#include "../widgets/XenoraButton.h"

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGridLayout>
#include <QLineEdit>
#include <QLabel>
#include <QScrollArea>
#include <QPropertyAnimation>
#include <QGraphicsDropShadowEffect>
#include <QDir>
#include <QStandardPaths>
#include <QDebug>

AppLauncher::AppLauncher(QWidget *parent)
    : QWidget(parent),
      m_isVisible(false)
{
    setWindowFlags(Qt::FramelessWindowHint | Qt::WindowStaysOnTopHint);
    setAttribute(Qt::WA_TranslucentBackground);
    setFocusPolicy(Qt::StrongFocus);
    
    setupUi();
    loadApplications();
    
    // Hide by default
    hide();
}

AppLauncher::~AppLauncher()
{
}

void AppLauncher::toggle()
{
    if (isVisible()) {
        hideWithAnimation();
    } else {
        showWithAnimation();
    }
}

void AppLauncher::showWithAnimation()
{
    // Set initial position (off-screen)
    setGeometry(m_targetGeometry.x(), 
                m_targetGeometry.y() - m_targetGeometry.height(), 
                m_targetGeometry.width(), 
                m_targetGeometry.height());
    
    show();
    
    // Create animation to slide in from top
    QPropertyAnimation *animation = new QPropertyAnimation(this, "geometry");
    animation->setDuration(250);
    animation->setStartValue(geometry());
    animation->setEndValue(m_targetGeometry);
    animation->setEasingCurve(QEasingCurve::OutCubic);
    
    connect(animation, &QPropertyAnimation::finished, animation, &QPropertyAnimation::deleteLater);
    
    animation->start();
    m_isVisible = true;
    
    // Set focus to search box
    m_searchEdit->setFocus();
}

void AppLauncher::hideWithAnimation()
{
    // Create animation to slide out to top
    QPropertyAnimation *animation = new QPropertyAnimation(this, "geometry");
    animation->setDuration(250);
    animation->setStartValue(geometry());
    animation->setEndValue(QRect(m_targetGeometry.x(), 
                               m_targetGeometry.y() - m_targetGeometry.height(), 
                               m_targetGeometry.width(), 
                               m_targetGeometry.height()));
    animation->setEasingCurve(QEasingCurve::InCubic);
    
    connect(animation, &QPropertyAnimation::finished, this, &QWidget::hide);
    connect(animation, &QPropertyAnimation::finished, animation, &QPropertyAnimation::deleteLater);
    
    animation->start();
    m_isVisible = false;
}

void AppLauncher::setTargetGeometry(const QRect &rect)
{
    m_targetGeometry = rect;
    
    // If visible, update geometry immediately
    if (isVisible()) {
        setGeometry(m_targetGeometry);
    }
}

void AppLauncher::refresh()
{
    // Update styling
    m_mainPanel->setBackgroundColor(StyleManager::currentTheme().panelColor);
    
    // Reload applications
    loadApplications();
}

void AppLauncher::setupUi()
{
    // Main layout
    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(0, 0, 0, 0);
    mainLayout->setSpacing(0);
    
    // Create main panel with blur and shadow effects
    m_mainPanel = new XenoraPanel(this);
    m_mainPanel->setBackgroundColor(StyleManager::currentTheme().panelColor);
    m_mainPanel->setCornerRadius(StyleManager::currentTheme().cornerRadius);
    m_mainPanel->setBlurEnabled(true);
    m_mainPanel->setBlurRadius(30);
    
    // Shadow effect
    QGraphicsDropShadowEffect *shadowEffect = new QGraphicsDropShadowEffect(this);
    shadowEffect->setBlurRadius(30);
    shadowEffect->setColor(QColor(0, 0, 0, 80));
    shadowEffect->setOffset(0, 5);
    m_mainPanel->setGraphicsEffect(shadowEffect);
    
    // Content layout for the panel
    QVBoxLayout *contentLayout = new QVBoxLayout(m_mainPanel);
    contentLayout->setContentsMargins(20, 20, 20, 20);
    contentLayout->setSpacing(15);
    
    // Header with search
    QHBoxLayout *headerLayout = new QHBoxLayout();
    
    QLabel *titleLabel = new QLabel("Applications");
    titleLabel->setStyleSheet("font-size: 24px; font-weight: bold;");
    
    m_searchEdit = new QLineEdit();
    m_searchEdit->setPlaceholderText("Search applications...");
    m_searchEdit->setMinimumWidth(250);
    m_searchEdit->setStyleSheet(QString(
        "QLineEdit {"
        "  background-color: %1;"
        "  border-radius: %2px;"
        "  padding: 8px 12px;"
        "  border: none;"
        "}"
    ).arg(StyleManager::currentTheme().foregroundColor.name())
     .arg(StyleManager::currentTheme().cornerRadius));
    
    connect(m_searchEdit, &QLineEdit::textChanged, this, &AppLauncher::filterApplications);
    
    headerLayout->addWidget(titleLabel);
    headerLayout->addStretch();
    headerLayout->addWidget(m_searchEdit);
    
    contentLayout->addLayout(headerLayout);
    
    // Scroll area for applications
    QScrollArea *scrollArea = new QScrollArea();
    scrollArea->setWidgetResizable(true);
    scrollArea->setFrameShape(QFrame::NoFrame);
    scrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    
    m_appsContainer = new QWidget();
    m_appsLayout = new QGridLayout(m_appsContainer);
    m_appsLayout->setContentsMargins(0, 0, 0, 0);
    m_appsLayout->setSpacing(10);
    
    scrollArea->setWidget(m_appsContainer);
    contentLayout->addWidget(scrollArea);
    
    // Set layout
    mainLayout->addWidget(m_mainPanel);
    
    // Set initial fixed size
    setFixedSize(800, 600);
}

void AppLauncher::loadApplications()
{
    // Clear the existing app grid
    QLayoutItem *item;
    while ((item = m_appsLayout->takeAt(0)) != nullptr) {
        if (item->widget()) {
            delete item->widget();
        }
        delete item;
    }
    
    m_applications.clear();
    
    // TODO: In a real implementation, parse .desktop files from standard locations
    // For now, create some sample applications
    
    // Sample applications
    addApplication("Web Browser", "firefox", ":/icons/apps/browser.svg");
    addApplication("File Manager", "nautilus", ":/icons/apps/files.svg");
    addApplication("Terminal", "gnome-terminal", ":/icons/apps/terminal.svg");
    addApplication("Text Editor", "gedit", ":/icons/apps/editor.svg");
    addApplication("Settings", "gnome-control-center", ":/icons/apps/settings.svg");
    addApplication("Media Player", "vlc", ":/icons/apps/media.svg");
    addApplication("Calculator", "gnome-calculator", ":/icons/apps/calculator.svg");
    addApplication("Calendar", "gnome-calendar", ":/icons/apps/calendar.svg");
    addApplication("Email", "thunderbird", ":/icons/apps/email.svg");
    addApplication("Photos", "shotwell", ":/icons/apps/photos.svg");
    
    // Arrange in grid layout
    arrangeApplications();
}

void AppLauncher::addApplication(const QString &name, const QString &exec, const QString &iconPath)
{
    AppInfo app;
    app.name = name;
    app.exec = exec;
    app.iconPath = iconPath;
    
    m_applications.append(app);
}

void AppLauncher::arrangeApplications(const QString &filter)
{
    int row = 0, col = 0;
    int maxCols = 5; // Number of columns in the grid
    
    for (const AppInfo &app : m_applications) {
        // Apply filter if specified
        if (!filter.isEmpty() && !app.name.contains(filter, Qt::CaseInsensitive)) {
            continue;
        }
        
        // Create app button
        QWidget *appWidget = new QWidget();
        appWidget->setFixedSize(120, 120);
        
        QVBoxLayout *appLayout = new QVBoxLayout(appWidget);
        appLayout->setContentsMargins(10, 10, 10, 10);
        appLayout->setSpacing(5);
        appLayout->setAlignment(Qt::AlignCenter);
        
        XenoraButton *iconButton = new XenoraButton();
        iconButton->setButtonStyle(XenoraButton::Icon);
        iconButton->setIconSize(48);
        
        // Use placeholder icon if the real one not found
        QIcon icon(app.iconPath);
        if (icon.isNull()) {
            icon = QIcon::fromTheme("application-x-executable");
        }
        
        iconButton->setIcon(icon);
        iconButton->setFixedSize(64, 64);
        
        QLabel *nameLabel = new QLabel(app.name);
        nameLabel->setAlignment(Qt::AlignCenter);
        nameLabel->setWordWrap(true);
        
        appLayout->addWidget(iconButton, 0, Qt::AlignCenter);
        appLayout->addWidget(nameLabel, 0, Qt::AlignCenter);
        
        // Connect launch signal
        connect(iconButton, &XenoraButton::clicked, [this, app]() {
            launchApplication(app);
        });
        
        m_appsLayout->addWidget(appWidget, row, col);
        
        // Move to next position
        col++;
        if (col >= maxCols) {
            col = 0;
            row++;
        }
    }
    
    // Add empty stretch at the end
    m_appsLayout->addItem(new QSpacerItem(20, 20, QSizePolicy::Minimum, QSizePolicy::Expanding), row+1, 0);
}

void AppLauncher::filterApplications(const QString &text)
{
    // Clear current layout
    QLayoutItem *item;
    while ((item = m_appsLayout->takeAt(0)) != nullptr) {
        if (item->widget()) {
            delete item->widget();
        }
        delete item;
    }
    
    // Rearrange with filter
    arrangeApplications(text);
}

void AppLauncher::launchApplication(const AppInfo &app)
{
    qDebug() << "Launching application:" << app.name << "(" << app.exec << ")";
    
    // TODO: Implement actual application launching
    
    // Hide the app launcher
    hideWithAnimation();
    
    // Emit signal that an app was launched
    emit applicationLaunched(app.name);
}

void AppLauncher::focusOutEvent(QFocusEvent *event)
{
    QWidget::focusOutEvent(event);
    
    // Auto-hide when focus is lost
    hideWithAnimation();
}

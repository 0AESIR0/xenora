#include "AppLauncher.h"
#include "../widgets/XenoraButton.h"
#include "../widgets/XenoraPanel.h"
#include "StyleManager.h"

#include <QLineEdit>
#include <QVBoxLayout>
#include <QGridLayout>
#include <QScrollArea>
#include <QPainter>
#include <QPainterPath>
#include <QApplication>
#include <QDesktopWidget>
#include <QKeyEvent>
#include <QProcess>
#include <QDebug>

AppLauncher::AppLauncher(QWidget *parent)
    : QWidget(parent)
{
    setWindowFlags(Qt::FramelessWindowHint | Qt::Popup);
    setAttribute(Qt::WA_TranslucentBackground);
    
    setupUi();
    loadApps();
    
    // Start hidden
    hide();
}

AppLauncher::~AppLauncher()
{
}

void AppLauncher::loadApps()
{
    // This would normally load from .desktop files
    // For demonstration, let's add some sample apps
    
    AppInfo terminal;
    terminal.name = "Terminal";
    terminal.executable = "xterm";
    terminal.iconPath = ":/icons/system/terminal.svg";
    terminal.category = "System";
    terminal.description = "Terminal emulator";
    m_apps["Terminal"] = terminal;
    
    AppInfo fileManager;
    fileManager.name = "Files";
    fileManager.executable = "nautilus";
    fileManager.iconPath = ":/icons/system/files.svg";
    fileManager.category = "System";
    fileManager.description = "File manager";
    m_apps["Files"] = fileManager;
    
    AppInfo browser;
    browser.name = "Web Browser";
    browser.executable = "firefox";
    browser.iconPath = ":/icons/system/browser.svg";
    browser.category = "Internet";
    browser.description = "Web browser";
    m_apps["Web Browser"] = browser;
    
    AppInfo settings;
    settings.name = "Settings";
    settings.executable = "xenora-settings";
    settings.iconPath = ":/icons/system/settings.svg";
    settings.category = "System";
    settings.description = "System settings";
    m_apps["Settings"] = settings;
    
    // Create app grid
    int column = 0;
    int row = 0;
    const int maxColumns = 5;
    
    for (auto it = m_apps.begin(); it != m_apps.end(); ++it) {
        createAppButton(it.value(), row, column);
        
        column++;
        if (column >= maxColumns) {
            column = 0;
            row++;
        }
    }
}

void AppLauncher::setPosition(const QPoint &pos)
{
    move(pos);
}

void AppLauncher::show()
{
    // Set position to bottom left of screen, above the panel
    QRect screenGeometry = QApplication::desktop()->availableGeometry();
    int panelHeight = 48; // Assuming panel height
    
    int x = screenGeometry.x();
    int y = screenGeometry.height() - height() - panelHeight;
    
    move(x, y);
    
    // Clear search when showing
    m_searchBox->clear();
    m_searchBox->setFocus();
    
    QWidget::show();
}

void AppLauncher::hide()
{
    QWidget::hide();
}

void AppLauncher::toggleVisibility()
{
    isVisible() ? hide() : show();
}

void AppLauncher::searchApps(const QString &searchText)
{
    // Clear the current layout
    QLayoutItem *child;
    while ((child = m_appsLayout->takeAt(0)) != nullptr) {
        if (child->widget()) {
            child->widget()->hide();
            delete child->widget();
        }
        delete child;
    }
    
    if (searchText.isEmpty()) {
        // Show all apps
        loadApps();
        return;
    }
    
    // Filter apps based on search text
    int column = 0;
    int row = 0;
    const int maxColumns = 5;
    
    QString lowerSearchText = searchText.toLower();
    
    for (auto it = m_apps.begin(); it != m_apps.end(); ++it) {
        if (it.value().name.toLower().contains(lowerSearchText) ||
            it.value().description.toLower().contains(lowerSearchText)) {
            
            createAppButton(it.value(), row, column);
            
            column++;
            if (column >= maxColumns) {
                column = 0;
                row++;
            }
        }
    }
}

void AppLauncher::paintEvent(QPaintEvent *event)
{
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);
    
    // Draw rounded rect background
    QColor bgColor = StyleManager::currentTheme().foregroundColor;
    painter.setPen(Qt::NoPen);
    painter.setBrush(bgColor);
    
    int cornerRadius = StyleManager::cornerRadius();
    painter.drawRoundedRect(rect(), cornerRadius, cornerRadius);
    
    QWidget::paintEvent(event);
}

void AppLauncher::keyPressEvent(QKeyEvent *event)
{
    if (event->key() == Qt::Key_Escape) {
        hide();
        event->accept();
    } else {
        QWidget::keyPressEvent(event);
    }
}

bool AppLauncher::eventFilter(QObject *watched, QEvent *event)
{
    if (watched == m_searchBox && event->type() == QEvent::KeyPress) {
        QKeyEvent *keyEvent = static_cast<QKeyEvent*>(event);
        if (keyEvent->key() == Qt::Key_Escape) {
            hide();
            return true;
        }
    }
    
    return QWidget::eventFilter(watched, event);
}

void AppLauncher::setupUi()
{
    // Set fixed size
    setFixedSize(600, 500);
    
    // Main layout
    m_mainLayout = new QVBoxLayout(this);
    m_mainLayout->setContentsMargins(20, 20, 20, 20);
    m_mainLayout->setSpacing(15);
    
    // Search box
    m_searchBox = new QLineEdit(this);
    m_searchBox->setPlaceholderText("Search applications...");
    m_searchBox->setMinimumHeight(40);
    m_searchBox->installEventFilter(this);
    
    connect(m_searchBox, &QLineEdit::textChanged, this, &AppLauncher::searchApps);
    
    // Apps area
    m_appsContainer = new QWidget();
    m_appsLayout = new QGridLayout(m_appsContainer);
    m_appsLayout->setContentsMargins(0, 0, 0, 0);
    m_appsLayout->setSpacing(15);
    
    m_appsScrollArea = new QScrollArea(this);
    m_appsScrollArea->setWidgetResizable(true);
    m_appsScrollArea->setWidget(m_appsContainer);
    m_appsScrollArea->setFrameShape(QFrame::NoFrame);
    
    // Add to main layout
    m_mainLayout->addWidget(m_searchBox);
    m_mainLayout->addWidget(m_appsScrollArea, 1);
    
    setLayout(m_mainLayout);
    
    // Style the app launcher
    m_searchBox->setStyleSheet(
        "QLineEdit {"
        "  background-color: " + StyleManager::currentTheme().backgroundColor.name() + ";"
        "  border-radius: " + QString::number(StyleManager::cornerRadius()) + "px;"
        "  padding: 8px 12px;"
        "  color: " + StyleManager::currentTheme().textColor.name() + ";"
        "  font-size: 14px;"
        "}"
    );
    
    m_appsScrollArea->setStyleSheet(
        "QScrollArea {"
        "  background-color: transparent;"
        "}"
        "QScrollBar:vertical {"
        "  background-color: transparent;"
        "  width: 8px;"
        "  margin: 0px;"
        "}"
        "QScrollBar::handle:vertical {"
        "  background-color: " + StyleManager::currentTheme().accentColor.name() + ";"
        "  border-radius: 4px;"
        "  min-height: 30px;"
        "}"
        "QScrollBar::add-line:vertical, QScrollBar::sub-line:vertical {"
        "  height: 0px;"
        "}"
    );
}

void AppLauncher::launchApp(const QString &appName)
{
    if (!m_apps.contains(appName)) {
        qWarning() << "App not found:" << appName;
        return;
    }
    
    const AppInfo &app = m_apps[appName];
    
    // Launch the app
    QProcess::startDetached(app.executable);
    
    emit appLaunched(appName);
    
    // Hide the launcher
    hide();
}

void AppLauncher::createAppButton(const AppInfo &app, int row, int column)
{
    XenoraButton *appButton = new XenoraButton(this);
    appButton->setText(app.name);
    appButton->setButtonStyle(XenoraButton::Secondary);
    appButton->setMinimumSize(100, 100);
    
    // Use icon if available
    if (!app.iconPath.isEmpty()) {
        appButton->setIcon(QIcon(app.iconPath));
        appButton->setIconSize(QSize(32, 32));
    }
    
    connect(appButton, &XenoraButton::clicked, this, [this, app]() {
        launchApp(app.name);
    });
    
    m_appsLayout->addWidget(appButton, row, column);
}

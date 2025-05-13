#include "StyleManager.h"
#include <QStyleFactory>
#include <QFont>

XenoraTheme StyleManager::m_currentTheme = StyleManager::createDarkTheme();

void StyleManager::applyStyle(QApplication* app)
{
    app->setStyle(QStyleFactory::create("Fusion"));
    
    // Use system theme by default
    setTheme(SystemTheme);
    
    // Apply the stylesheet
    app->setStyleSheet(getStylesheet());
}

void StyleManager::setTheme(Theme theme)
{
    switch (theme) {
        case LightTheme:
            m_currentTheme = createLightTheme();
            break;
        case DarkTheme:
            m_currentTheme = createDarkTheme();
            break;
        case SystemTheme:
            // TODO: Detect system theme
            m_currentTheme = createDarkTheme();
            break;
    }
}

XenoraTheme StyleManager::currentTheme()
{
    return m_currentTheme;
}

QString StyleManager::getStylesheet()
{
    return buildStylesheet(m_currentTheme);
}

QColor StyleManager::accentColor()
{
    return m_currentTheme.accentColor;
}

QColor StyleManager::backgroundColor()
{
    return m_currentTheme.backgroundColor;
}

QColor StyleManager::foregroundColor()
{
    return m_currentTheme.foregroundColor;
}

QColor StyleManager::textColor()
{
    return m_currentTheme.textColor;
}

QColor StyleManager::panelColor()
{
    return m_currentTheme.panelColor;
}

int StyleManager::cornerRadius()
{
    return m_currentTheme.cornerRadius;
}

XenoraTheme StyleManager::createLightTheme()
{
    XenoraTheme theme;
    theme.name = "Light";
    theme.darkMode = false;
    theme.accentColor = QColor(0, 120, 212);
    theme.backgroundColor = QColor(245, 245, 245);
    theme.foregroundColor = QColor(255, 255, 255);
    theme.highlightColor = QColor(230, 240, 250);
    theme.shadowColor = QColor(0, 0, 0, 40);
    theme.textColor = QColor(30, 30, 30);
    theme.secondaryTextColor = QColor(120, 120, 120);
    theme.panelColor = QColor(255, 255, 255, 220);
    theme.windowColor = QColor(255, 255, 255);
    theme.cornerRadius = 8;
    theme.shadowRadius = 15;
    theme.borderWidth = 1;
    return theme;
}

XenoraTheme StyleManager::createDarkTheme()
{
    XenoraTheme theme;
    theme.name = "Dark";
    theme.darkMode = true;
    theme.accentColor = QColor(75, 145, 255);
    theme.backgroundColor = QColor(25, 25, 25);
    theme.foregroundColor = QColor(40, 40, 40);
    theme.highlightColor = QColor(55, 55, 55);
    theme.shadowColor = QColor(0, 0, 0, 70);
    theme.textColor = QColor(240, 240, 240);
    theme.secondaryTextColor = QColor(180, 180, 180);
    theme.panelColor = QColor(35, 35, 35, 230);
    theme.windowColor = QColor(45, 45, 45);
    theme.cornerRadius = 8;
    theme.shadowRadius = 20;
    theme.borderWidth = 1;
    return theme;
}

QString StyleManager::buildStylesheet(const XenoraTheme& theme)
{
    return QString(R"(
        QWidget {
            font-family: "XenoraOS", sans-serif;
            color: %1;
            background-color: transparent;
        }
        
        QMainWindow, QDialog {
            background-color: %2;
        }
        
        QToolTip {
            color: %1;
            background-color: %3;
            border: 1px solid %4;
            border-radius: %9px;
            padding: 4px;
        }
        
        QPushButton {
            background-color: %5;
            border-radius: %9px;
            border: none;
            padding: 8px 16px;
            color: %6;
        }
        
        QPushButton:hover {
            background-color: %7;
        }
        
        QPushButton:pressed {
            background-color: %8;
        }
        
        QLineEdit, QTextEdit, QPlainTextEdit {
            background-color: %3;
            border: 1px solid %4;
            border-radius: %9px;
            padding: 4px;
            color: %1;
        }
        
        QComboBox, QSpinBox {
            background-color: %3;
            border: 1px solid %4;
            border-radius: %9px;
            padding: 4px;
            color: %1;
            min-height: 24px;
        }
        
        QScrollBar:vertical {
            background-color: %3;
            width: 12px;
            margin: 0;
        }
        
        QScrollBar::handle:vertical {
            background-color: %4;
            border-radius: 4px;
            min-height: 20px;
            margin: 2px;
        }
        
        QScrollBar:horizontal {
            background-color: %3;
            height: 12px;
            margin: 0;
        }
        
        QScrollBar::handle:horizontal {
            background-color: %4;
            border-radius: 4px;
            min-width: 20px;
            margin: 2px;
        }
        
        QMenu {
            background-color: %3;
            border: 1px solid %4;
            border-radius: %9px;
            padding: 4px;
        }
        
        QMenu::item {
            padding: 6px 24px 6px 12px;
            border-radius: 4px;
        }
        
        QMenu::item:selected {
            background-color: %7;
        }
        
        QMenuBar {
            background-color: %3;
        }
        
        QMenuBar::item:selected {
            background-color: %7;
            border-radius: 4px;
        }
    )")
    .arg(theme.textColor.name())
    .arg(theme.backgroundColor.name())
    .arg(theme.foregroundColor.name())
    .arg(theme.highlightColor.name())
    .arg(theme.accentColor.name())
    .arg(theme.darkMode ? "#FFFFFF" : "#FFFFFF")
    .arg(theme.accentColor.lighter(120).name())
    .arg(theme.accentColor.darker(120).name())
    .arg(QString::number(theme.cornerRadius));
}

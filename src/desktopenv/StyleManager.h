#pragma once

#include <QApplication>
#include <QString>
#include <QColor>
#include <QMap>

// Theme settings struct
struct XenoraTheme {
    QString name;
    QColor accentColor;
    QColor backgroundColor;
    QColor foregroundColor;
    QColor highlightColor;
    QColor shadowColor;
    QColor textColor;
    QColor secondaryTextColor;
    QColor panelColor;
    QColor windowColor;
    bool darkMode;
    int cornerRadius;
    int shadowRadius;
    int borderWidth;
};

class StyleManager
{
public:
    enum Theme {
        LightTheme,
        DarkTheme,
        SystemTheme
    };
    
    static void applyStyle(QApplication* app);
    static void setTheme(Theme theme);
    static XenoraTheme currentTheme();
    
    static QString getStylesheet();
    static QColor accentColor();
    static QColor backgroundColor();
    static QColor foregroundColor();
    static QColor textColor();
    static QColor panelColor();
    static int cornerRadius();
    
private:
    static XenoraTheme m_currentTheme;
    static XenoraTheme createLightTheme();
    static XenoraTheme createDarkTheme();
    static QString buildStylesheet(const XenoraTheme& theme);
};

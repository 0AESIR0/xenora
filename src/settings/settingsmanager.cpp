#include "settingsmanager.h"
#include <QDebug>
#include <QDir>
#include <QStandardPaths>

SettingsManager::SettingsManager(QObject *parent)
    : QObject(parent),
      m_settings(nullptr),
      m_backgroundPath(":/backgrounds/background.jpg"),
      m_themeName("Default"),
      m_accentColor(0, 120, 215),
      m_darkMode(true),
      m_fontFamily("Xenora Font"),
      m_fontSize(10)
{
    // Create settings object
    QString configPath = QStandardPaths::writableLocation(QStandardPaths::AppConfigLocation);
    QDir().mkpath(configPath);
    m_settings = new QSettings(configPath + "/xenora-settings.ini", QSettings::IniFormat);
    
    // Load settings
    loadSettings();
}

SettingsManager::~SettingsManager() {
    saveSettings();
    delete m_settings;
}

QString SettingsManager::getBackgroundPath() const {
    return m_backgroundPath;
}

void SettingsManager::setBackgroundPath(const QString &path) {
    if (m_backgroundPath != path) {
        m_backgroundPath = path;
        emit backgroundChanged(path);
    }
}

QString SettingsManager::getThemeName() const {
    return m_themeName;
}

void SettingsManager::setThemeName(const QString &name) {
    if (m_themeName != name) {
        m_themeName = name;
        emit themeChanged(name);
    }
}

QColor SettingsManager::getAccentColor() const {
    return m_accentColor;
}

void SettingsManager::setAccentColor(const QColor &color) {
    if (m_accentColor != color) {
        m_accentColor = color;
        emit accentColorChanged(color);
    }
}

bool SettingsManager::isDarkMode() const {
    return m_darkMode;
}

void SettingsManager::setDarkMode(bool enabled) {
    if (m_darkMode != enabled) {
        m_darkMode = enabled;
        emit darkModeChanged(enabled);
    }
}

QString SettingsManager::getFontFamily() const {
    return m_fontFamily;
}

void SettingsManager::setFontFamily(const QString &family) {
    if (m_fontFamily != family) {
        m_fontFamily = family;
        emit fontFamilyChanged(family);
    }
}

int SettingsManager::getFontSize() const {
    return m_fontSize;
}

void SettingsManager::setFontSize(int size) {
    if (m_fontSize != size) {
        m_fontSize = size;
        emit fontSizeChanged(size);
    }
}

void SettingsManager::saveSettings() {
    m_settings->beginGroup("Appearance");
    m_settings->setValue("BackgroundPath", m_backgroundPath);
    m_settings->setValue("ThemeName", m_themeName);
    m_settings->setValue("AccentColor", m_accentColor);
    m_settings->setValue("DarkMode", m_darkMode);
    m_settings->endGroup();
    
    m_settings->beginGroup("Fonts");
    m_settings->setValue("FontFamily", m_fontFamily);
    m_settings->setValue("FontSize", m_fontSize);
    m_settings->endGroup();
    
    m_settings->sync();
}

void SettingsManager::loadSettings() {
    m_settings->beginGroup("Appearance");
    m_backgroundPath = m_settings->value("BackgroundPath", m_backgroundPath).toString();
    m_themeName = m_settings->value("ThemeName", m_themeName).toString();
    m_accentColor = m_settings->value("AccentColor", m_accentColor).value<QColor>();
    m_darkMode = m_settings->value("DarkMode", m_darkMode).toBool();
    m_settings->endGroup();
    
    m_settings->beginGroup("Fonts");
    m_fontFamily = m_settings->value("FontFamily", m_fontFamily).toString();
    m_fontSize = m_settings->value("FontSize", m_fontSize).toInt();
    m_settings->endGroup();
}

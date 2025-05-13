#ifndef SETTINGSMANAGER_H
#define SETTINGSMANAGER_H

#include <QObject>
#include <QSettings>
#include <QString>
#include <QColor>

class SettingsManager : public QObject {
    Q_OBJECT

public:
    explicit SettingsManager(QObject *parent = nullptr);
    ~SettingsManager();

    // Background settings
    QString getBackgroundPath() const;
    void setBackgroundPath(const QString &path);

    // Theme settings
    QString getThemeName() const;
    void setThemeName(const QString &name);
    QColor getAccentColor() const;
    void setAccentColor(const QColor &color);
    bool isDarkMode() const;
    void setDarkMode(bool enabled);

    // Font settings
    QString getFontFamily() const;
    void setFontFamily(const QString &family);
    int getFontSize() const;
    void setFontSize(int size);

    // Save and load settings
    void saveSettings();
    void loadSettings();

signals:
    void backgroundChanged(const QString &path);
    void themeChanged(const QString &name);
    void accentColorChanged(const QColor &color);
    void darkModeChanged(bool enabled);
    void fontFamilyChanged(const QString &family);
    void fontSizeChanged(int size);

private:
    QSettings *m_settings;
    
    // Settings cache
    QString m_backgroundPath;
    QString m_themeName;
    QColor m_accentColor;
    bool m_darkMode;
    QString m_fontFamily;
    int m_fontSize;
};

#endif // SETTINGSMANAGER_H

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QSettings>
#include "desktopmanager.h"
#include "panel.h"

// Tema verileri için yapı
struct ThemeData {
    QColor primaryColor;   // Ana arka plan rengi
    QColor accentColor;    // Vurgu rengi
    QColor textColor;      // Metin rengi
    QColor glowColor;      // Parıltı efekti rengi
    qreal transparencyLevel; // Şeffaflık seviyesi (0-1)
    int blurRadius;        // Bulanıklık seviyesi
    int animationSpeed;    // Animasyon hızı (ms)
};

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

    ThemeData themeData() const { return m_themeData; }

public slots:
    void changeTheme(const QString &themeName);

private slots:
    void handleDesktopMenu(const QPoint &pos);
    void changeWallpaper();
    void openSettings();
    void logout();
    void showThemeSelector();

private:
    void createDesktop();
    void createPanel();
    void loadSettings();
    void saveSettings();
    void loadThemeData();
    void loadThemeByName(const QString &themeName);
    void applyVisualEffects();

    DesktopManager *m_desktopManager;
    Panel *m_panel;
    QSettings m_settings;
    ThemeData m_themeData;
};

#endif // MAINWINDOW_H

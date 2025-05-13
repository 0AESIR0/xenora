#ifndef SYSTEMSETTINGS_H
#define SYSTEMSETTINGS_H

#include <QMainWindow>
#include <QTreeWidget>
#include <QStackedWidget>
#include <QSettings>
#include <QLabel>
#include <QPushButton>
#include <QComboBox>
#include <QSlider>
#include <QGroupBox>
#include <QSpinBox>
#include <QCheckBox>
#include <QColorDialog>

class SystemSettings : public QMainWindow
{
    Q_OBJECT
    
public:
    explicit SystemSettings(QWidget *parent = nullptr);
    ~SystemSettings();
    
private slots:
    void categoryChanged(QTreeWidgetItem *current, QTreeWidgetItem *previous);
    void applyAppearanceSettings();
    void applyDesktopSettings();
    void applyPanelSettings();
    
    void changeWallpaper();
    void selectTheme(int index);
    void changePanelPosition(int index);
    void togglePanelAutoHide(bool checked);
    void applySettings();
    
private:
    void setupUI();
    void setupCategoryList();
    void setupAppearancePage();
    void setupDesktopPage();
    void setupPanelPage();
    void setupSystemPage();
    
    void loadSettings();
    void saveSettings();
    
    QTreeWidget *m_categoryList;
    QStackedWidget *m_pageStack;
    
    // Ayarlar sayfaları için widgetlar
    QWidget *m_appearancePage;
    QWidget *m_desktopPage;
    QWidget *m_panelPage;
    QWidget *m_systemPage;
    
    // Görünüm ayarları
    QComboBox *m_themeCombo;
    QPushButton *m_wallpaperButton;
    QLabel *m_wallpaperPreview;
    QSlider *m_transparencySlider;
    QSlider *m_animationSpeedSlider;
    QCheckBox *m_enableEffectsCheck;
    QCheckBox *m_enableBlurCheck;
    
    // Panel ayarları
    QComboBox *m_panelPositionCombo;
    QSpinBox *m_panelSizeSpin;
    QCheckBox *m_panelAutoHideCheck;
    
    // Sistem ayarları
    QCheckBox *m_startupNotificationsCheck;
    QComboBox *m_powerProfileCombo;
    
    QString m_wallpaperPath;
    QSettings m_settings;
    bool m_needsRestart;
};

#endif // SYSTEMSETTINGS_H

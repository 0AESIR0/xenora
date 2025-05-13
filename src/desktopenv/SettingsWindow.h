#pragma once

#include <QWidget>
#include <QTabWidget>
#include <QStackedWidget>
#include <QListWidget>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QLabel>
#include <memory>

class XenoraButton;

class SettingsWindow : public QWidget
{
    Q_OBJECT
    
public:
    explicit SettingsWindow(QWidget *parent = nullptr);
    ~SettingsWindow();
    
    void show();
    
signals:
    void themeChanged(int themeIndex);
    void accentColorChanged(const QColor& color);
    void fontSizeChanged(int size);
    void desktopBackgroundChanged(const QString& path);
    
private slots:
    void handleCategorySelection(int row);
    void applySettings();
    void cancelSettings();
    void restoreDefaults();
    void changeTheme(int index);
    void changeAccentColor(const QColor& color);
    void browseWallpaper();
    
private:
    QHBoxLayout* m_mainLayout;
    QListWidget* m_categoriesList;
    QStackedWidget* m_pagesWidget;
    
    // Settings Pages
    QWidget* createAppearancePage();
    QWidget* createDesktopPage();
    QWidget* createTaskbarPage();
    QWidget* createSystemPage();
    
    // Bottom buttons
    QWidget* m_buttonsContainer;
    XenoraButton* m_applyButton;
    XenoraButton* m_cancelButton;
    XenoraButton* m_defaultsButton;
    
    void setupUi();
    void setupCategories();
    void loadSettings();
    void saveSettings();
};

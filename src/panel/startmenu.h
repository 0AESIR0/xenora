#ifndef STARTMENU_H
#define STARTMENU_H

#include <QWidget>
#include <QVBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QList>

class StartMenu : public QWidget {
    Q_OBJECT

public:
    explicit StartMenu(QWidget *parent = nullptr);
    ~StartMenu();

signals:
    void appLaunched(const QString &appName);
    void powerAction(const QString &action);

private slots:
    void onAppButtonClicked();
    void onPowerButtonClicked();
    void onSettingsButtonClicked();

private:
    QVBoxLayout *m_mainLayout;
    QWidget *m_userSection;
    QWidget *m_appsSection;
    QWidget *m_powerSection;
    
    QLabel *m_userAvatar;
    QLabel *m_userName;
    
    QList<QPushButton*> m_appButtons;
    QPushButton *m_settingsButton;
    QPushButton *m_powerButton;
    
    void initializeMenu();
    void createUserSection();
    void createAppsSection();
    void createPowerSection();
    void addAppButton(const QString &name, const QString &iconPath);
};

#endif // STARTMENU_H

#ifndef LAUNCHER_H
#define LAUNCHER_H

#include <QWidget>
#include <QList>
#include <QPushButton>
#include <QSettings>

class AppButton : public QPushButton
{
    Q_OBJECT
    
public:
    AppButton(const QString &appName, const QString &command, const QIcon &icon, QWidget *parent = nullptr);
    
    QString appName() const { return m_appName; }
    QString command() const { return m_command; }
    
private slots:
    void launchApp();
    
private:
    QString m_appName;
    QString m_command;
};

class Launcher : public QWidget
{
    Q_OBJECT
    
public:
    explicit Launcher(QWidget *parent = nullptr);
    ~Launcher();
    
public slots:
    void addApplication(const QString &appName, const QString &command, const QIcon &icon);
    void removeApplication(const QString &appName);
    
private:
    void loadFavoriteApps();
    void saveFavoriteApps();
    
    QList<AppButton*> m_appButtons;
    QSettings m_settings;
};

#endif // LAUNCHER_H

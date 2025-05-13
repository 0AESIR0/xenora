#pragma once

#include <QWidget>
#include <QMap>
#include <QString>

class QLineEdit;
class QVBoxLayout;
class QGridLayout;
class QScrollArea;
class XenoraButton;

struct AppInfo {
    QString name;
    QString executable;
    QString iconPath;
    QString category;
    QString description;
};

class AppLauncher : public QWidget
{
    Q_OBJECT
    
public:
    explicit AppLauncher(QWidget *parent = nullptr);
    ~AppLauncher();
    
    void loadApps();
    void setPosition(const QPoint &pos);
    
public slots:
    void show();
    void hide();
    void toggleVisibility();
    void searchApps(const QString &searchText);
    
signals:
    void appLaunched(const QString &appName);
    
protected:
    void paintEvent(QPaintEvent *event) override;
    void keyPressEvent(QKeyEvent *event) override;
    bool eventFilter(QObject *watched, QEvent *event) override;
    
private:
    QLineEdit *m_searchBox;
    QScrollArea *m_appsScrollArea;
    QWidget *m_appsContainer;
    QGridLayout *m_appsLayout;
    QVBoxLayout *m_mainLayout;
    QMap<QString, AppInfo> m_apps;
    
    void setupUi();
    void launchApp(const QString &appName);
    void createAppButton(const AppInfo &app, int row, int column);
};

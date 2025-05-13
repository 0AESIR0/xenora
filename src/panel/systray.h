#ifndef SYSTEMTRAY_H
#define SYSTEMTRAY_H

#include <QWidget>
#include <QHBoxLayout>
#include <QPushButton>
#include <QList>

class SystemTray : public QWidget {
    Q_OBJECT

public:
    explicit SystemTray(QWidget *parent = nullptr);
    ~SystemTray();

    void addTrayIcon(const QString &name, const QIcon &icon);

signals:
    void trayIconClicked(const QString &name);

private slots:
    void onTrayIconClicked();

private:
    QHBoxLayout *m_layout;
    QList<QPushButton*> m_trayIcons;
    
    void initializeSystemTray();
    void setupDefaultIcons();
};

#endif // SYSTEMTRAY_H

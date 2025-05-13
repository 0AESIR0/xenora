#pragma once

#include <QWidget>
#include <QBoxLayout>
#include <QList>
#include <QPushButton>

class SystemTrayButton : public QPushButton {
    Q_OBJECT
    
public:
    explicit SystemTrayButton(const QIcon &icon, QWidget *parent = nullptr);
};

class SystemTray : public QWidget {
    Q_OBJECT

public:
    explicit SystemTray(QWidget *parent = nullptr);
    ~SystemTray();
    
private:
    QHBoxLayout *m_layout;
    QList<SystemTrayButton*> m_buttons;
    
    void addWifiButton();
    void addVolumeButton();
    void addBatteryButton();
    void addPowerButton();
};

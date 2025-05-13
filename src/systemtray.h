#ifndef SYSTEMTRAY_H
#define SYSTEMTRAY_H

#include <QWidget>
#include <QVector>

class QLabel;
class QPushButton;

class SystemTray : public QWidget
{
    Q_OBJECT

public:
    explicit SystemTray(QWidget *parent = nullptr);
    ~SystemTray();

private slots:
    void updateClock();
    void showVolumeControl();
    void showNetworkSettings();
    void showBatteryInfo();

private:
    void setupTray();
    
    QLabel *m_clockLabel;
    QPushButton *m_volumeButton;
    QPushButton *m_networkButton;
    QPushButton *m_batteryButton;
};

#endif // SYSTEMTRAY_H

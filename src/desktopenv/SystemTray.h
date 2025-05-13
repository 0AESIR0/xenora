#pragma once

#include <QWidget>
#include <QVector>

class QHBoxLayout;
class XenoraButton;

class SystemTrayIcon : public QObject
{
    Q_OBJECT
    
public:
    explicit SystemTrayIcon(QObject *parent = nullptr);
    virtual ~SystemTrayIcon();
    
    QString id() const { return m_id; }
    QString tooltip() const { return m_tooltip; }
    QIcon icon() const { return m_icon; }
    
    void setId(const QString &id) { m_id = id; }
    void setTooltip(const QString &tooltip) { m_tooltip = tooltip; }
    void setIcon(const QIcon &icon) { m_icon = icon; }
    
    virtual void activate() = 0;
    virtual QWidget* createWidget() = 0;
    
protected:
    QString m_id;
    QString m_tooltip;
    QIcon m_icon;
};

class NetworkIcon : public SystemTrayIcon
{
    Q_OBJECT
    
public:
    explicit NetworkIcon(QObject *parent = nullptr);
    
    void activate() override;
    QWidget* createWidget() override;
};

class VolumeIcon : public SystemTrayIcon
{
    Q_OBJECT
    
public:
    explicit VolumeIcon(QObject *parent = nullptr);
    
    void activate() override;
    QWidget* createWidget() override;
};

class BatteryIcon : public SystemTrayIcon
{
    Q_OBJECT
    
public:
    explicit BatteryIcon(QObject *parent = nullptr);
    
    void activate() override;
    QWidget* createWidget() override;
};

class SystemTray : public QWidget
{
    Q_OBJECT
    
public:
    explicit SystemTray(QWidget *parent = nullptr);
    ~SystemTray();
    
    void addIcon(SystemTrayIcon *icon);
    void removeIcon(const QString &id);
    
public slots:
    void openQuickSettings();
    
private:
    QHBoxLayout *m_layout;
    QVector<SystemTrayIcon*> m_icons;
    
    void setupUi();
    void setupDefaultIcons();
};

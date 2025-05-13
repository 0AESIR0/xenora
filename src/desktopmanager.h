#ifndef DESKTOPMANAGER_H
#define DESKTOPMANAGER_H

#include <QWidget>
#include <QPixmap>
#include <QVector>
#include <QFileSystemWatcher>

class DesktopIcon;

class DesktopManager : public QWidget
{
    Q_OBJECT

public:
    explicit DesktopManager(QWidget *parent = nullptr);
    ~DesktopManager();

    void setWallpaper(const QString &path);
    QString currentWallpaper() const;

public slots:
    void refreshDesktopIcons();

protected:
    void paintEvent(QPaintEvent *event) override;
    void resizeEvent(QResizeEvent *event) override;

private:
    void loadIcons();
    void arrangeIcons();

    QPixmap m_wallpaper;
    QString m_wallpaperPath;
    QVector<DesktopIcon*> m_icons;
    QFileSystemWatcher m_fileWatcher;
};

#endif // DESKTOPMANAGER_H

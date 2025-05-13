#pragma once

#include <QWidget>
#include <QPixmap>
#include <QPoint>
#include <QVector>
#include <QMenu>

class DesktopManager : public QWidget {
    Q_OBJECT

public:
    explicit DesktopManager(QWidget *parent = nullptr);
    ~DesktopManager();

    void setWallpaper(const QString &path);

protected:
    void paintEvent(QPaintEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;
    void contextMenuEvent(QContextMenuEvent *event) override;

private:
    QPixmap m_wallpaper;
    QPoint m_lastPos;
    QMenu *m_contextMenu;

    void createContextMenu();
    
private slots:
    void changeWallpaper();
    void openTerminal();
    void openSettings();
};

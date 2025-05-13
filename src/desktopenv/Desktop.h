#pragma once

#include <QWidget>
#include <QPixmap>
#include <QString>

class QVBoxLayout;
class QGridLayout;

class Desktop : public QWidget
{
    Q_OBJECT
    
public:
    explicit Desktop(QWidget *parent = nullptr);
    ~Desktop();
    
    void refresh();
    void setWallpaper(const QString &path);
    
    QPixmap wallpaper() const { return m_wallpaper; }
    
public slots:
    void handleDesktopContextMenu(const QPoint &pos);
    
protected:
    void paintEvent(QPaintEvent *event) override;
    void contextMenuEvent(QContextMenuEvent *event) override;
    
private:
    QPixmap m_wallpaper;
    QVBoxLayout *m_mainLayout;
    QGridLayout *m_iconsLayout;
    
    void setupUi();
    void loadDesktopIcons();
};

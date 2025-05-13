#ifndef DESKTOP_H
#define DESKTOP_H

#include <QWidget>
#include <QPixmap>
#include "desktopiconview.h"
#include "../panel/startmenu.h"

class Desktop : public QWidget {
    Q_OBJECT

public:
    explicit Desktop(QWidget *parent = nullptr);
    ~Desktop();

    void setBackground(const QString &path);

public slots:
    void onStartMenuToggled(bool visible);

protected:
    void paintEvent(QPaintEvent *event) override;
    void resizeEvent(QResizeEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;

private:
    QPixmap m_background;
    DesktopIconView *m_iconView;
    StartMenu *m_startMenu;
    
    void initializeDesktop();
    void setupDesktopIcons();
};

#endif // DESKTOP_H

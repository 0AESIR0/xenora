#pragma once

#include <QWidget>
#include <QBoxLayout>
#include <QPushButton>
#include <QLabel>

#include "panel/taskbar.h"
#include "widgets/systemtray.h"

class Panel : public QWidget {
    Q_OBJECT

public:
    explicit Panel(QWidget *parent = nullptr);
    ~Panel();

    int height() const { return 48; }

protected:
    void paintEvent(QPaintEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;

private:
    QHBoxLayout *m_layout;
    QPushButton *m_startButton;
    Taskbar *m_taskbar;
    SystemTray *m_systemTray;
    QLabel *m_clockLabel;
    
    void createStartMenu();
    void updateClock();

private slots:
    void startMenuClicked();
    void clockTick();
};

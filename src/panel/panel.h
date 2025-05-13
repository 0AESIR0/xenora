#ifndef PANEL_H
#define PANEL_H

#include <QWidget>
#include <QPushButton>
#include <QHBoxLayout>
#include <QLabel>
#include "systray.h"

class Panel : public QWidget {
    Q_OBJECT

public:
    explicit Panel(QWidget *parent = nullptr);
    ~Panel();

    void updateGeometry(const QRect &screenGeometry);

signals:
    void startMenuToggled(bool visible);

private slots:
    void onStartButtonClicked();
    void updateClock();

private:
    QWidget *m_panelWidget;
    QPushButton *m_startButton;
    QLabel *m_clockLabel;
    SystemTray *m_systemTray;
    QHBoxLayout *m_layout;
    bool m_startMenuVisible;
    
    void initializePanel();
    void setupStartButton();
    void setupClock();
};

#endif // PANEL_H

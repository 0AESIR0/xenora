#ifndef TASKBAR_H
#define TASKBAR_H

#include <QWidget>
#include <QMap>
#include <QVector>
#include <xcb/xcb.h>

class TaskButton;

class Taskbar : public QWidget
{
    Q_OBJECT

public:
    explicit Taskbar(QWidget *parent = nullptr);
    ~Taskbar();

private slots:
    void updateTasks();
    void activateWindow(xcb_window_t window);

private:
    void setupTaskbar();
    QVector<xcb_window_t> getWindowList();
    QString getWindowTitle(xcb_window_t window);
    QIcon getWindowIcon(xcb_window_t window);

    QMap<xcb_window_t, TaskButton*> m_taskButtons;
};

#endif // TASKBAR_H

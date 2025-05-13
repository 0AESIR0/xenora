#ifndef WINDOWMANAGER_H
#define WINDOWMANAGER_H

#include <QObject>
#include <QList>
#include <QWidget>
#include "../xenorawindow.h"

class WindowManager : public QObject {
    Q_OBJECT

public:
    explicit WindowManager(QObject *parent = nullptr);
    ~WindowManager();

    XenoraWindow* createWindow(const QString &title = "");
    void closeWindow(XenoraWindow *window);
    void minimizeWindow(XenoraWindow *window);
    void maximizeWindow(XenoraWindow *window);
    void bringToFront(XenoraWindow *window);
    
    QList<XenoraWindow*> getAllWindows() const;
    XenoraWindow* getActiveWindow() const;

private slots:
    void onWindowActivated();

private:
    QList<XenoraWindow*> m_windows;
    XenoraWindow* m_activeWindow;
};

#endif // WINDOWMANAGER_H

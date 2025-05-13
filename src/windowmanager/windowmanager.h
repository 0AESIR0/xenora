#ifndef WINDOWMANAGER_H
#define WINDOWMANAGER_H

#include <QObject>
#include <QList>
#include <QWidget>
#include <QEvent>
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

protected:
    // Add event filter method to catch window activation
    bool eventFilter(QObject *watched, QEvent *event) override;

private slots:
    void onWindowActivated();

private:
    QList<XenoraWindow*> m_windows;
    XenoraWindow* m_activeWindow;
};

#endif // WINDOWMANAGER_H

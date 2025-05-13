#pragma once

#include <QObject>
#include <QMap>
#include <QRect>
#include <QWindow>
#include <memory>
#include <X11/Xlib.h>

class XenoraWindow;

class WindowManager : public QObject
{
    Q_OBJECT
    
public:
    explicit WindowManager(QObject *parent = nullptr);
    ~WindowManager();
    
    void initialize();
    void refresh();
    
    void setAvailableGeometry(const QRect& geometry);
    QRect availableGeometry() const { return m_availableGeometry; }
    
    void minimizeWindow(XenoraWindow* window);
    void maximizeWindow(XenoraWindow* window);
    void restoreWindow(XenoraWindow* window);
    void closeWindow(XenoraWindow* window);
    
signals:
    void windowAdded(XenoraWindow* window);
    void windowRemoved(XenoraWindow* window);
    void windowMinimized(XenoraWindow* window);
    void windowMaximized(XenoraWindow* window);
    void windowRestored(XenoraWindow* window);
    void windowFocused(XenoraWindow* window);
    
private slots:
    void detectWindows();
    void handleX11Event();
    
private:
    Display* m_display;
    Window m_rootWindow;
    QRect m_availableGeometry;
    QMap<Window, XenoraWindow*> m_windows;
    
    void registerForWindowEvents();
    void captureExistingWindows();
    XenoraWindow* createWindowFrame(Window window);
};

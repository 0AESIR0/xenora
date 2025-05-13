#include "WindowManager.h"
#include "../widgets/XenoraWindow.h"

#include <QApplication>
#include <QX11Info>
#include <QTimer>
#include <QDebug>

#include <X11/Xlib.h>
#include <X11/Xatom.h>
#include <X11/Xutil.h>

WindowManager::WindowManager(QObject *parent)
    : QObject(parent),
      m_display(nullptr),
      m_rootWindow(0)
{
}

WindowManager::~WindowManager()
{
    if (m_display) {
        XCloseDisplay(m_display);
    }
}

void WindowManager::initialize()
{
    // Open X display
    m_display = XOpenDisplay(nullptr);
    if (!m_display) {
        qCritical() << "Failed to open X display";
        return;
    }
    
    // Get root window
    m_rootWindow = DefaultRootWindow(m_display);
    
    // Register for X11 events
    registerForWindowEvents();
    
    // Capture existing windows
    captureExistingWindows();
    
    // Set up a timer to periodically check for new windows
    QTimer *timer = new QTimer(this);
    connect(timer, &QTimer::timeout, this, &WindowManager::detectWindows);
    timer->start(2000); // Check every 2 seconds
    
    qDebug() << "Window Manager initialized";
}

void WindowManager::refresh()
{
    captureExistingWindows();
}

void WindowManager::setAvailableGeometry(const QRect& geometry)
{
    m_availableGeometry = geometry;
    
    // Update window positions if needed
    for (auto window : m_windows.values()) {
        if (window->windowState() == XenoraWindow::Maximized) {
            window->setGeometry(m_availableGeometry);
        }
    }
}

void WindowManager::minimizeWindow(XenoraWindow* window)
{
    if (window) {
        window->minimize();
        emit windowMinimized(window);
    }
}

void WindowManager::maximizeWindow(XenoraWindow* window)
{
    if (window) {
        window->maximize();
        emit windowMaximized(window);
    }
}

void WindowManager::restoreWindow(XenoraWindow* window)
{
    if (window) {
        window->restore();
        emit windowRestored(window);
    }
}

void WindowManager::closeWindow(XenoraWindow* window)
{
    if (window) {
        window->closeWindow();
        m_windows.remove(m_windows.key(window));
        emit windowRemoved(window);
    }
}

void WindowManager::detectWindows()
{
    captureExistingWindows();
}

void WindowManager::handleX11Event()
{
    // This would normally handle X11 events such as window creation, destruction,
    // property changes, etc. In a real implementation, we would connect this to
    // X11 event processing.
    
    // For the prototype, we're using polling via detectWindows instead.
}

void WindowManager::registerForWindowEvents()
{
    if (!m_display || !m_rootWindow) return;
    
    // Select events on the root window to be notified of window changes
    XSelectInput(m_display, m_rootWindow, SubstructureNotifyMask | SubstructureRedirectMask);
    
    // Flush the display to ensure our request is processed
    XFlush(m_display);
}

void WindowManager::captureExistingWindows()
{
    if (!m_display || !m_rootWindow) return;
    
    Window root_return, parent_return;
    Window *children = nullptr;
    unsigned int nchildren = 0;
    
    // Get the list of windows
    if (XQueryTree(m_display, m_rootWindow, &root_return, &parent_return, &children, &nchildren)) {
        for (unsigned int i = 0; i < nchildren; i++) {
            Window window = children[i];
            
            // Skip windows we already know about
            if (m_windows.contains(window)) continue;
            
            // Check if this is a normal window we should manage
            XWindowAttributes attrs;
            if (XGetWindowAttributes(m_display, window, &attrs)) {
                if (attrs.map_state == IsViewable) {
                    // Get window properties to determine if it's a normal application window
                    Atom type;
                    int format;
                    unsigned long nitems, bytes_after;
                    unsigned char *data = nullptr;
                    
                    Atom windowTypeAtom = XInternAtom(m_display, "_NET_WM_WINDOW_TYPE", False);
                    
                    if (XGetWindowProperty(m_display, window, windowTypeAtom, 0, 32, False, 
                                          AnyPropertyType, &type, &format, &nitems, 
                                          &bytes_after, &data) == Success) {
                        
                        if (data) {
                            // Normal application windows have _NET_WM_WINDOW_TYPE_NORMAL
                            Atom normalWindowAtom = XInternAtom(m_display, "_NET_WM_WINDOW_TYPE_NORMAL", False);
                            Atom dialogWindowAtom = XInternAtom(m_display, "_NET_WM_WINDOW_TYPE_DIALOG", False);
                            
                            Atom windowType = *(Atom*)data;
                            if (windowType == normalWindowAtom || windowType == dialogWindowAtom) {
                                createWindowFrame(window);
                            }
                            
                            XFree(data);
                        }
                    }
                }
            }
        }
        
        if (children) {
            XFree(children);
        }
    }
}

XenoraWindow* WindowManager::createWindowFrame(Window window)
{
    // In a real window manager, here we would:
    // 1. Reparent the X11 window into our frame
    // 2. Setup event handling
    // 3. Manage window state
    
    // For our prototype, we'll create a simulated window
    XenoraWindow* frameWindow = new XenoraWindow();
    
    // Get window name
    char *windowName = nullptr;
    if (XFetchName(m_display, window, &windowName) && windowName) {
        frameWindow->setTitle(QString::fromLocal8Bit(windowName));
        XFree(windowName);
    } else {
        frameWindow->setTitle("Application");
    }
    
    // Set a placeholder content widget
    QWidget* content = new QWidget(frameWindow);
    content->setStyleSheet("background-color: rgba(255, 255, 255, 30);");
    frameWindow->setContent(content);
    
    // Position window
    int x, y;
    unsigned int width, height, border_width, depth;
    Window root_return;
    if (XGetGeometry(m_display, window, &root_return, &x, &y, &width, &height, &border_width, &depth)) {
        frameWindow->setGeometry(x, y, width, height);
    } else {
        // Default size and position
        frameWindow->setGeometry(100, 100, 800, 600);
    }
    
    // Connect signals
    connect(frameWindow, &XenoraWindow::windowClosed, [this, frameWindow, window]() {
        m_windows.remove(window);
        emit windowRemoved(frameWindow);
        frameWindow->deleteLater();
    });
    
    // Store in our map
    m_windows[window] = frameWindow;
    
    // Show the window
    frameWindow->show();
    
    // Notify others about the new window
    emit windowAdded(frameWindow);
    
    return frameWindow;
}

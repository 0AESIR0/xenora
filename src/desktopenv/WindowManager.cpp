#include "WindowManager.h"
#include "../widgets/XenoraWindow.h"
#include <QDebug>
#include <QX11Info>
#include <QTimer>
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
    // Clean up X11 resources
    if (m_display) {
        XCloseDisplay(m_display);
    }
}

void WindowManager::initialize()
{
    qDebug() << "Initializing XenoraOS Window Manager...";
    
    // Open connection to X server
    m_display = XOpenDisplay(nullptr);
    if (!m_display) {
        qCritical() << "Failed to open X display";
        return;
    }
    
    m_rootWindow = DefaultRootWindow(m_display);
    
    // Register for window events
    registerForWindowEvents();
    
    // Start periodic window detection
    QTimer *timer = new QTimer(this);
    connect(timer, &QTimer::timeout, this, &WindowManager::detectWindows);
    timer->start(1000); // Check for new windows every second
    
    // Capture existing windows
    captureExistingWindows();
    
    qDebug() << "XenoraOS Window Manager initialized successfully";
}

void WindowManager::refresh()
{
    captureExistingWindows();
}

void WindowManager::setAvailableGeometry(const QRect& geometry)
{
    m_availableGeometry = geometry;
    
    // Update all managed window geometries
    for (auto window : m_windows.values()) {
        if (window->isMaximized()) {
            window->setGeometry(m_availableGeometry);
        }
    }
}

void WindowManager::minimizeWindow(XenoraWindow* window)
{
    if (!window) return;
    
    Window xWindow = window->windowId();
    XIconifyWindow(m_display, xWindow, DefaultScreen(m_display));
    XFlush(m_display);
    
    emit windowMinimized(window);
}

void WindowManager::maximizeWindow(XenoraWindow* window)
{
    if (!window) return;
    
    window->setGeometry(m_availableGeometry);
    window->setMaximized(true);
    
    // Set X11 maximize property
    Atom wmState = XInternAtom(m_display, "_NET_WM_STATE", False);
    Atom maxH = XInternAtom(m_display, "_NET_WM_STATE_MAXIMIZED_HORZ", False);
    Atom maxV = XInternAtom(m_display, "_NET_WM_STATE_MAXIMIZED_VERT", False);
    
    XClientMessageEvent xclient;
    memset(&xclient, 0, sizeof(xclient));
    
    xclient.type = ClientMessage;
    xclient.window = window->windowId();
    xclient.message_type = wmState;
    xclient.format = 32;
    xclient.data.l[0] = 1; // Add property
    xclient.data.l[1] = maxH;
    xclient.data.l[2] = maxV;
    xclient.data.l[3] = 1; // Source indication
    
    XSendEvent(m_display, m_rootWindow, False,
               SubstructureNotifyMask | SubstructureRedirectMask,
               (XEvent*)&xclient);
    XFlush(m_display);
    
    emit windowMaximized(window);
}

void WindowManager::restoreWindow(XenoraWindow* window)
{
    if (!window) return;
    
    // Remove X11 maximize property
    Atom wmState = XInternAtom(m_display, "_NET_WM_STATE", False);
    Atom maxH = XInternAtom(m_display, "_NET_WM_STATE_MAXIMIZED_HORZ", False);
    Atom maxV = XInternAtom(m_display, "_NET_WM_STATE_MAXIMIZED_VERT", False);
    
    XClientMessageEvent xclient;
    memset(&xclient, 0, sizeof(xclient));
    
    xclient.type = ClientMessage;
    xclient.window = window->windowId();
    xclient.message_type = wmState;
    xclient.format = 32;
    xclient.data.l[0] = 0; // Remove property
    xclient.data.l[1] = maxH;
    xclient.data.l[2] = maxV;
    xclient.data.l[3] = 1; // Source indication
    
    XSendEvent(m_display, m_rootWindow, False,
               SubstructureNotifyMask | SubstructureRedirectMask,
               (XEvent*)&xclient);
    
    // Restore to previous size
    window->setMaximized(false);
    window->restorePreviousGeometry();
    
    XFlush(m_display);
    
    emit windowRestored(window);
}

void WindowManager::closeWindow(XenoraWindow* window)
{
    if (!window) return;
    
    // Send WM_DELETE_WINDOW message
    Atom wmDeleteWindow = XInternAtom(m_display, "WM_DELETE_WINDOW", False);
    
    XEvent ev;
    memset(&ev, 0, sizeof(ev));
    
    ev.xclient.type = ClientMessage;
    ev.xclient.window = window->windowId();
    ev.xclient.message_type = XInternAtom(m_display, "WM_PROTOCOLS", False);
    ev.xclient.format = 32;
    ev.xclient.data.l[0] = wmDeleteWindow;
    ev.xclient.data.l[1] = CurrentTime;
    
    XSendEvent(m_display, window->windowId(), False, NoEventMask, &ev);
    XFlush(m_display);
}

void WindowManager::detectWindows()
{
    Atom type;
    int format;
    unsigned long count, after;
    unsigned char *data = nullptr;
    Window *windows;
    
    Atom netClientList = XInternAtom(m_display, "_NET_CLIENT_LIST", False);
    
    int status = XGetWindowProperty(m_display, m_rootWindow, netClientList,
                                    0L, ~0L, False, XA_WINDOW, &type, &format,
                                    &count, &after, &data);
    
    if (status != Success) {
        qWarning() << "Failed to get window list";
        return;
    }
    
    windows = reinterpret_cast<Window*>(data);
    
    QSet<Window> currentWindows;
    for (unsigned long i = 0; i < count; i++) {
        currentWindows.insert(windows[i]);
        
        // If this is a new window, add it
        if (!m_windows.contains(windows[i])) {
            XenoraWindow* window = createWindowFrame(windows[i]);
            if (window) {
                m_windows[windows[i]] = window;
                emit windowAdded(window);
            }
        }
    }
    
    // Remove windows that no longer exist
    QList<Window> removedWindows;
    for (auto it = m_windows.begin(); it != m_windows.end(); ++it) {
        if (!currentWindows.contains(it.key())) {
            emit windowRemoved(it.value());
            delete it.value();
            removedWindows.append(it.key());
        }
    }
    
    for (const Window& win : removedWindows) {
        m_windows.remove(win);
    }
    
    XFree(data);
}

void WindowManager::handleX11Event()
{
    // This would be connected to X11 event processing
    // Implementation depends on how you're integrating with X11 events
}

void WindowManager::registerForWindowEvents()
{
    XSelectInput(m_display, m_rootWindow,
                 SubstructureNotifyMask | PropertyChangeMask);
    
    // Get notified about window creation, destruction, property changes
    XSetWindowAttributes attrs;
    attrs.event_mask = PropertyChangeMask | StructureNotifyMask | SubstructureNotifyMask;
    XChangeWindowAttributes(m_display, m_rootWindow, CWEventMask, &attrs);
}

void WindowManager::captureExistingWindows()
{
    detectWindows();
}

XenoraWindow* WindowManager::createWindowFrame(Window window)
{
    // Check if window should be managed
    XWindowAttributes attrs;
    if (!XGetWindowAttributes(m_display, window, &attrs)) {
        return nullptr;
    }
    
    // Skip invisible or special windows
    if (attrs.override_redirect || attrs.map_state != IsViewable) {
        return nullptr;
    }
    
    // Get window title
    char* windowName = nullptr;
    XFetchName(m_display, window, &windowName);
    QString title = windowName ? QString::fromLocal8Bit(windowName) : "Unknown";
    if (windowName) XFree(windowName);
    
    // Create Xenora window frame
    XenoraWindow* xenoraWindow = new XenoraWindow(nullptr, window);
    xenoraWindow->setTitle(title);
    xenoraWindow->setGeometry(attrs.x, attrs.y, attrs.width, attrs.height);
    xenoraWindow->show();
    
    return xenoraWindow;
}

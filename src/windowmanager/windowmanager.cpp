#include "windowmanager.h"
#include <QDebug>
#include <QApplication>

WindowManager::WindowManager(QObject *parent)
    : QObject(parent),
      m_activeWindow(nullptr)
{
    // Set up an event filter to detect window activation
    qApp->installEventFilter(this);
}

WindowManager::~WindowManager() {
    // Close all windows
    while (!m_windows.isEmpty()) {
        XenoraWindow *window = m_windows.takeFirst();
        window->close();
        delete window;
    }
}

XenoraWindow* WindowManager::createWindow(const QString &title) {
    XenoraWindow *window = new XenoraWindow(title);
    
    // Connect signals/slots
    connect(window, &XenoraWindow::destroyed, [this, window]() {
        this->closeWindow(window);
    });
    
    // No need for the direct windowActivated connection anymore
    // We'll use event filtering instead
    
    // Add to window list
    m_windows.append(window);
    
    // Set as active window
    m_activeWindow = window;
    
    // Show the window
    window->show();
    window->raise();
    window->activateWindow();
    
    return window;
}

// Add an event filter to catch window activation
bool WindowManager::eventFilter(QObject *watched, QEvent *event) {
    if (event->type() == QEvent::WindowActivate) {
        XenoraWindow *window = qobject_cast<XenoraWindow*>(watched);
        if (window && m_windows.contains(window)) {
            m_activeWindow = window;
        }
    }
    
    // Let the event continue
    return QObject::eventFilter(watched, event);
}

void WindowManager::closeWindow(XenoraWindow *window) {
    if (!window) return;
    
    // Remove from window list
    m_windows.removeOne(window);
    
    // Update active window
    if (m_activeWindow == window) {
        m_activeWindow = m_windows.isEmpty() ? nullptr : m_windows.last();
        if (m_activeWindow) {
            m_activeWindow->raise();
            m_activeWindow->activateWindow();
        }
    }
}

void WindowManager::minimizeWindow(XenoraWindow *window) {
    if (!window) return;
    
    window->setWindowState(window->windowState() | Qt::WindowMinimized);
}

void WindowManager::maximizeWindow(XenoraWindow *window) {
    if (!window) return;
    
    if (window->windowState() & Qt::WindowMaximized) {
        window->setWindowState(window->windowState() & ~Qt::WindowMaximized);
    } else {
        window->setWindowState(window->windowState() | Qt::WindowMaximized);
    }
}

void WindowManager::bringToFront(XenoraWindow *window) {
    if (!window) return;
    
    window->raise();
    window->activateWindow();
    m_activeWindow = window;
}

QList<XenoraWindow*> WindowManager::getAllWindows() const {
    return m_windows;
}

XenoraWindow* WindowManager::getActiveWindow() const {
    return m_activeWindow;
}

void WindowManager::onWindowActivated() {
    XenoraWindow *window = qobject_cast<XenoraWindow*>(sender());
    if (window) {
        m_activeWindow = window;
    }
}

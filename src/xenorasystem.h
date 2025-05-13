#include "xenorasystem.h"
#include <QDebug>
#include <QFile>
#include <QDir>
#include <QPainter>
#include <QRandomGenerator>
#include <QLinearGradient>
#include <QColor>
#include <QPoint>

#ifndef XENORASYSTEM_H
#define XENORASYSTEM_H

#include <QObject>
#include <QScreen>
#include <QApplication>
#include "desktop/desktop.h"
#include "panel/panel.h"
#include "windowmanager/windowmanager.h"
#include "settings/settingsmanager.h"

class XenoraSystem : public QObject {
    Q_OBJECT

public:
    explicit XenoraSystem(QObject *parent = nullptr);
    ~XenoraSystem();

    void initialize();
    void shutdown();
    
    Desktop* getDesktop() const { return m_desktop; }
    Panel* getPanel() const { return m_panel; }
    WindowManager* getWindowManager() const { return m_windowManager; }
    SettingsManager* getSettingsManager() const { return m_settingsManager; }

private slots:
    void onScreenGeometryChanged(const QRect &geometry);

private:
    Desktop* m_desktop;
    Panel* m_panel;
    WindowManager* m_windowManager;
    SettingsManager* m_settingsManager;
    
    void setupSystemConnections();
    void createFallbackResources(); // Yeni fonksiyon
};

#endif // XENORASYSTEM_H

#include "Desktop.h"
#include "StyleManager.h"

#include <QPainter>
#include <QVBoxLayout>
#include <QGridLayout>
#include <QMenu>
#include <QAction>
#include <QContextMenuEvent>
#include <QDebug>

Desktop::Desktop(QWidget *parent)
    : QWidget(parent)
{
    setWindowFlags(Qt::FramelessWindowHint | Qt::WindowStaysOnBottomHint);
    setAttribute(Qt::WA_TranslucentBackground);
    
    // Load default wallpaper
    setWallpaper(":/backgrounds/default.jpg");
    
    setupUi();
}

Desktop::~Desktop()
{
}

void Desktop::refresh()
{
    update();
}

void Desktop::setWallpaper(const QString &path)
{
    m_wallpaper = QPixmap(path);
    if (m_wallpaper.isNull()) {
        qWarning() << "Failed to load wallpaper:" << path;
        // Set a default solid color as fallback
        m_wallpaper = QPixmap(1, 1);
        m_wallpaper.fill(StyleManager::currentTheme().backgroundColor);
    }
    update();
}

void Desktop::handleDesktopContextMenu(const QPoint &pos)
{
    QMenu contextMenu(tr("Desktop Menu"), this);
    
    QAction *refreshAction = contextMenu.addAction(tr("Refresh"));
    QAction *newFolderAction = contextMenu.addAction(tr("New Folder"));
    contextMenu.addSeparator();
    QAction *changeWallpaperAction = contextMenu.addAction(tr("Change Wallpaper"));
    QAction *settingsAction = contextMenu.addAction(tr("Settings"));
    
    QAction *selectedAction = contextMenu.exec(mapToGlobal(pos));
    
    if (selectedAction == refreshAction) {
        refresh();
    } else if (selectedAction == changeWallpaperAction) {
        // TODO: Implement wallpaper chooser
        qDebug() << "Change wallpaper requested";
    } else if (selectedAction == settingsAction) {
        // TODO: Open settings window
        qDebug() << "Settings requested";
    } else if (selectedAction == newFolderAction) {
        // TODO: Create new folder on desktop
        qDebug() << "New folder requested";
    }
}

void Desktop::paintEvent(QPaintEvent *event)
{
    QPainter painter(this);
    
    // Draw wallpaper scaled to fit screen
    if (!m_wallpaper.isNull()) {
        painter.drawPixmap(rect(), m_wallpaper, m_wallpaper.rect());
    } else {
        // Fallback solid color
        painter.fillRect(rect(), StyleManager::currentTheme().backgroundColor);
    }
    
    QWidget::paintEvent(event);
}

void Desktop::contextMenuEvent(QContextMenuEvent *event)
{
    handleDesktopContextMenu(event->pos());
}

void Desktop::setupUi()
{
    m_mainLayout = new QVBoxLayout(this);
    m_mainLayout->setContentsMargins(20, 20, 20, 20);
    
    // Grid layout for desktop icons
    m_iconsLayout = new QGridLayout();
    m_iconsLayout->setContentsMargins(0, 0, 0, 0);
    m_iconsLayout->setSpacing(20);
    
    m_mainLayout->addLayout(m_iconsLayout);
    m_mainLayout->addStretch(1); // Push all icons to the top
    
    setLayout(m_mainLayout);
    
    loadDesktopIcons();
}

void Desktop::loadDesktopIcons()
{
    // This would load desktop icons from a configuration
    // For now, we'll leave it empty
}

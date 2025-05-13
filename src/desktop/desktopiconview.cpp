#include "desktopiconview.h"
#include <QPainter>
#include <QMouseEvent>
#include <QApplication>
#include <QDebug>

DesktopIconView::DesktopIconView(QWidget *parent)
    : QWidget(parent),
      m_iconSize(64),
      m_spacing(20),
      m_textHeight(20),
      m_columns(0),
      m_draggedIcon(-1),
      m_isDragging(false)
{
    setAcceptDrops(true);
    setAttribute(Qt::WA_TransparentForMouseEvents, false);
    setObjectName("DesktopIconView");
}

DesktopIconView::~DesktopIconView() {
    // Clean up resources
}

void DesktopIconView::addIcon(const QString &name, const QString &iconPath) {
    DesktopIcon icon;
    icon.name = name;
    icon.icon = QPixmap(iconPath).scaled(m_iconSize, m_iconSize, Qt::KeepAspectRatio, Qt::SmoothTransformation);
    icon.selected = false;
    
    m_icons.append(icon);
    calculateIconPositions();
    update();
}

void DesktopIconView::calculateIconPositions() {
    if (width() <= 0) return;
    
    // Calculate how many columns can fit
    m_columns = qMax(1, (width() / (m_iconSize + m_spacing)));
    
    // Position each icon
    for (int i = 0; i < m_icons.size(); ++i) {
        int row = i / m_columns;
        int col = i % m_columns;
        
        int x = col * (m_iconSize + m_spacing) + m_spacing;
        int y = row * (m_iconSize + m_textHeight + m_spacing) + m_spacing;
        
        // Icon rectangle includes both the icon and the text area
        m_icons[i].rect = QRect(x, y, m_iconSize, m_iconSize + m_textHeight);
    }
}

void DesktopIconView::paintEvent(QPaintEvent *event) {
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);
    painter.setRenderHint(QPainter::TextAntialiasing);
    
    for (int i = 0; i < m_icons.size(); ++i) {
        const DesktopIcon &icon = m_icons[i];
        
        // Skip painting the icon if it's being dragged
        if (m_isDragging && i == m_draggedIcon) continue;
        
        // Draw icon background if selected
        if (icon.selected) {
            painter.setPen(Qt::NoPen);
            painter.setBrush(QColor(255, 255, 255, 40));
            painter.drawRoundedRect(icon.rect, 5, 5);
        }
        
        // Draw icon
        QRect iconRect(icon.rect.x(), icon.rect.y(), m_iconSize, m_iconSize);
        painter.drawPixmap(iconRect, icon.icon);
        
        // Draw text
        QRect textRect(icon.rect.x(), icon.rect.y() + m_iconSize, m_iconSize, m_textHeight);
        painter.setPen(Qt::white);
        painter.drawText(textRect, Qt::AlignHCenter | Qt::AlignVCenter, icon.name);
    }
    
    // Draw dragged icon if needed
    if (m_isDragging && m_draggedIcon >= 0 && m_draggedIcon < m_icons.size()) {
        const DesktopIcon &icon = m_icons[m_draggedIcon];
        
        // Translate to mouse position
        QPoint pos = mapFromGlobal(QCursor::pos());
        pos -= QPoint(m_iconSize / 2, m_iconSize / 2);
        
        // Draw semi-transparent icon
        painter.setOpacity(0.7);
        painter.drawPixmap(QRect(pos.x(), pos.y(), m_iconSize, m_iconSize), icon.icon);
        painter.setOpacity(1.0);
    }
    
    QWidget::paintEvent(event);
}

void DesktopIconView::mousePressEvent(QMouseEvent *event) {
    if (event->button() == Qt::LeftButton) {
        int iconIndex = getIconAtPosition(event->pos());
        
        // Deselect all icons first
        for (int i = 0; i < m_icons.size(); ++i) {
            m_icons[i].selected = false;
        }
        
        if (iconIndex >= 0) {
            m_icons[iconIndex].selected = true;
            m_draggedIcon = iconIndex;
            m_dragStartPos = event->pos();
        }
        
        update();
    }
    
    QWidget::mousePressEvent(event);
}

void DesktopIconView::mouseDoubleClickEvent(QMouseEvent *event) {
    if (event->button() == Qt::LeftButton) {
        int iconIndex = getIconAtPosition(event->pos());
        
        if (iconIndex >= 0) {
            emit iconDoubleClicked(m_icons[iconIndex].name);
        }
    }
    
    QWidget::mouseDoubleClickEvent(event);
}

void DesktopIconView::mouseMoveEvent(QMouseEvent *event) {
    if (event->buttons() & Qt::LeftButton && m_draggedIcon >= 0) {
        QPoint diff = event->pos() - m_dragStartPos;
        
        // Start dragging if the distance is large enough
        if (!m_isDragging && diff.manhattanLength() > QApplication::startDragDistance()) {
            m_isDragging = true;
        }
        
        if (m_isDragging) {
            update(); // Repaint to show dragging
        }
    }
    
    QWidget::mouseMoveEvent(event);
}

void DesktopIconView::mouseReleaseEvent(QMouseEvent *event) {
    if (event->button() == Qt::LeftButton && m_isDragging) {
        // Finish dragging
        if (m_draggedIcon >= 0 && rect().contains(event->pos())) {
            // Update icon position
            QPoint iconPos = event->pos();
            iconPos.rx() -= m_iconSize / 2;
            iconPos.ry() -= m_iconSize / 2;
            
            // Snap to grid (optional)
            int col = qBound(0, iconPos.x() / (m_iconSize + m_spacing), m_columns - 1);
            int row = qBound(0, iconPos.y() / (m_iconSize + m_textHeight + m_spacing), (m_icons.size() + m_columns - 1) / m_columns - 1);
            
            int x = col * (m_iconSize + m_spacing) + m_spacing;
            int y = row * (m_iconSize + m_textHeight + m_spacing) + m_spacing;
            
            m_icons[m_draggedIcon].rect = QRect(x, y, m_iconSize, m_iconSize + m_textHeight);
        }
        
        m_isDragging = false;
        m_draggedIcon = -1;
        update();
    }
    
    QWidget::mouseReleaseEvent(event);
}

int DesktopIconView::getIconAtPosition(const QPoint &pos) {
    for (int i = 0; i < m_icons.size(); ++i) {
        if (m_icons[i].rect.contains(pos)) {
            return i;
        }
    }
    
    return -1;
}

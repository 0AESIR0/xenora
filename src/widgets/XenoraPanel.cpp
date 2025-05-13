#include "XenoraPanel.h"
#include "../desktopenv/StyleManager.h"

#include <QPainter>
#include <QPainterPath>
#include <QDebug>

XenoraPanel::XenoraPanel(QWidget *parent)
    : QWidget(parent),
      m_backgroundColor(StyleManager::currentTheme().panelColor),
      m_borderColor(StyleManager::currentTheme().highlightColor),
      m_blurRadius(20),
      m_cornerRadius(StyleManager::cornerRadius()),
      m_borderWidth(1),
      m_blurEnabled(true)
{
    setAttribute(Qt::WA_TranslucentBackground);
}

void XenoraPanel::setBackgroundColor(const QColor &color)
{
    m_backgroundColor = color;
    update();
}

QColor XenoraPanel::backgroundColor() const
{
    return m_backgroundColor;
}

void XenoraPanel::setBlurRadius(int radius)
{
    m_blurRadius = radius;
    update();
}

int XenoraPanel::blurRadius() const
{
    return m_blurRadius;
}

void XenoraPanel::setCornerRadius(int radius)
{
    m_cornerRadius = radius;
    update();
}

int XenoraPanel::cornerRadius() const
{
    return m_cornerRadius;
}

void XenoraPanel::setBorderWidth(int width)
{
    m_borderWidth = width;
    update();
}

int XenoraPanel::borderWidth() const
{
    return m_borderWidth;
}

void XenoraPanel::setBorderColor(const QColor &color)
{
    m_borderColor = color;
    update();
}

QColor XenoraPanel::borderColor() const
{
    return m_borderColor;
}

void XenoraPanel::setBlurEnabled(bool enabled)
{
    m_blurEnabled = enabled;
    update();
}

bool XenoraPanel::blurEnabled() const
{
    return m_blurEnabled;
}

void XenoraPanel::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event);
    
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);
    
    QRect panelRect = rect();
    
    // Create path for rounded rectangle
    QPainterPath path;
    path.addRoundedRect(panelRect, m_cornerRadius, m_cornerRadius);
    
    // Draw panel background
    painter.setPen(Qt::NoPen);
    painter.setBrush(m_backgroundColor);
    painter.drawPath(path);
    
    // Draw border if needed
    if (m_borderWidth > 0) {
        painter.setPen(QPen(m_borderColor, m_borderWidth));
        painter.setBrush(Qt::NoBrush);
        painter.drawPath(path);
    }
    
    // Note: Real blur would need platform-specific X11 or Wayland integration
    // Here we just draw a semi-transparent background to simulate blur
    
    QWidget::paintEvent(event);
}

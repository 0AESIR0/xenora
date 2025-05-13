#include "XenoraButton.h"
#include "../desktopenv/StyleManager.h"

#include <QPainter>
#include <QPainterPath>
#include <QMouseEvent>
#include <QGraphicsDropShadowEffect>
#include <QPropertyAnimation>
#include <QDebug>

XenoraButton::XenoraButton(QWidget *parent)
    : QPushButton(parent),
      m_elevation(0),
      m_style(Primary),
      m_shadowEffect(nullptr)
{
    initButton();
}

XenoraButton::XenoraButton(const QString &text, QWidget *parent)
    : QPushButton(text, parent),
      m_elevation(0),
      m_style(Primary),
      m_shadowEffect(nullptr)
{
    initButton();
}

void XenoraButton::setElevation(qreal elevation)
{
    if (m_elevation == elevation)
        return;
    
    m_elevation = elevation;
    
    if (m_shadowEffect) {
        m_shadowEffect->setBlurRadius(m_elevation * 4);
    }
    
    update();
}

void XenoraButton::setButtonStyle(ButtonStyle style)
{
    m_style = style;
    updateStyle();
}

void XenoraButton::setHoverColor(const QColor &color)
{
    m_hoverColor = color;
}

void XenoraButton::setIconSize(int size)
{
    QPushButton::setIconSize(QSize(size, size));
}

void XenoraButton::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event);
    
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);
    
    QRect buttonRect = rect();
    int cornerRadius = StyleManager::cornerRadius();
    
    // Choose button colors based on style
    QColor bgColor;
    QColor textColor;
    QColor borderColor;
    
    switch (m_style) {
        case Primary:
            bgColor = StyleManager::accentColor();
            textColor = QColor(255, 255, 255);
            borderColor = bgColor.darker(110);
            break;
        case Secondary:
            bgColor = StyleManager::currentTheme().foregroundColor;
            textColor = StyleManager::currentTheme().textColor;
            borderColor = StyleManager::currentTheme().highlightColor;
            break;
        case Flat:
            bgColor = Qt::transparent;
            textColor = StyleManager::currentTheme().textColor;
            borderColor = Qt::transparent;
            break;
        case Icon:
            bgColor = Qt::transparent;
            textColor = StyleManager::currentTheme().textColor;
            borderColor = Qt::transparent;
            break;
        case Panel:
            bgColor = QColor(0, 0, 0, 30); // Semi-transparent
            textColor = StyleManager::currentTheme().textColor;
            borderColor = Qt::transparent;
            break;
    }
    
    // Handle hover state
    if (underMouse() && isEnabled()) {
        if (!m_hoverColor.isValid()) {
            // Default hover behavior if no custom hover color
            if (m_style == Primary) {
                bgColor = bgColor.lighter(110);
            } else if (m_style == Secondary) {
                bgColor = bgColor.lighter(110);
            } else if (m_style == Panel || m_style == Icon) {
                bgColor = QColor(255, 255, 255, 30);
            }
        } else {
            bgColor = m_hoverColor;
        }
    }
    
    // Handle pressed state
    if (isDown()) {
        bgColor = bgColor.darker(110);
        setElevation(1);
    }
    
    // Draw button background
    painter.setPen(Qt::NoPen);
    painter.setBrush(bgColor);
    
    // Adjust corner radius for different styles
    if (m_style == Icon || m_style == Flat) {
        cornerRadius = height() / 2; // Circular for icon buttons
    }
    
    if (m_style != Flat) {
        if (cornerRadius > 0) {
            painter.drawRoundedRect(buttonRect, cornerRadius, cornerRadius);
        } else {
            painter.drawRect(buttonRect);
        }
    }
    
    // Draw text for text buttons
    if (!text().isEmpty()) {
        painter.setPen(textColor);
        painter.setBrush(Qt::NoBrush);
        painter.drawText(buttonRect, Qt::AlignCenter, text());
    }
    
    // Draw icon if present
    if (!icon().isNull()) {
        QIcon::Mode mode = isEnabled() ? QIcon::Normal : QIcon::Disabled;
        QIcon::State state = isChecked() ? QIcon::On : QIcon::Off;
        QSize iconSize = this->iconSize();
        QRect iconRect(QPoint((width() - iconSize.width()) / 2, 
                             (height() - iconSize.height()) / 2), 
                     iconSize);
        
        if (!text().isEmpty()) {
            // Adjust icon position if we have text
            iconRect.moveLeft(buttonRect.left() + cornerRadius);
        }
        
        painter.drawPixmap(iconRect, icon().pixmap(iconSize, mode, state));
    }
}

void XenoraButton::enterEvent(QEvent *event)
{
    m_elevationAnimation->setEndValue(2.0);
    m_elevationAnimation->start();
    setCursor(Qt::PointingHandCursor);
    
    QPushButton::enterEvent(event);
}

void XenoraButton::leaveEvent(QEvent *event)
{
    m_elevationAnimation->setEndValue(0.0);
    m_elevationAnimation->start();
    setCursor(Qt::ArrowCursor);
    
    QPushButton::leaveEvent(event);
}

void XenoraButton::mousePressEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton) {
        m_elevationAnimation->setEndValue(4.0);
        m_elevationAnimation->start();
    }
    
    QPushButton::mousePressEvent(event);
}

void XenoraButton::mouseReleaseEvent(QMouseEvent *event)
{
    m_elevationAnimation->setEndValue(2.0);
    m_elevationAnimation->start();
    
    QPushButton::mouseReleaseEvent(event);
}

void XenoraButton::initButton()
{
    setCursor(Qt::PointingHandCursor);
    setMouseTracking(true);
    
    // No focus frame
    setFocusPolicy(Qt::NoFocus);
    
    // Add shadow effect
    m_shadowEffect = new QGraphicsDropShadowEffect(this);
    m_shadowEffect->setBlurRadius(0);
    m_shadowEffect->setColor(QColor(0, 0, 0, 70));
    m_shadowEffect->setOffset(0, 0);
    setGraphicsEffect(m_shadowEffect);
    
    setupAnimation();
    updateStyle();
}

void XenoraButton::setupAnimation()
{
    m_elevationAnimation = new QPropertyAnimation(this, "elevation");
    m_elevationAnimation->setDuration(150);
    m_elevationAnimation->setEasingCurve(QEasingCurve::OutCubic);
}

void XenoraButton::updateStyle()
{
    // Set default properties based on button style
    switch (m_style) {
        case Primary:
            setElevation(2.0);
            break;
        case Secondary:
            setElevation(1.0);
            break;
        case Flat:
            setElevation(0.0);
            break;
        case Icon:
            setElevation(0.0);
            break;
        case Panel:
            setElevation(0.0);
            break;
    }
    
    update();
}

#include "XenoraWindow.h"
#include "XenoraButton.h"
#include "../desktopenv/StyleManager.h"

#include <QLabel>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QPainter>
#include <QMouseEvent>
#include <QApplication>
#include <QDesktopWidget>
#include <QGraphicsDropShadowEffect>
#include <QDebug>

XenoraWindow::XenoraWindow(QWidget *parent, Window xWindowId)
    : QWidget(parent),
      m_windowId(xWindowId),
      m_isMaximized(false),
      m_isDragging(false),
      m_resizeArea(None)
{
    setWindowFlags(Qt::FramelessWindowHint | Qt::WindowSystemMenuHint);
    setAttribute(Qt::WA_TranslucentBackground);
    
    // Create shadow effect
    QGraphicsDropShadowEffect *shadowEffect = new QGraphicsDropShadowEffect(this);
    shadowEffect->setBlurRadius(20);
    shadowEffect->setColor(QColor(0, 0, 0, 80));
    shadowEffect->setOffset(0, 0);
    setGraphicsEffect(shadowEffect);
    
    setupUi();
}

XenoraWindow::~XenoraWindow()
{
}

void XenoraWindow::setTitle(const QString &title)
{
    m_titleLabel->setText(title);
}

QString XenoraWindow::title() const
{
    return m_titleLabel->text();
}

void XenoraWindow::setMaximized(bool maximized)
{
    m_isMaximized = maximized;
    
    // Update maximize button icon
    if (m_isMaximized) {
        m_maximizeButton->setIcon(QIcon(":/icons/system/restore.svg"));
    } else {
        m_maximizeButton->setIcon(QIcon(":/icons/system/maximize.svg"));
    }
    
    // Disable shadow when maximized
    graphicsEffect()->setEnabled(!m_isMaximized);
}

void XenoraWindow::restorePreviousGeometry()
{
    if (!m_previousGeometry.isNull()) {
        setGeometry(m_previousGeometry);
    }
}

void XenoraWindow::saveCurrentGeometry()
{
    if (!m_isMaximized) {
        m_previousGeometry = geometry();
    }
}

void XenoraWindow::paintEvent(QPaintEvent *event)
{
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);
    
    // Create rounded rectangle for window
    QColor windowColor = StyleManager::currentTheme().windowColor;
    QRect windowRect = rect();
    
    // Adjust the rect when maximized to remove rounded corners
    int cornerRadius = m_isMaximized ? 0 : StyleManager::cornerRadius();
    
    painter.setPen(Qt::NoPen);
    painter.setBrush(windowColor);
    painter.drawRoundedRect(windowRect, cornerRadius, cornerRadius);
}

void XenoraWindow::mousePressEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton) {
        m_resizeArea = getResizeArea(event->pos());
        
        if (m_resizeArea != None) {
            // Resize mode
        } else if (m_titleBar->geometry().contains(event->pos())) {
            // Title bar drag mode
            m_isDragging = true;
            m_dragPosition = event->globalPos() - frameGeometry().topLeft();
        }
    }
    
    QWidget::mousePressEvent(event);
}

void XenoraWindow::mouseMoveEvent(QMouseEvent *event)
{
    if (m_isMaximized) {
        QWidget::mouseMoveEvent(event);
        return;
    }
    
    updateCursor(event->pos());
    
    if (event->buttons() & Qt::LeftButton) {
        if (m_isDragging) {
            // Move window
            move(event->globalPos() - m_dragPosition);
        } else if (m_resizeArea != None) {
            // Resize window
            QRect newGeom = geometry();
            QPoint delta = event->globalPos() - QPoint(newGeom.x(), newGeom.y()) - m_dragPosition;
            
            switch (m_resizeArea) {
                case Top:
                    newGeom.setTop(newGeom.top() + delta.y());
                    break;
                case Bottom:
                    newGeom.setBottom(event->globalPos().y());
                    break;
                case Left:
                    newGeom.setLeft(newGeom.left() + delta.x());
                    break;
                case Right:
                    newGeom.setRight(event->globalPos().x());
                    break;
                case TopLeft:
                    newGeom.setTopLeft(newGeom.topLeft() + QPoint(delta.x(), delta.y()));
                    break;
                case TopRight:
                    newGeom.setTopRight(QPoint(event->globalPos().x(), newGeom.top() + delta.y()));
                    break;
                case BottomLeft:
                    newGeom.setBottomLeft(QPoint(newGeom.left() + delta.x(), event->globalPos().y()));
                    break;
                case BottomRight:
                    newGeom.setBottomRight(event->globalPos());
                    break;
                default:
                    break;
            }
            
            // Enforce minimum size
            int minWidth = 200;
            int minHeight = 150;
            if (newGeom.width() >= minWidth && newGeom.height() >= minHeight) {
                setGeometry(newGeom);
                m_dragPosition = event->globalPos() - QPoint(newGeom.x(), newGeom.y());
            }
        }
    }
    
    QWidget::mouseMoveEvent(event);
}

void XenoraWindow::mouseReleaseEvent(QMouseEvent *event)
{
    m_isDragging = false;
    m_resizeArea = None;
    setCursor(Qt::ArrowCursor);
    
    QWidget::mouseReleaseEvent(event);
}

void XenoraWindow::resizeEvent(QResizeEvent *event)
{
    QWidget::resizeEvent(event);
}

void XenoraWindow::handleCloseClicked()
{
    // This will emit a signal that WindowManager will pick up
    emit close();
}

void XenoraWindow::handleMaximizeClicked()
{
    if (m_isMaximized) {
        // Restore
        setMaximized(false);
        restorePreviousGeometry();
    } else {
        // Maximize
        saveCurrentGeometry();
        setMaximized(true);
        setGeometry(QApplication::desktop()->availableGeometry(this));
    }
}

void XenoraWindow::handleMinimizeClicked()
{
    showMinimized();
}

void XenoraWindow::setupUi()
{
    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(0, 0, 0, 0);
    mainLayout->setSpacing(0);
    
    setupTitleBar();
    mainLayout->addWidget(m_titleBar);
    
    // Add window content placeholder
    QWidget *contentWidget = new QWidget(this);
    contentWidget->setObjectName("windowContent");
    contentWidget->setStyleSheet("background-color: transparent;");
    
    mainLayout->addWidget(contentWidget, 1);
    setLayout(mainLayout);
}

void XenoraWindow::setupTitleBar()
{
    m_titleBar = new QWidget(this);
    m_titleBar->setObjectName("titleBar");
    m_titleBar->setFixedHeight(36);
    
    QHBoxLayout *titleBarLayout = new QHBoxLayout(m_titleBar);
    titleBarLayout->setContentsMargins(12, 0, 12, 0);
    titleBarLayout->setSpacing(8);
    
    // Window title
    m_titleLabel = new QLabel(this);
    m_titleLabel->setObjectName("windowTitle");
    m_titleLabel->setText("XenoraOS Window");
    m_titleLabel->setAlignment(Qt::AlignCenter);
    
    // Window buttons
    setupWindowButtons();
    
    titleBarLayout->addWidget(m_titleLabel, 1);
    titleBarLayout->addWidget(m_minimizeButton);
    titleBarLayout->addWidget(m_maximizeButton);
    titleBarLayout->addWidget(m_closeButton);
    
    m_titleBar->setLayout(titleBarLayout);
    
    // Style the title bar
    m_titleBar->setStyleSheet(
        "QWidget#titleBar {"
        "  background-color: transparent;"
        "}"
        "QLabel#windowTitle {"
        "  color: " + StyleManager::currentTheme().textColor.name() + ";"
        "  font-size: 12px;"
        "}"
    );
}

void XenoraWindow::setupWindowButtons()
{
    // Minimize button
    m_minimizeButton = new XenoraButton(this);
    m_minimizeButton->setIcon(QIcon(":/icons/system/minimize.svg"));
    m_minimizeButton->setIconSize(QSize(16, 16));
    m_minimizeButton->setFixedSize(36, 36);
    m_minimizeButton->setButtonStyle(XenoraButton::Icon);
    
    // Maximize button
    m_maximizeButton = new XenoraButton(this);
    m_maximizeButton->setIcon(QIcon(":/icons/system/maximize.svg"));
    m_maximizeButton->setIconSize(QSize(16, 16));
    m_maximizeButton->setFixedSize(36, 36);
    m_maximizeButton->setButtonStyle(XenoraButton::Icon);
    
    // Close button
    m_closeButton = new XenoraButton(this);
    m_closeButton->setIcon(QIcon(":/icons/system/close.svg"));
    m_closeButton->setIconSize(QSize(16, 16));
    m_closeButton->setFixedSize(36, 36);
    m_closeButton->setButtonStyle(XenoraButton::Icon);
    m_closeButton->setHoverColor(QColor(232, 17, 35));
    
    // Connect signals
    connect(m_minimizeButton, &XenoraButton::clicked, this, &XenoraWindow::handleMinimizeClicked);
    connect(m_maximizeButton, &XenoraButton::clicked, this, &XenoraWindow::handleMaximizeClicked);
    connect(m_closeButton, &XenoraButton::clicked, this, &XenoraWindow::handleCloseClicked);
}

XenoraWindow::ResizeArea XenoraWindow::getResizeArea(const QPoint &pos)
{
    if (m_isMaximized) {
        return None;
    }
    
    const int borderWidth = 8;
    
    bool onLeft = pos.x() <= borderWidth;
    bool onRight = pos.x() >= width() - borderWidth;
    bool onTop = pos.y() <= borderWidth;
    bool onBottom = pos.y() >= height() - borderWidth;
    
    if (onTop && onLeft) return TopLeft;
    if (onTop && onRight) return TopRight;
    if (onBottom && onLeft) return BottomLeft;
    if (onBottom && onRight) return BottomRight;
    if (onTop) return Top;
    if (onBottom) return Bottom;
    if (onLeft) return Left;
    if (onRight) return Right;
    
    return None;
}

void XenoraWindow::updateCursor(const QPoint &pos)
{
    if (m_isDragging || m_isMaximized) {
        return;
    }
    
    m_resizeArea = getResizeArea(pos);
    
    switch (m_resizeArea) {
        case Top:
        case Bottom:
            setCursor(Qt::SizeVerCursor);
            break;
        case Left:
        case Right:
            setCursor(Qt::SizeHorCursor);
            break;
        case TopLeft:
        case BottomRight:
            setCursor(Qt::SizeFDiagCursor);
            break;
        case TopRight:
        case BottomLeft:
            setCursor(Qt::SizeBDiagCursor);
            break;
        default:
            setCursor(Qt::ArrowCursor);
            break;
    }
}

#include "XenoraWindow.h"
#include "XenoraButton.h"
#include "../desktopenv/StyleManager.h"

#include <QPainter>
#include <QPainterPath>
#include <QMouseEvent>
#include <QGraphicsDropShadowEffect>
#include <QApplication>
#include <QDesktopWidget>
#include <QScreen>
#include <QDebug>

XenoraWindow::XenoraWindow(QWidget *parent)
    : QWidget(parent),
      m_title("Xenora Window"),
      m_windowState(Normal),
      m_isDragging(false),
      m_resizeDirection(None),
      m_borderWidth(5)
{
    setWindowFlags(Qt::FramelessWindowHint);
    setAttribute(Qt::WA_TranslucentBackground);
    
    // Apply shadow effect
    QGraphicsDropShadowEffect *shadowEffect = new QGraphicsDropShadowEffect(this);
    shadowEffect->setBlurRadius(20);
    shadowEffect->setColor(QColor(0, 0, 0, 70));
    shadowEffect->setOffset(0, 0);
    setGraphicsEffect(shadowEffect);
    
    setupUi();
    setMinimumSize(200, 150);
}

XenoraWindow::XenoraWindow(const QString &title, QWidget *parent)
    : XenoraWindow(parent)
{
    setTitle(title);
}

XenoraWindow::~XenoraWindow()
{
}

void XenoraWindow::setContent(QWidget *content)
{
    if (!content) return;
    
    // Clear existing content
    QLayoutItem *child;
    while ((child = m_contentLayout->takeAt(0)) != nullptr) {
        if (child->widget()) {
            child->widget()->setParent(nullptr);
        }
        delete child;
    }
    
    // Add new content
    m_contentLayout->addWidget(content);
}

void XenoraWindow::setIcon(const QPixmap &icon)
{
    m_icon = icon;
    if (!m_icon.isNull()) {
        m_iconLabel->setPixmap(m_icon.scaled(16, 16, Qt::KeepAspectRatio, Qt::SmoothTransformation));
        m_iconLabel->setVisible(true);
    } else {
        m_iconLabel->setVisible(false);
    }
}

void XenoraWindow::setTitle(const QString &title)
{
    m_title = title;
    m_titleLabel->setText(title);
}

QString XenoraWindow::title() const
{
    return m_title;
}

XenoraWindow::WindowState XenoraWindow::windowState() const
{
    return m_windowState;
}

void XenoraWindow::minimize()
{
    if (m_windowState != Minimized) {
        m_windowState = Minimized;
        showMinimized();
        emit windowStateChanged(m_windowState);
    }
}

void XenoraWindow::maximize()
{
    if (m_windowState != Maximized) {
        m_normalGeometry = geometry();
        m_windowState = Maximized;
        
        QRect screenGeometry = QApplication::primaryScreen()->availableGeometry();
        setGeometry(screenGeometry);
        
        updateWindowButtons();
        emit windowStateChanged(m_windowState);
    }
}

void XenoraWindow::restore()
{
    if (m_windowState != Normal) {
        m_windowState = Normal;
        
        if (m_normalGeometry.isValid()) {
            setGeometry(m_normalGeometry);
        }
        
        updateWindowButtons();
        emit windowStateChanged(m_windowState);
    }
}

void XenoraWindow::toggleMaximize()
{
    if (m_windowState == Maximized) {
        restore();
    } else {
        maximize();
    }
}

void XenoraWindow::showFullScreen()
{
    if (m_windowState != FullScreen) {
        m_normalGeometry = geometry();
        m_windowState = FullScreen;
        QWidget::showFullScreen();
        emit windowStateChanged(m_windowState);
    }
}

void XenoraWindow::exitFullScreen()
{
    if (m_windowState == FullScreen) {
        m_windowState = Normal;
        QWidget::showNormal();
        if (m_normalGeometry.isValid()) {
            setGeometry(m_normalGeometry);
        }
        emit windowStateChanged(m_windowState);
    }
}

void XenoraWindow::closeWindow()
{
    emit windowClosed();
    close();
}

void XenoraWindow::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event);
    
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);
    
    QRect windowRect = rect();
    int cornerRadius = StyleManager::cornerRadius();
    
    // Draw window background
    QColor bgColor = StyleManager::currentTheme().windowColor;
    
    painter.setPen(Qt::NoPen);
    painter.setBrush(bgColor);
    painter.drawRoundedRect(windowRect, cornerRadius, cornerRadius);
    
    QWidget::paintEvent(event);
}

void XenoraWindow::mousePressEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton) {
        m_resizeDirection = getResizeDirection(event->pos());
        
        if (m_resizeDirection == None && m_titleBar->geometry().contains(event->pos())) {
            m_isDragging = true;
            m_dragPosition = event->globalPos() - frameGeometry().topLeft();
        }
    }
    
    QWidget::mousePressEvent(event);
}

void XenoraWindow::mouseReleaseEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton) {
        m_isDragging = false;
        m_resizeDirection = None;
        setCursor(Qt::ArrowCursor);
    }
    
    QWidget::mouseReleaseEvent(event);
}

void XenoraWindow::mouseMoveEvent(QMouseEvent *event)
{
    if (m_windowState == Maximized || m_windowState == FullScreen) {
        QWidget::mouseMoveEvent(event);
        return;
    }
    
    if (event->buttons() & Qt::LeftButton) {
        if (m_resizeDirection != None) {
            // Resize window
            QRect newGeometry = geometry();
            
            if (m_resizeDirection & Left) {
                int newX = event->globalX();
                int newWidth = geometry().right() - newX + 1;
                if (newWidth >= minimumWidth()) {
                    newGeometry.setLeft(newX);
                }
            }
            
            if (m_resizeDirection & Right) {
                int newWidth = event->globalX() - geometry().left() + 1;
                if (newWidth >= minimumWidth()) {
                    newGeometry.setRight(event->globalX());
                }
            }
            
            if (m_resizeDirection & Top) {
                int newY = event->globalY();
                int newHeight = geometry().bottom() - newY + 1;
                if (newHeight >= minimumHeight()) {
                    newGeometry.setTop(newY);
                }
            }
            
            if (m_resizeDirection & Bottom) {
                int newHeight = event->globalY() - geometry().top() + 1;
                if (newHeight >= minimumHeight()) {
                    newGeometry.setBottom(event->globalY());
                }
            }
            
            setGeometry(newGeometry);
        } else if (m_isDragging) {
            // Move window
            move(event->globalPos() - m_dragPosition);
        }
    } else {
        // Update cursor based on resize direction
        ResizeDirection direction = getResizeDirection(event->pos());
        updateCursor(direction);
    }
    
    QWidget::mouseMoveEvent(event);
}

void XenoraWindow::resizeEvent(QResizeEvent *event)
{
    QWidget::resizeEvent(event);
    
    // Ensure title bar buttons stay on the right
    updateWindowButtons();
}

bool XenoraWindow::eventFilter(QObject *watched, QEvent *event)
{
    if (watched == m_titleBar && event->type() == QEvent::MouseButtonDblClick) {
        QMouseEvent *mouseEvent = static_cast<QMouseEvent*>(event);
        if (mouseEvent->button() == Qt::LeftButton) {
            toggleMaximize();
            return true;
        }
    }
    
    return QWidget::eventFilter(watched, event);
}

void XenoraWindow::setupUi()
{
    m_mainLayout = new QVBoxLayout(this);
    m_mainLayout->setContentsMargins(m_borderWidth, m_borderWidth, m_borderWidth, m_borderWidth);
    m_mainLayout->setSpacing(0);
    
    createTitleBar();
    
    // Content container
    m_contentContainer = new QWidget(this);
    m_contentLayout = new QVBoxLayout(m_contentContainer);
    m_contentLayout->setContentsMargins(0, 0, 0, 0);
    
    m_mainLayout->addWidget(m_titleBar);
    m_mainLayout->addWidget(m_contentContainer, 1);
    
    setLayout(m_mainLayout);
}

void XenoraWindow::createTitleBar()
{
    m_titleBar = new QWidget(this);
    m_titleBar->setObjectName("windowTitleBar");
    m_titleBar->setFixedHeight(40);
    m_titleBar->installEventFilter(this);
    
    m_titleBarLayout = new QHBoxLayout(m_titleBar);
    m_titleBarLayout->setContentsMargins(10, 0, 10, 0);
    
    m_iconLabel = new QLabel(m_titleBar);
    m_iconLabel->setFixedSize(16, 16);
    m_iconLabel->setVisible(false);
    
    m_titleLabel = new QLabel(m_title, m_titleBar);
    m_titleLabel->setStyleSheet("font-size: 12px; font-weight: bold;");
    
    createWindowButtons();
    
    m_titleBarLayout->addWidget(m_iconLabel);
    m_titleBarLayout->addWidget(m_titleLabel, 1);
    m_titleBarLayout->addWidget(m_minimizeButton);
    m_titleBarLayout->addWidget(m_maximizeButton);
    m_titleBarLayout->addWidget(m_closeButton);
    
    m_titleBar->setLayout(m_titleBarLayout);
    
    // Style title bar
    QColor titleBarColor = StyleManager::currentTheme().foregroundColor;
    m_titleBar->setStyleSheet(
        "background-color: " + titleBarColor.name() + ";"
        "border-top-left-radius: " + QString::number(StyleManager::cornerRadius()) + "px;"
        "border-top-right-radius: " + QString::number(StyleManager::cornerRadius()) + "px;"
    );
}

void XenoraWindow::createWindowButtons()
{
    m_minimizeButton = new XenoraButton(m_titleBar);
    m_minimizeButton->setIcon(QIcon(":/icons/system/minimize.svg"));
    m_minimizeButton->setIconSize(QSize(12, 12));
    m_minimizeButton->setFixedSize(30, 30);
    m_minimizeButton->setButtonStyle(XenoraButton::Icon);
    
    m_maximizeButton = new XenoraButton(m_titleBar);
    m_maximizeButton->setIcon(QIcon(":/icons/system/maximize.svg"));
    m_maximizeButton->setIconSize(QSize(12, 12));
    m_maximizeButton->setFixedSize(30, 30);
    m_maximizeButton->setButtonStyle(XenoraButton::Icon);
    
    m_closeButton = new XenoraButton(m_titleBar);
    m_closeButton->setIcon(QIcon(":/icons/system/close.svg"));
    m_closeButton->setIconSize(QSize(12, 12));
    m_closeButton->setFixedSize(30, 30);
    m_closeButton->setButtonStyle(XenoraButton::Icon);
    m_closeButton->setHoverColor(QColor(232, 17, 35, 180));
    
    connect(m_minimizeButton, &XenoraButton::clicked, this, &XenoraWindow::minimize);
    connect(m_maximizeButton, &XenoraButton::clicked, this, &XenoraWindow::toggleMaximize);
    connect(m_closeButton, &XenoraButton::clicked, this, &XenoraWindow::closeWindow);
}

void XenoraWindow::updateWindowButtons()
{
    if (m_windowState == Maximized) {
        m_maximizeButton->setIcon(QIcon(":/icons/system/restore.svg"));
    } else {
        m_maximizeButton->setIcon(QIcon(":/icons/system/maximize.svg"));
    }
}

XenoraWindow::ResizeDirection XenoraWindow::getResizeDirection(const QPoint &pos)
{
    if (m_windowState == Maximized || m_windowState == FullScreen) {
        return None;
    }
    
    const int borderWidth = m_borderWidth;
    
    bool top = pos.y() <= borderWidth;
    bool bottom = pos.y() >= height() - borderWidth;
    bool left = pos.x() <= borderWidth;
    bool right = pos.x() >= width() - borderWidth;
    
    if (top && left) return TopLeft;
    if (top && right) return TopRight;
    if (bottom && left) return BottomLeft;
    if (bottom && right) return BottomRight;
    if (top) return Top;
    if (bottom) return Bottom;
    if (left) return Left;
    if (right) return Right;
    
    return None;
}

void XenoraWindow::updateCursor(ResizeDirection direction)
{
    switch (direction) {
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
        case None:
        default:
            setCursor(Qt::ArrowCursor);
            break;
    }
}

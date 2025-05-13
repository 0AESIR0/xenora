#include "XenoraPopup.h"
#include "XenoraButton.h"
#include "../desktopenv/StyleManager.h"

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QPainter>
#include <QMouseEvent>
#include <QApplication>
#include <QScreen>
#include <QGraphicsDropShadowEffect>
#include <QDebug>

XenoraPopup::XenoraPopup(QWidget *parent, const QString &title)
    : QWidget(parent),
      m_title(title),
      m_isDragging(false)
{
    setWindowFlags(Qt::FramelessWindowHint | Qt::Popup | Qt::NoDropShadowWindowHint);
    setAttribute(Qt::WA_TranslucentBackground);
    
    // Apply shadow effect
    QGraphicsDropShadowEffect *shadowEffect = new QGraphicsDropShadowEffect(this);
    shadowEffect->setBlurRadius(30);
    shadowEffect->setColor(QColor(0, 0, 0, 80));
    shadowEffect->setOffset(0, 0);
    setGraphicsEffect(shadowEffect);
    
    setupUi();
}

XenoraPopup::~XenoraPopup()
{
}

void XenoraPopup::setTitle(const QString &title)
{
    m_title = title;
    m_titleLabel->setText(title);
}

QString XenoraPopup::title() const
{
    return m_title;
}

void XenoraPopup::setContent(QWidget *content)
{
    if (!content) return;
    
    // Clear existing content
    QLayoutItem *child;
    while ((child = m_contentLayout->takeAt(0)) != nullptr) {
        delete child->widget();
        delete child;
    }
    
    // Add new content
    m_contentLayout->addWidget(content);
}

void XenoraPopup::setSize(int width, int height)
{
    resize(width, height);
}

void XenoraPopup::show()
{
    // Center on screen
    QRect screenGeometry = QApplication::primaryScreen()->availableGeometry();
    move(
        screenGeometry.center().x() - width() / 2,
        screenGeometry.center().y() - height() / 2
    );
    
    QWidget::show();
}

void XenoraPopup::close()
{
    QWidget::close();
}

void XenoraPopup::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event);
    
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);
    
    QRect popupRect = rect();
    int cornerRadius = StyleManager::cornerRadius();
    
    // Draw popup background
    QColor bgColor = StyleManager::currentTheme().foregroundColor;
    
    painter.setPen(Qt::NoPen);
    painter.setBrush(bgColor);
    painter.drawRoundedRect(popupRect, cornerRadius, cornerRadius);
    
    QWidget::paintEvent(event);
}

void XenoraPopup::mousePressEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton) {
        if (m_titleBar->geometry().contains(event->pos())) {
            m_isDragging = true;
            m_dragStartPosition = event->globalPos() - frameGeometry().topLeft();
            event->accept();
        }
    }
    
    QWidget::mousePressEvent(event);
}

void XenoraPopup::mouseMoveEvent(QMouseEvent *event)
{
    if (event->buttons() & Qt::LeftButton && m_isDragging) {
        move(event->globalPos() - m_dragStartPosition);
        event->accept();
    }
    
    QWidget::mouseMoveEvent(event);
}

void XenoraPopup::mouseReleaseEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton) {
        m_isDragging = false;
        event->accept();
    }
    
    QWidget::mouseReleaseEvent(event);
}

void XenoraPopup::setupUi()
{
    m_mainLayout = new QVBoxLayout(this);
    m_mainLayout->setContentsMargins(0, 0, 0, 0);
    m_mainLayout->setSpacing(0);
    
    // Title bar
    m_titleBar = new QWidget(this);
    m_titleBar->setObjectName("popupTitleBar");
    m_titleBar->setFixedHeight(40);
    
    QHBoxLayout *titleLayout = new QHBoxLayout(m_titleBar);
    titleLayout->setContentsMargins(12, 0, 12, 0);
    
    m_titleLabel = new QLabel(m_title, this);
    m_titleLabel->setObjectName("popupTitle");
    
    m_closeButton = new XenoraButton(this);
    m_closeButton->setIcon(QIcon(":/icons/system/close.svg"));
    m_closeButton->setIconSize(QSize(16, 16));
    m_closeButton->setFixedSize(30, 30);
    m_closeButton->setButtonStyle(XenoraButton::Icon);
    
    connect(m_closeButton, &XenoraButton::clicked, this, &XenoraPopup::close);
    
    titleLayout->addWidget(m_titleLabel, 1);
    titleLayout->addWidget(m_closeButton);
    
    m_titleBar->setLayout(titleLayout);
    
    // Content area
    m_contentArea = new QWidget(this);
    m_contentLayout = new QVBoxLayout(m_contentArea);
    m_contentLayout->setContentsMargins(16, 16, 16, 16);
    
    m_mainLayout->addWidget(m_titleBar);
    m_mainLayout->addWidget(m_contentArea, 1);
    
    setLayout(m_mainLayout);
    
    // Style popup
    m_titleLabel->setStyleSheet(
        "font-size: 14px;"
        "font-weight: bold;"
        "color: " + StyleManager::currentTheme().textColor.name() + ";"
    );
    
    m_titleBar->setStyleSheet(
        "background-color: " + StyleManager::currentTheme().highlightColor.name() + ";"
        "border-top-left-radius: " + QString::number(StyleManager::cornerRadius()) + "px;"
        "border-top-right-radius: " + QString::number(StyleManager::cornerRadius()) + "px;"
    );
    
    m_contentArea->setStyleSheet(
        "background-color: transparent;"
    );
}

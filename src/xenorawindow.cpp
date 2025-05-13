#include "xenorawindow.h"
#include <QMouseEvent>
#include <QPainter>
#include <QPainterPath>
#include <QApplication>
#include <QScreen>

XenoraWindow::XenoraWindow(QWidget *parent)
    : XenoraWindow("XenoraOS Window", parent)
{
}

XenoraWindow::XenoraWindow(const QString &title, QWidget *parent)
    : QWidget(parent, Qt::FramelessWindowHint),
      m_titleBar(nullptr),
      m_titleLabel(nullptr),
      m_closeButton(nullptr),
      m_minimizeButton(nullptr),
      m_maximizeButton(nullptr),
      m_contentWidget(nullptr),
      m_mainLayout(nullptr),
      m_isDragging(false),
      m_isResizable(true),
      m_isMaximized(false)
{
    setAttribute(Qt::WA_TranslucentBackground);
    setObjectName("XenoraWindow");
    
    initializeWindow();
    setTitle(title);
}

XenoraWindow::~XenoraWindow() {
}

void XenoraWindow::initializeWindow() {
    // Create main layout
    m_mainLayout = new QVBoxLayout(this);
    m_mainLayout->setContentsMargins(0, 0, 0, 0);
    m_mainLayout->setSpacing(0);
    
    // Create title bar
    createTitleBar();
    
    // Create content placeholder
    m_contentWidget = new QWidget(this);
    m_contentWidget->setObjectName("WindowContent");
    m_contentWidget->setLayout(new QVBoxLayout());
    m_contentWidget->layout()->setContentsMargins(10, 10, 10, 10);
    
    // Add widgets to main layout
    m_mainLayout->addWidget(m_titleBar);
    m_mainLayout->addWidget(m_contentWidget, 1);
    
    // Set layout for this window
    setLayout(m_mainLayout);
    
    // Set initial size
    resize(800, 600);
    
    // Set up window styling
    setupWindowStyle();
}

void XenoraWindow::createTitleBar() {
    // Create title bar widget
    m_titleBar = new QWidget(this);
    m_titleBar->setObjectName("TitleBar");
    m_titleBar->setFixedHeight(32);
    
    // Create layout for title bar
    QHBoxLayout *titleLayout = new QHBoxLayout(m_titleBar);
    titleLayout->setContentsMargins(10, 0, 10, 0);
    
    // Create window title
    m_titleLabel = new QLabel("", m_titleBar);
    m_titleLabel->setObjectName("WindowTitle");
    
    // Create window control buttons
    m_minimizeButton = new QPushButton(m_titleBar);
    m_minimizeButton->setObjectName("MinimizeButton");
    m_minimizeButton->setFixedSize(24, 24);
    m_minimizeButton->setToolTip("Minimize");
    
    m_maximizeButton = new QPushButton(m_titleBar);
    m_maximizeButton->setObjectName("MaximizeButton");
    m_maximizeButton->setFixedSize(24, 24);
    m_maximizeButton->setToolTip("Maximize");
    
    m_closeButton = new QPushButton(m_titleBar);
    m_closeButton->setObjectName("CloseButton");
    m_closeButton->setFixedSize(24, 24);
    m_closeButton->setToolTip("Close");
    
    // Add widgets to title layout
    titleLayout->addWidget(m_titleLabel, 1);
    titleLayout->addWidget(m_minimizeButton);
    titleLayout->addSpacing(6);
    titleLayout->addWidget(m_maximizeButton);
    titleLayout->addSpacing(6);
    titleLayout->addWidget(m_closeButton);
    
    // Connect signals
    connect(m_closeButton, &QPushButton::clicked, this, &XenoraWindow::onCloseClicked);
    connect(m_minimizeButton, &QPushButton::clicked, this, &XenoraWindow::onMinimizeClicked);
    connect(m_maximizeButton, &QPushButton::clicked, this, &XenoraWindow::onMaximizeClicked);
}

void XenoraWindow::setupWindowStyle() {
    setStyleSheet(
        "QLabel#WindowTitle {"
        "   color: white;"
        "   font-size: 12px;"
        "}"
    );
}

void XenoraWindow::setTitle(const QString &title) {
    m_titleLabel->setText(title);
    setWindowTitle(title);
}

void XenoraWindow::setContent(QWidget *content) {
    if (!content) return;
    
    // Clear current content
    QLayout *layout = m_contentWidget->layout();
    QLayoutItem *item;
    while ((item = layout->takeAt(0)) != nullptr) {
        if (item->widget()) {
            item->widget()->setParent(nullptr);
        }
        delete item;
    }
    
    // Add new content
    layout->addWidget(content);
}

void XenoraWindow::setResizable(bool resizable) {
    m_isResizable = resizable;
}

void XenoraWindow::mousePressEvent(QMouseEvent *event) {
    if (event->button() == Qt::LeftButton && m_titleBar->rect().contains(event->pos())) {
        m_isDragging = true;
        m_dragStartPosition = event->globalPosition().toPoint() - frameGeometry().topLeft();
    }
    QWidget::mousePressEvent(event);
}

void XenoraWindow::mouseMoveEvent(QMouseEvent *event) {
    if (m_isDragging && (event->buttons() & Qt::LeftButton)) {
        move(event->globalPosition().toPoint() - m_dragStartPosition);
    }
    QWidget::mouseMoveEvent(event);
}

void XenoraWindow::mouseReleaseEvent(QMouseEvent *event) {
    if (event->button() == Qt::LeftButton) {
        m_isDragging = false;
    }
    QWidget::mouseReleaseEvent(event);
}

void XenoraWindow::paintEvent(QPaintEvent *event) {
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);
    
    // Create rounded rectangle path
    QPainterPath path;
    path.addRoundedRect(rect(), 10, 10);
    
    // Set clip mask for rounded corners
    painter.setClipPath(path);
    
    // Draw background
    painter.fillRect(rect(), QColor(35, 35, 50, 240));
    
    // Draw border
    painter.setPen(QPen(QColor(60, 60, 80, 100), 1));
    painter.drawPath(path);
    
    QWidget::paintEvent(event);
}

void XenoraWindow::onCloseClicked() {
    close();
}

void XenoraWindow::onMinimizeClicked() {
    setWindowState(windowState() | Qt::WindowMinimized);
}

void XenoraWindow::onMaximizeClicked() {
    if (m_isMaximized) {
        setWindowState(windowState() & ~Qt::WindowMaximized);
        m_isMaximized = false;
    } else {
        setWindowState(windowState() | Qt::WindowMaximized);
        m_isMaximized = true;
    }
}

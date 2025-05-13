#include "panel/taskbar.h"

#include <QPainter>
#include <QPaintEvent>
#include <QDebug>

TaskbarButton::TaskbarButton(const QString &title, const QIcon &icon, QWidget *parent)
    : QPushButton(parent), m_active(false)
{
    setText(title);
    setIcon(icon);
    setIconSize(QSize(24, 24));
    setFixedHeight(40);
    setMinimumWidth(120);
    setCheckable(true);
    
    setStyleSheet("QPushButton { color: white; text-align: left; padding-left: 10px; background-color: rgba(70, 70, 90, 120); border-radius: 5px; }"
                  "QPushButton:hover { background-color: rgba(80, 80, 100, 160); }"
                  "QPushButton:checked { background-color: rgba(90, 90, 120, 200); }");
}

void TaskbarButton::setActive(bool active)
{
    m_active = active;
    setChecked(active);
    update();
}

void TaskbarButton::paintEvent(QPaintEvent *event)
{
    QPushButton::paintEvent(event);
    
    if (m_active) {
        QPainter painter(this);
        painter.setPen(Qt::NoPen);
        painter.setBrush(QColor(100, 150, 255));
        painter.drawRect(0, height() - 3, width(), 3);
    }
}

Taskbar::Taskbar(QWidget *parent)
    : QWidget(parent)
{
    m_layout = new QHBoxLayout(this);
    m_layout->setContentsMargins(0, 0, 0, 0);
    m_layout->setSpacing(5);
    
    setLayout(m_layout);
    
    // Demo uygulamalar ekle
    addApplication("Terminal", QIcon::fromTheme("terminal"));
    addApplication("Dosya Yöneticisi", QIcon::fromTheme("system-file-manager"));
    addApplication("Web Tarayıcı", QIcon::fromTheme("web-browser"));
}

Taskbar::~Taskbar()
{
}

void Taskbar::addApplication(const QString &title, const QIcon &icon)
{
    if (m_buttons.contains(title)) {
        removeApplication(title);
    }
    
    TaskbarButton *button = new TaskbarButton(title, icon, this);
    connect(button, &QPushButton::clicked, this, &Taskbar::onTaskButtonClicked);
    
    m_buttons[title] = button;
    m_layout->addWidget(button);
}

void Taskbar::removeApplication(const QString &title)
{
    if (m_buttons.contains(title)) {
        TaskbarButton *button = m_buttons[title];
        m_layout->removeWidget(button);
        m_buttons.remove(title);
        delete button;
    }
}

void Taskbar::onTaskButtonClicked()
{
    TaskbarButton *button = qobject_cast<TaskbarButton*>(sender());
    if (button) {
        bool wasActive = button->isChecked();
        
        // Tüm butonları pasif yap
        for (auto btn : m_buttons.values()) {
            btn->setActive(false);
        }
        
        // Tıklanan butonu aktif yap (toggle)
        button->setActive(wasActive);
    }
}

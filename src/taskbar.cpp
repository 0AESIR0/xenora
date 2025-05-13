#include "taskbar.h"
#include <QHBoxLayout>
#include <QPushButton>
#include <QTimer>
#include <QX11Info>
#include <xcb/xcb.h>

// TaskButton sınıfı (basitleştirilmiş)
class TaskButton : public QPushButton
{
public:
    TaskButton(xcb_window_t window, const QString &title, const QIcon &icon, QWidget *parent = nullptr)
        : QPushButton(parent), m_window(window)
    {
        setText(title);
        setIcon(icon);
        setCheckable(true);
        setFlat(true);
    }
    
    xcb_window_t window() const { return m_window; }

private:
    xcb_window_t m_window;
};

Taskbar::Taskbar(QWidget *parent)
    : QWidget(parent)
{
    setupTaskbar();
    
    // Düzenli aralıklarla görev listesini güncelle
    QTimer *timer = new QTimer(this);
    connect(timer, &QTimer::timeout, this, &Taskbar::updateTasks);
    timer->start(1000);
    
    // İlk çağrı
    updateTasks();
}

Taskbar::~Taskbar()
{
}

void Taskbar::setupTaskbar()
{
    QHBoxLayout *layout = new QHBoxLayout(this);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->setSpacing(3);
    
    setLayout(layout);
}

void Taskbar::updateTasks()
{
    // Mevcut pencereleri al
    QVector<xcb_window_t> windows = getWindowList();
    
    // Artık mevcut olmayan görevleri kaldır
    QMutableMapIterator<xcb_window_t, TaskButton*> it(m_taskButtons);
    while (it.hasNext()) {
        it.next();
        if (!windows.contains(it.key())) {
            delete it.value();
            it.remove();
        }
    }
    
    // Yeni pencereleri ekle
    for (xcb_window_t window : windows) {
        if (!m_taskButtons.contains(window)) {
            QString title = getWindowTitle(window);
            QIcon icon = getWindowIcon(window);
            
            TaskButton *button = new TaskButton(window, title, icon, this);
            connect(button, &QPushButton::clicked, [this, window]() {
                activateWindow(window);
            });
            
            layout()->addWidget(button);
            m_taskButtons[window] = button;
        }
    }
}

QVector<xcb_window_t> Taskbar::getWindowList()
{
    // NOT: Gerçek uygulamada EWMH/NetWM kullanılarak
    // pencere listesi alınacaktır. Bu sadece iskelet kodudur.
    QVector<xcb_window_t> windows;
    
    // X11 pencere listesini almak için gerçek kod burada olacak
    
    return windows;
}

QString Taskbar::getWindowTitle(xcb_window_t window)
{
    // NOT: Gerçek uygulamada X11 özelliklerinden başlık alınacaktır.
    // Bu sadece iskelet kodudur.
    return QString("Pencere %1").arg(window);
}

QIcon Taskbar::getWindowIcon(xcb_window_t window)
{
    // NOT: Gerçek uygulamada X11 özelliklerinden simge alınacaktır.
    // Bu sadece iskelet kodudur.
    Q_UNUSED(window);
    return QIcon::fromTheme("application-x-executable");
}

void Taskbar::activateWindow(xcb_window_t window)
{
    // NOT: Burada XCB kullanılarak pencere aktif hale getirilecektir.
    // Bu sadece iskelet kodudur.
    
    // X11 penceresini aktif hale getirmek için gerçek kod burada olacak
}

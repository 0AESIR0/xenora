#include "launcher.h"
#include <QHBoxLayout>
#include <QProcess>
#include <QToolTip>
#include <QMenu>
#include <QAction>
#include <QFileInfo>
#include <QStandardPaths>
#include <QDir>

AppButton::AppButton(const QString &appName, const QString &command, const QIcon &icon, QWidget *parent)
    : QPushButton(parent)
    , m_appName(appName)
    , m_command(command)
{
    setIcon(icon);
    setIconSize(QSize(32, 32));
    setToolTip(appName);
    setFixedSize(40, 40);
    setFlat(true);
    
    // Uygulama çalıştırma bağlantısı
    connect(this, &AppButton::clicked, this, &AppButton::launchApp);
    
    // Sağ tık menüsü için bağlam menüsü
    setContextMenuPolicy(Qt::CustomContextMenu);
    connect(this, &AppButton::customContextMenuRequested, [this](const QPoint &pos) {
        QMenu menu;
        menu.addAction(QIcon::fromTheme("dialog-information"), "Bilgi: " + m_appName)->setEnabled(false);
        menu.addSeparator();
        
        QAction *removeAction = menu.addAction(QIcon::fromTheme("list-remove"), "Favorilerden Kaldır");
        connect(removeAction, &QAction::triggered, [this]() {
            emit customContextMenuRequested(QPoint(-1, -1)); // Özel sinyal
        });
        
        menu.exec(mapToGlobal(pos));
    });
}

void AppButton::launchApp()
{
    // Yöntem güncellendi: QStringList kullan
    QProcess::startDetached(m_command, QStringList());
}

/////////////////////////////////////////////

Launcher::Launcher(QWidget *parent)
    : QWidget(parent)
    , m_settings("XenoraOS", "launcher")
{
    QHBoxLayout *layout = new QHBoxLayout(this);
    layout->setContentsMargins(2, 2, 2, 2);
    layout->setSpacing(5);
    
    setLayout(layout);
    
    // Varsayılan favori uygulamaları yükle
    loadFavoriteApps();
}

Launcher::~Launcher()
{
    saveFavoriteApps();
}

void Launcher::addApplication(const QString &appName, const QString &command, const QIcon &icon)
{
    // Uygulama zaten varsa ekleme
    for (AppButton *btn : m_appButtons) {
        if (btn->appName() == appName) {
            return;
        }
    }
    
    AppButton *appButton = new AppButton(appName, command, icon, this);
    layout()->addWidget(appButton);
    m_appButtons.append(appButton);
    
    // Kaldırma işlemi için özel bağlam menüsü sinyali
    connect(appButton, &AppButton::customContextMenuRequested, [this, appButton](const QPoint &pos) {
        if (pos == QPoint(-1, -1)) { // Özel sinyal için
            removeApplication(appButton->appName());
        }
    });
    
    saveFavoriteApps();
}

void Launcher::removeApplication(const QString &appName)
{
    for (int i = 0; i < m_appButtons.size(); ++i) {
        AppButton *btn = m_appButtons.at(i);
        if (btn->appName() == appName) {
            layout()->removeWidget(btn);
            m_appButtons.removeAt(i);
            btn->deleteLater();
            break;
        }
    }
    
    saveFavoriteApps();
}

void Launcher::loadFavoriteApps()
{
    // Favori uygulamaları ayarlardan yükle
    int size = m_settings.beginReadArray("favoriteApps");
    
    // Eğer daha önce kaydedilmiş uygulama yoksa, varsayılanları ekle
    if (size == 0) {
        // Dosya Yöneticisi
        addApplication("Dosya Yöneticisi", "nautilus", QIcon::fromTheme("system-file-manager"));
        
        // Terminal
        addApplication("Terminal", "gnome-terminal", QIcon::fromTheme("utilities-terminal"));
        
        // Web Tarayıcı
        addApplication("Web Tarayıcı", "firefox", QIcon::fromTheme("web-browser"));
        
        // Metin Düzenleyici
        addApplication("Metin Düzenleyici", "gedit", QIcon::fromTheme("accessories-text-editor"));
    } else {
        for (int i = 0; i < size; ++i) {
            m_settings.setArrayIndex(i);
            QString appName = m_settings.value("name").toString();
            QString command = m_settings.value("command").toString();
            QString iconName = m_settings.value("icon").toString();
            
            QIcon icon = QIcon::fromTheme(iconName);
            addApplication(appName, command, icon);
        }
    }
    
    m_settings.endArray();
}

void Launcher::saveFavoriteApps()
{
    m_settings.beginWriteArray("favoriteApps");
    
    for (int i = 0; i < m_appButtons.size(); ++i) {
        AppButton *btn = m_appButtons.at(i);
        m_settings.setArrayIndex(i);
        m_settings.setValue("name", btn->appName());
        m_settings.setValue("command", btn->command());
        m_settings.setValue("icon", btn->icon().name());
    }
    
    m_settings.endArray();
}

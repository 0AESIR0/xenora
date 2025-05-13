#include "desktopicon.h"
#include <QVBoxLayout>
#include <QPainter>
#include <QFileIconProvider>
#include <QMenu>
#include <QAction>
#include <QMessageBox>
#include <QProcess>
#include <QMimeData>
#include <QDrag>
#include <QApplication>
#include <QDesktopServices>
#include <QUrl>
#include <QGraphicsDropShadowEffect>
#include <QFileDialog>
#include <QInputDialog>
#include <QSettings>
#include <QDir>

DesktopIcon::DesktopIcon(const QFileInfo &fileInfo, QWidget *parent)
    : QWidget(parent)
    , m_fileInfo(fileInfo)
    , m_hover(false)
    , m_dragging(false)
{
    setFixedSize(80, 90);
    setAttribute(Qt::WA_TranslucentBackground);
    setContextMenuPolicy(Qt::CustomContextMenu);
    
    connect(this, &QWidget::customContextMenuRequested, 
            [this](const QPoint &pos) { createContextMenu(pos); });
    
    setupIcon();
}

DesktopIcon::~DesktopIcon()
{
}

void DesktopIcon::setupIcon()
{
    QVBoxLayout *layout = new QVBoxLayout(this);
    layout->setContentsMargins(2, 2, 2, 2);
    layout->setSpacing(2);
    
    // Simge
    m_iconLabel = new QLabel(this);
    m_iconLabel->setAlignment(Qt::AlignCenter);
    
    // Simgeyi al
    QFileIconProvider iconProvider;
    QIcon icon = iconProvider.icon(m_fileInfo);
    
    if (icon.isNull()) {
        // Varsayılan simgeler
        if (m_fileInfo.isDir()) {
            icon = QIcon::fromTheme("folder");
        } else {
            QString suffix = m_fileInfo.suffix().toLower();
            
            if (suffix == "txt" || suffix == "md") {
                icon = QIcon::fromTheme("text-x-generic");
            } else if (suffix == "png" || suffix == "jpg" || suffix == "jpeg" || suffix == "gif") {
                icon = QIcon::fromTheme("image-x-generic");
            } else if (suffix == "mp3" || suffix == "wav" || suffix == "ogg") {
                icon = QIcon::fromTheme("audio-x-generic");
            } else if (suffix == "mp4" || suffix == "avi" || suffix == "mkv") {
                icon = QIcon::fromTheme("video-x-generic");
            } else if (suffix == "pdf") {
                icon = QIcon::fromTheme("application-pdf");
            } else if (suffix == "zip" || suffix == "tar" || suffix == "gz" || suffix == "xz") {
                icon = QIcon::fromTheme("package-x-generic");
            } else if (suffix == "desktop") {
                icon = QIcon::fromTheme("application-x-executable");
            } else {
                icon = QIcon::fromTheme("text-x-generic");
            }
        }
    }
    
    QPixmap pixmap = icon.pixmap(48, 48);
    m_iconLabel->setPixmap(pixmap);
    m_iconLabel->setFixedSize(48, 48);
    
    // Metin
    m_textLabel = new QLabel(m_fileInfo.fileName(), this);
    m_textLabel->setAlignment(Qt::AlignCenter);
    m_textLabel->setWordWrap(true);
    m_textLabel->setFixedWidth(76);
    
    // Metin metnini kısaltma
    QFontMetrics fm(m_textLabel->font());
    QString elidedText = fm.elidedText(m_fileInfo.fileName(), Qt::ElideMiddle, 76);
    m_textLabel->setText(elidedText);
    
    // Düzene ekle
    layout->addWidget(m_iconLabel, 0, Qt::AlignCenter);
    layout->addWidget(m_textLabel, 0, Qt::AlignCenter);
    
    setLayout(layout);
}

void DesktopIcon::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event);
    
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);
    
    if (m_hover) {
        // Üzerine gelindiğinde arka planı çiz
        QColor hoverColor(255, 255, 255, 40);
        painter.setPen(Qt::NoPen);
        painter.setBrush(hoverColor);
        painter.drawRoundedRect(rect(), 8, 8);
        
        // Metin için kontrastlı renk
        m_textLabel->setStyleSheet("QLabel { color: white; background-color: rgba(0, 0, 0, 100); border-radius: 4px; }");
    } else {
        // Normal durum: Şeffaf arka plan ve gölgeli metin
        m_textLabel->setStyleSheet("QLabel { color: white; background-color: rgba(0, 0, 0, 70); border-radius: 4px; }");
    }
}

void DesktopIcon::mousePressEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton) {
        m_dragStartPosition = event->pos();
    }
    QWidget::mousePressEvent(event);
}

void DesktopIcon::mouseMoveEvent(QMouseEvent *event)
{
    if (!(event->buttons() & Qt::LeftButton)) {
        return;
    }
    
    if ((event->pos() - m_dragStartPosition).manhattanLength() < QApplication::startDragDistance()) {
        return;
    }
    
    m_dragging = true;
    
    // Sürükleme işlemini başlat
    QDrag *drag = new QDrag(this);
    QMimeData *mimeData = new QMimeData;
    
    // URL olarak dosya/klasör yolunu ekle
    QList<QUrl> urls;
    urls << QUrl::fromLocalFile(m_fileInfo.absoluteFilePath());
    mimeData->setUrls(urls);
    
    drag->setMimeData(mimeData);
    
    // Sürükleme sırasındaki görüntüyü ayarla
    QPixmap pixmap = grab();
    drag->setPixmap(pixmap);
    drag->setHotSpot(event->pos());
    
    drag->exec(Qt::CopyAction | Qt::MoveAction | Qt::LinkAction);
    
    m_dragging = false;
    update();
}

void DesktopIcon::mouseReleaseEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton && !m_dragging) {
        // Sadece tıklama (sürükleme olmadan)
        update();
    }
    QWidget::mouseReleaseEvent(event);
}

void DesktopIcon::mouseDoubleClickEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton) {
        launchFile();
    }
    QWidget::mouseDoubleClickEvent(event);
}

void DesktopIcon::enterEvent(QEvent *event)
{
    m_hover = true;
    update();
    QWidget::enterEvent(event);
}

void DesktopIcon::leaveEvent(QEvent *event)
{
    m_hover = false;
    update();
    QWidget::leaveEvent(event);
}

void DesktopIcon::createContextMenu(const QPoint &pos)
{
    QMenu menu(this);
    
    menu.addAction(QIcon::fromTheme("document-open"), "Aç", this, &DesktopIcon::launchFile);
    
    if (!m_fileInfo.isDir()) {
        menu.addAction("İle Aç...", [this]() {
            QString program = QFileDialog::getOpenFileName(this, "Uygulama Seç", "/usr/bin");
            if (!program.isEmpty()) {
                QProcess::startDetached(program, QStringList() << m_fileInfo.absoluteFilePath());
            }
        });
    }
    
    menu.addSeparator();
    
    menu.addAction(QIcon::fromTheme("edit-cut"), "Kes");
    menu.addAction(QIcon::fromTheme("edit-copy"), "Kopyala");
    
    menu.addSeparator();
    
    menu.addAction(QIcon::fromTheme("edit-delete"), "Sil", [this]() {
        if (QMessageBox::question(this, "Sil", 
                                 QString("\"%1\" öğesini silmek istediğinize emin misiniz?").arg(m_fileInfo.fileName())) == QMessageBox::Yes) {
            
            if (m_fileInfo.isDir()) {
                QDir dir(m_fileInfo.absoluteFilePath());
                dir.removeRecursively();
            } else {
                QFile file(m_fileInfo.absoluteFilePath());
                file.remove();
            }
            
            // Simgeyi kaldır
            deleteLater();
        }
    });
    
    menu.addAction(QIcon::fromTheme("edit-rename"), "Yeniden Adlandır", [this]() {
        QString newName = QInputDialog::getText(this, "Yeniden Adlandır", 
                                              "Yeni ad:", QLineEdit::Normal, m_fileInfo.fileName());
        if (!newName.isEmpty() && newName != m_fileInfo.fileName()) {
            QString newPath = m_fileInfo.absolutePath() + "/" + newName;
            QFile file(m_fileInfo.absoluteFilePath());
            if (file.rename(newPath)) {
                m_fileInfo = QFileInfo(newPath);
                m_textLabel->setText(m_fileInfo.fileName());
            } else {
                QMessageBox::warning(this, "Hata", "Yeniden adlandırma başarısız oldu.");
            }
        }
    });
    
    menu.addSeparator();
    
    menu.addAction(QIcon::fromTheme("document-properties"), "Özellikler", [this]() {
        // Dosya özellikleri penceresi
        QMessageBox::information(this, "Özellikler", 
                               QString("Ad: %1\nTür: %2\nBoyut: %3 bayt\nKonum: %4")
                               .arg(m_fileInfo.fileName())
                               .arg(m_fileInfo.isDir() ? "Klasör" : m_fileInfo.suffix() + " dosyası")
                               .arg(m_fileInfo.size())
                               .arg(m_fileInfo.absolutePath()));
    });
    
    menu.exec(mapToGlobal(pos));
}

void DesktopIcon::launchFile()
{
    if (m_fileInfo.suffix().toLower() == "desktop") {
        // .desktop dosyalarını özel olarak işle
        QSettings desktopFile(m_fileInfo.absoluteFilePath(), QSettings::IniFormat);
        desktopFile.beginGroup("Desktop Entry");
        
        QString exec = desktopFile.value("Exec").toString();
        if (!exec.isEmpty()) {
            // Exec komutundan %f, %F, %u, %U gibi argümanları temizle
            exec = exec.split(" ", Qt::SkipEmptyParts).first();
            // Güncellenmiş metod kullanımı
            QProcess::startDetached(exec, QStringList());
        }
        
        desktopFile.endGroup();
    } else if (m_fileInfo.isDir()) {
        // Klasör için dosya yöneticisini aç
        QProcess::startDetached("xdg-open", QStringList() << m_fileInfo.absoluteFilePath());
    } else {
        // Normal dosyayı varsayılan uygulamayla aç
        QDesktopServices::openUrl(QUrl::fromLocalFile(m_fileInfo.absoluteFilePath()));
    }
}
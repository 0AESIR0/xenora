#include "filemanager.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QMenuBar>
#include <QMenu>
#include <QMessageBox>
#include <QInputDialog>
#include <QDir>
#include <QFile>
#include <QFileInfo>
#include <QProcess>
#include <QUrl>
#include <QDesktopServices>
#include <QClipboard>
#include <QApplication>
#include <QMimeData>
#include <QShortcut>
#include <QStandardPaths>

FileManager::FileManager(QWidget *parent)
    : QMainWindow(parent)
    , m_historyIndex(-1)
    , m_isNavigating(false)
{
    setWindowTitle("XenoraOS Dosya Yöneticisi");
    setWindowIcon(QIcon::fromTheme("system-file-manager"));
    resize(900, 600);
    
    setupUI();
    setupActions();
    setupToolbar();
    setupMenus();
    
    // Varsayılan olarak ev dizinini göster
    openLocation(QDir::homePath());
}

FileManager::~FileManager()
{
}

void FileManager::setupUI()
{
    QWidget *centralWidget = new QWidget(this);
    setCentralWidget(centralWidget);
    
    // Ana düzen
    QVBoxLayout *mainLayout = new QVBoxLayout(centralWidget);
    mainLayout->setContentsMargins(0, 0, 0, 0);
    mainLayout->setSpacing(0);
    
    // Adres çubuğu
    QHBoxLayout *addressLayout = new QHBoxLayout();
    addressLayout->setContentsMargins(5, 5, 5, 5);
    
    m_pathBox = new QComboBox(this);
    m_pathBox->setEditable(true);
    m_pathBox->setMinimumWidth(200);
    m_pathEdit = m_pathBox->lineEdit();
    
    addressLayout->addWidget(m_pathBox);
    
    // Yan panel ve dosya listesi için splitter
    m_splitter = new QSplitter(Qt::Horizontal, this);
    
    // Yan panel (ağaç görünümü)
    m_sidebarView = new QTreeView(this);
    m_sidebarView->setMinimumWidth(180);
    m_sidebarView->setMaximumWidth(300);
    
    // Dosya listesi
    m_fileView = new QListView(this);
    m_fileView->setViewMode(QListView::IconMode);
    m_fileView->setIconSize(QSize(48, 48));
    m_fileView->setGridSize(QSize(120, 100));
    m_fileView->setResizeMode(QListView::Adjust);
    m_fileView->setWrapping(true);
    m_fileView->setWordWrap(true);
    m_fileView->setSelectionMode(QAbstractItemView::ExtendedSelection);
    m_fileView->setEditTriggers(QAbstractItemView::EditKeyPressed | QAbstractItemView::SelectedClicked);
    m_fileView->setContextMenuPolicy(Qt::CustomContextMenu);
    
    // Model oluştur ve ayarla
    m_fileModel = new QFileSystemModel(this);
    m_fileModel->setFilter(QDir::AllEntries | QDir::NoDotAndDotDot);
    m_fileModel->setNameFilterDisables(false);
    
    m_fileView->setModel(m_fileModel);
    m_sidebarView->setModel(m_fileModel);
    
    // Sadece klasörleri göster (yan panel için)
    m_sidebarView->setRootIsDecorated(true);
    m_sidebarView->setHeaderHidden(true);
    m_sidebarView->hideColumn(1); // Boyut
    m_sidebarView->hideColumn(2); // Tür
    m_sidebarView->hideColumn(3); // Değiştirilme tarihi
    
    // Splitter'a ekle
    m_splitter->addWidget(m_sidebarView);
    m_splitter->addWidget(m_fileView);
    m_splitter->setStretchFactor(0, 0);
    m_splitter->setStretchFactor(1, 1);
    
    // Durum çubuğu
    m_statusBar = new QStatusBar(this);
    setStatusBar(m_statusBar);
    
    // Düzene ekle
    mainLayout->addLayout(addressLayout);
    mainLayout->addWidget(m_splitter);
    
    // Sinyaller ve slotlar
    connect(m_pathEdit, &QLineEdit::returnPressed, this, &FileManager::onPathEdited);
    connect(m_fileView, &QListView::activated, this, &FileManager::onFileActivated);
    connect(m_sidebarView, &QTreeView::clicked, this, &FileManager::onLocationChanged);
    connect(m_fileView, &QListView::customContextMenuRequested, this, &FileManager::handleContextMenu);
}

void FileManager::setupActions()
{
    m_backAction = new QAction(QIcon::fromTheme("go-previous"), "Geri", this);
    m_backAction->setShortcut(QKeySequence::Back);
    m_backAction->setEnabled(false);
    connect(m_backAction, &QAction::triggered, this, &FileManager::navigateBack);
    
    m_forwardAction = new QAction(QIcon::fromTheme("go-next"), "İleri", this);
    m_forwardAction->setShortcut(QKeySequence::Forward);
    m_forwardAction->setEnabled(false);
    connect(m_forwardAction, &QAction::triggered, this, &FileManager::navigateForward);
    
    m_upAction = new QAction(QIcon::fromTheme("go-up"), "Yukarı", this);
    m_upAction->setShortcut(QKeySequence(Qt::Key_Backspace));
    connect(m_upAction, &QAction::triggered, this, &FileManager::navigateUp);
    
    m_homeAction = new QAction(QIcon::fromTheme("go-home"), "Ev Dizini", this);
    m_homeAction->setShortcut(QKeySequence(Qt::ALT + Qt::Key_Home));
    connect(m_homeAction, &QAction::triggered, this, &FileManager::navigateHome);
    
    m_refreshAction = new QAction(QIcon::fromTheme("view-refresh"), "Yenile", this);
    m_refreshAction->setShortcut(QKeySequence::Refresh);
    connect(m_refreshAction, &QAction::triggered, this, &FileManager::refresh);
    
    m_newFolderAction = new QAction(QIcon::fromTheme("folder-new"), "Yeni Klasör", this);
    m_newFolderAction->setShortcut(QKeySequence(Qt::CTRL + Qt::SHIFT + Qt::Key_N));
    connect(m_newFolderAction, &QAction::triggered, this, &FileManager::createNewFolder);
}

void FileManager::setupToolbar()
{
    m_toolbar = addToolBar("Gezinme");
    m_toolbar->setIconSize(QSize(24, 24));
    m_toolbar->setMovable(false);
    
    m_toolbar->addAction(m_backAction);
    m_toolbar->addAction(m_forwardAction);
    m_toolbar->addAction(m_upAction);
    m_toolbar->addAction(m_homeAction);
    m_toolbar->addAction(m_refreshAction);
    m_toolbar->addSeparator();
    m_toolbar->addAction(m_newFolderAction);
}

void FileManager::setupMenus()
{
    // Dosya menüsü
    QMenu *fileMenu = menuBar()->addMenu("Dosya");
    fileMenu->addAction(QIcon::fromTheme("document-new"), "Yeni Pencere", this, [this]() {
        FileManager *fm = new FileManager();
        fm->show();
    });
    fileMenu->addSeparator();
    fileMenu->addAction(m_newFolderAction);
    fileMenu->addSeparator();
    fileMenu->addAction(QIcon::fromTheme("window-close"), "Kapat", this, &QMainWindow::close, QKeySequence::Close);
    
    // Düzen menüsü
    QMenu *editMenu = menuBar()->addMenu("Düzen");
    editMenu->addAction(QIcon::fromTheme("edit-cut"), "Kes", this, [this]() {
        // Kesme işlemi kodu
    }, QKeySequence::Cut);
    editMenu->addAction(QIcon::fromTheme("edit-copy"), "Kopyala", this, [this]() {
        // Kopyalama işlemi kodu
    }, QKeySequence::Copy);
    editMenu->addAction(QIcon::fromTheme("edit-paste"), "Yapıştır", this, [this]() {
        // Yapıştırma işlemi kodu
    }, QKeySequence::Paste);
    
    // Görünüm menüsü
    QMenu *viewMenu = menuBar()->addMenu("Görünüm");
    viewMenu->addAction(QIcon::fromTheme("view-refresh"), "Yenile", this, &FileManager::refresh, QKeySequence::Refresh);
    
    QMenu *viewModeMenu = viewMenu->addMenu("Görünüm Modu");
    viewModeMenu->addAction("Simgeler", this, [this]() {
        m_fileView->setViewMode(QListView::IconMode);
        m_fileView->setIconSize(QSize(48, 48));
        m_fileView->setGridSize(QSize(120, 100));
    });
    viewModeMenu->addAction("Liste", this, [this]() {
        m_fileView->setViewMode(QListView::ListMode);
        m_fileView->setIconSize(QSize(16, 16));
        m_fileView->setGridSize(QSize());
    });
    
    // Yardım menüsü
    QMenu *helpMenu = menuBar()->addMenu("Yardım");
    helpMenu->addAction(QIcon::fromTheme("help-about"), "Hakkında", this, [this]() {
        QMessageBox::about(this, "XenoraOS Dosya Yöneticisi Hakkında",
                          "XenoraOS Dosya Yöneticisi\n"
                          "Sürüm 1.0\n\n"
                          "XenoraOS masaüstü ortamı için modern bir dosya yöneticisi.");
    });
}

void FileManager::openLocation(const QString &path)
{
    QModelIndex index = m_fileModel->setRootPath(path);
    m_fileView->setRootIndex(index);
    
    // Yan panelde seçili dizini göster
    QModelIndex sidebarIndex = m_fileModel->index(path);
    m_sidebarView->setCurrentIndex(sidebarIndex);
    m_sidebarView->scrollTo(sidebarIndex);
    
    // Adres çubuğunu güncelle
    m_pathEdit->setText(path);
    
    // Geçmişi güncelle
    if (!m_isNavigating) {
        // Geçmiş listesini temizle (ileri geçmiş)
        if (m_historyIndex < m_history.size() - 1 && m_historyIndex >= 0) {
            m_history = m_history.mid(0, m_historyIndex + 1);
        }
        
        m_history.append(path);
        m_historyIndex = m_history.size() - 1;
        
        m_backAction->setEnabled(m_historyIndex > 0);
        m_forwardAction->setEnabled(false);
    }
    
    // Durum çubuğunu güncelle
    QDir dir(path);
    int fileCount = dir.entryList(QDir::Files).count();
    int folderCount = dir.entryList(QDir::Dirs | QDir::NoDotAndDotDot).count();
    
    m_statusBar->showMessage(QString("%1 öğe (%2 dosya, %3 klasör)").arg(fileCount + folderCount).arg(fileCount).arg(folderCount));
    
    updatePathBox();
}

void FileManager::navigateUp()
{
    QDir dir(m_fileModel->rootPath());
    if (dir.cdUp()) {
        openLocation(dir.absolutePath());
    }
}

void FileManager::navigateBack()
{
    if (m_historyIndex > 0) {
        m_isNavigating = true;
        m_historyIndex--;
        openLocation(m_history.at(m_historyIndex));
        
        m_backAction->setEnabled(m_historyIndex > 0);
        m_forwardAction->setEnabled(m_historyIndex < m_history.size() - 1);
        m_isNavigating = false;
    }
}

void FileManager::navigateForward()
{
    if (m_historyIndex < m_history.size() - 1) {
        m_isNavigating = true;
        m_historyIndex++;
        openLocation(m_history.at(m_historyIndex));
        
        m_backAction->setEnabled(m_historyIndex > 0);
        m_forwardAction->setEnabled(m_historyIndex < m_history.size() - 1);
        m_isNavigating = false;
    }
}

void FileManager::navigateHome()
{
    openLocation(QDir::homePath());
}

void FileManager::refresh()
{
    m_fileModel->setRootPath("");
    openLocation(m_fileModel->rootPath());
}

void FileManager::createNewFolder()
{
    QString currentPath = m_fileModel->rootPath();
    QString newFolderName = QInputDialog::getText(this, "Yeni Klasör", "Klasör adı:");
    
    if (!newFolderName.isEmpty()) {
        QDir dir(currentPath);
        if (!dir.mkdir(newFolderName)) {
            QMessageBox::warning(this, "Hata", "Klasör oluşturulamadı.");
        }
    }
}

void FileManager::onLocationChanged(const QModelIndex &index)
{
    QString path = m_fileModel->filePath(index);
    QFileInfo fileInfo(path);
    
    if (fileInfo.isDir()) {
        openLocation(path);
    }
}

void FileManager::onPathEdited()
{
    QString path = m_pathEdit->text();
    QFileInfo fileInfo(path);
    
    if (fileInfo.exists() && fileInfo.isDir()) {
        openLocation(path);
    } else {
        m_pathEdit->setText(m_fileModel->rootPath());
    }
}

void FileManager::onFileActivated(const QModelIndex &index)
{
    QString path = m_fileModel->filePath(index);
    QFileInfo fileInfo(path);
    
    if (fileInfo.isDir()) {
        openLocation(path);
    } else {
        // Dosyayı varsayılan uygulamayla aç
        QDesktopServices::openUrl(QUrl::fromLocalFile(path));
    }
}

void FileManager::updatePathBox()
{
    m_pathBox->clear();
    
    QString path = m_fileModel->rootPath();
    QStringList parts = path.split("/", Qt::SkipEmptyParts);
    
    QString currentPath = "/";
    m_pathBox->addItem("/");
    
    for (const QString &part : parts) {
        currentPath += part + "/";
        m_pathBox->addItem(currentPath);
    }
    
    m_pathBox->setCurrentIndex(m_pathBox->count() - 1);
}

void FileManager::handleContextMenu(const QPoint &pos)
{
    QModelIndex index = m_fileView->indexAt(pos);
    QMenu menu(this);
    
    if (index.isValid()) {
        // Dosya veya klasör seçili
        QString filePath = m_fileModel->filePath(index);
        QFileInfo fileInfo(filePath);
        
        if (fileInfo.isDir()) {
            menu.addAction(QIcon::fromTheme("folder-open"), "Aç", [this, index]() {
                onFileActivated(index);
            });
        } else {
            menu.addAction(QIcon::fromTheme("document-open"), "Aç", [this, index]() {
                onFileActivated(index);
            });
            
            menu.addAction("İle Aç...", [this, filePath]() {
                // Uygulama seçme diyaloğu
                QString program = QInputDialog::getText(this, "İle Aç", "Uygulama:");
                if (!program.isEmpty()) {
                    QProcess::startDetached(program, QStringList() << filePath);
                }
            });
        }
        
        menu.addSeparator();
        menu.addAction(QIcon::fromTheme("edit-cut"), "Kes");
        menu.addAction(QIcon::fromTheme("edit-copy"), "Kopyala");
        menu.addSeparator();
        menu.addAction(QIcon::fromTheme("edit-delete"), "Sil", [this, filePath]() {
            if (QMessageBox::question(this, "Sil", QString("\"%1\" öğesini silmek istediğinize emin misiniz?").arg(QFileInfo(filePath).fileName())) == QMessageBox::Yes) {
                QFile file(filePath);
                file.remove();
            }
        });
        menu.addAction(QIcon::fromTheme("edit-rename"), "Yeniden Adlandır", [this, index]() {
            m_fileView->edit(index);
        });
        menu.addSeparator();
        menu.addAction(QIcon::fromTheme("document-properties"), "Özellikler");
    } else {
        // Boş alana tıklandı
        menu.addAction(m_newFolderAction);
        menu.addSeparator();
        menu.addAction(QIcon::fromTheme("edit-paste"), "Yapıştır");
        menu.addSeparator();
        menu.addAction(m_refreshAction);
    }
    
    menu.exec(m_fileView->mapToGlobal(pos));
}

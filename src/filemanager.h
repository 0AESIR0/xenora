#ifndef FILEMANAGER_H
#define FILEMANAGER_H

#include <QMainWindow>
#include <QListView>
#include <QTreeView>
#include <QFileSystemModel>
#include <QToolBar>
#include <QLineEdit>
#include <QComboBox>
#include <QAction>
#include <QSplitter>
#include <QStatusBar>

class FileManager : public QMainWindow
{
    Q_OBJECT
    
public:
    explicit FileManager(QWidget *parent = nullptr);
    ~FileManager();
    
    void openLocation(const QString &path);
    
private slots:
    void navigateUp();
    void navigateBack();
    void navigateForward();
    void navigateHome();
    void refresh();
    void createNewFolder();
    void onLocationChanged(const QModelIndex &index);
    void onPathEdited();
    void onFileActivated(const QModelIndex &index);
    void handleContextMenu(const QPoint &pos);
    
private:
    void setupUI();
    void setupActions();
    void setupToolbar();
    void setupMenus();
    void updatePathBox();
    
    QFileSystemModel *m_fileModel;
    QListView *m_fileView;
    QTreeView *m_sidebarView;
    QSplitter *m_splitter;
    QToolBar *m_toolbar;
    QLineEdit *m_pathEdit;
    QComboBox *m_pathBox;
    QStatusBar *m_statusBar;
    
    QAction *m_backAction;
    QAction *m_forwardAction;
    QAction *m_upAction;
    QAction *m_homeAction;
    QAction *m_refreshAction;
    QAction *m_newFolderAction;
    
    QStringList m_history;
    int m_historyIndex;
    bool m_isNavigating;
};

#endif // FILEMANAGER_H

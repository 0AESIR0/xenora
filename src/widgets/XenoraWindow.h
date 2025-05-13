#pragma once

#include <QWidget>
#include <QPoint>
#include <QRect>
#include <QString>
#include <QPixmap>
#include <QBoxLayout>
#include <QLabel>
#include <memory>

class XenoraButton;

class XenoraWindow : public QWidget
{
    Q_OBJECT
    
public:
    enum ResizeDirection {
        None = 0,
        Top = 1,
        Right = 2,
        Bottom = 4,
        Left = 8,
        TopLeft = Top | Left,
        TopRight = Top | Right,
        BottomLeft = Bottom | Left,
        BottomRight = Bottom | Right
    };
    
    enum WindowState {
        Normal,
        Maximized,
        Minimized,
        FullScreen
    };
    
    explicit XenoraWindow(QWidget *parent = nullptr);
    explicit XenoraWindow(const QString &title, QWidget *parent = nullptr);
    ~XenoraWindow();
    
    void setContent(QWidget *content);
    void setIcon(const QPixmap &icon);
    void setTitle(const QString &title);
    
    QString title() const;
    WindowState windowState() const;
    
public slots:
    void minimize();
    void maximize();
    void restore();
    void toggleMaximize();
    void showFullScreen();
    void exitFullScreen();
    void closeWindow();
    
signals:
    void windowStateChanged(WindowState newState);
    void windowClosed();
    
protected:
    void paintEvent(QPaintEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void resizeEvent(QResizeEvent *event) override;
    bool eventFilter(QObject *watched, QEvent *event) override;
    
private:
    QString m_title;
    QPixmap m_icon;
    WindowState m_windowState;
    QPoint m_dragPosition;
    bool m_isDragging;
    ResizeDirection m_resizeDirection;
    QRect m_normalGeometry;
    
    // Window components
    QWidget *m_titleBar;
    QLabel *m_titleLabel;
    QLabel *m_iconLabel;
    XenoraButton *m_minimizeButton;
    XenoraButton *m_maximizeButton;
    XenoraButton *m_closeButton;
    QWidget *m_contentContainer;
    QVBoxLayout *m_mainLayout;
    QHBoxLayout *m_titleBarLayout;
    QVBoxLayout *m_contentLayout;
    
    // Resize areas
    int m_borderWidth;
    
    void setupUi();
    void createTitleBar();
    void createWindowButtons();
    void updateWindowButtons();
    ResizeDirection getResizeDirection(const QPoint &pos);
    void updateCursor(ResizeDirection direction);
};

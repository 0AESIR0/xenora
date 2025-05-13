#pragma once

#include <QWidget>
#include <QPoint>
#include <QRect>
#include <QString>
#include <X11/Xlib.h>

class QLabel;
class XenoraButton;

class XenoraWindow : public QWidget
{
    Q_OBJECT
    
public:
    explicit XenoraWindow(QWidget *parent = nullptr, Window xWindowId = 0);
    ~XenoraWindow();
    
    void setTitle(const QString &title);
    QString title() const;
    
    Window windowId() const { return m_windowId; }
    
    void setMaximized(bool maximized);
    bool isMaximized() const { return m_isMaximized; }
    
    void restorePreviousGeometry();
    void saveCurrentGeometry();
    
protected:
    void paintEvent(QPaintEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;
    void resizeEvent(QResizeEvent *event) override;
    
private slots:
    void handleCloseClicked();
    void handleMaximizeClicked();
    void handleMinimizeClicked();
    
private:
    Window m_windowId;
    QLabel *m_titleLabel;
    XenoraButton *m_closeButton;
    XenoraButton *m_maximizeButton;
    XenoraButton *m_minimizeButton;
    QWidget *m_titleBar;
    QRect m_previousGeometry;
    bool m_isMaximized;
    bool m_isDragging;
    QPoint m_dragPosition;
    
    enum ResizeArea {
        None,
        Top,
        Bottom,
        Left,
        Right,
        TopLeft,
        TopRight,
        BottomLeft,
        BottomRight
    };
    
    ResizeArea m_resizeArea;
    
    void setupUi();
    void setupTitleBar();
    void setupWindowButtons();
    ResizeArea getResizeArea(const QPoint &pos);
    void updateCursor(const QPoint &pos);
};

#ifndef DESKTOPICON_H
#define DESKTOPICON_H

#include <QWidget>
#include <QFileInfo>
#include <QLabel>
#include <QEvent>
#include <QMouseEvent>
#include <QPoint>

class DesktopIcon : public QWidget
{
    Q_OBJECT
    
public:
    explicit DesktopIcon(const QFileInfo &fileInfo, QWidget *parent = nullptr);
    ~DesktopIcon();
    
    QString path() const { return m_fileInfo.absoluteFilePath(); }
    QFileInfo fileInfo() const { return m_fileInfo; }
    
protected:
    void paintEvent(QPaintEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;
    void mouseDoubleClickEvent(QMouseEvent *event) override;
    void enterEvent(QEvent *event) override;
    void leaveEvent(QEvent *event) override;
    
private:
    void setupIcon();
    void createContextMenu(const QPoint &pos);
    void launchFile();
    
    QFileInfo m_fileInfo;
    QLabel *m_iconLabel;
    QLabel *m_textLabel;
    
    bool m_hover;
    bool m_dragging;
    QPoint m_dragStartPosition;
};

#endif // DESKTOPICON_H

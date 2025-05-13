#ifndef DESKTOPICONVIEW_H
#define DESKTOPICONVIEW_H

#include <QWidget>
#include <QVector>
#include <QPair>
#include <QPixmap>
#include <QString>
#include <QPoint>

class DesktopIconView : public QWidget {
    Q_OBJECT

public:
    explicit DesktopIconView(QWidget *parent = nullptr);
    ~DesktopIconView();

    void addIcon(const QString &name, const QString &iconPath);

protected:
    void paintEvent(QPaintEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;
    void mouseDoubleClickEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;

signals:
    void iconClicked(const QString &name);
    void iconDoubleClicked(const QString &name);

private:
    struct DesktopIcon {
        QString name;
        QPixmap icon;
        QRect rect;
        bool selected;
    };

    QVector<DesktopIcon> m_icons;
    int m_iconSize;
    int m_spacing;
    int m_textHeight;
    int m_columns;
    
    int m_draggedIcon;
    QPoint m_dragStartPos;
    bool m_isDragging;

    void calculateIconPositions();
    int getIconAtPosition(const QPoint &pos);
};

#endif // DESKTOPICONVIEW_H

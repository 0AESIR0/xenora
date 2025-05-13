#pragma once

#include <QWidget>
#include <QString>

class QVBoxLayout;
class QLabel;
class XenoraButton;

class XenoraPopup : public QWidget
{
    Q_OBJECT
    
public:
    explicit XenoraPopup(QWidget *parent = nullptr, const QString &title = "");
    ~XenoraPopup();
    
    void setTitle(const QString &title);
    QString title() const;
    
    void setContent(QWidget *content);
    void setSize(int width, int height);
    
public slots:
    void show();
    void close();
    
protected:
    void paintEvent(QPaintEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;
    
private:
    QString m_title;
    QVBoxLayout *m_mainLayout;
    QWidget *m_titleBar;
    QLabel *m_titleLabel;
    XenoraButton *m_closeButton;
    QWidget *m_contentArea;
    QVBoxLayout *m_contentLayout;
    
    bool m_isDragging;
    QPoint m_dragStartPosition;
    
    void setupUi();
};

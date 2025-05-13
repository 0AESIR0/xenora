#ifndef XENORAWINDOW_H
#define XENORAWINDOW_H

#include <QWidget>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QPoint>

class XenoraWindow : public QWidget {
    Q_OBJECT

public:
    explicit XenoraWindow(QWidget *parent = nullptr);
    explicit XenoraWindow(const QString &title, QWidget *parent = nullptr);
    ~XenoraWindow();

    void setTitle(const QString &title);
    void setContent(QWidget *content);
    void setResizable(bool resizable);

protected:
    void mousePressEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;
    void paintEvent(QPaintEvent *event) override;

private slots:
    void onCloseClicked();
    void onMinimizeClicked();
    void onMaximizeClicked();

private:
    QWidget *m_titleBar;
    QLabel *m_titleLabel;
    QPushButton *m_closeButton;
    QPushButton *m_minimizeButton;
    QPushButton *m_maximizeButton;
    QWidget *m_contentWidget;
    QVBoxLayout *m_mainLayout;

    bool m_isDragging;
    QPoint m_dragStartPosition;
    bool m_isResizable;
    bool m_isMaximized;

    void initializeWindow();
    void createTitleBar();
    void setupWindowStyle();
};

#endif // XENORAWINDOW_H

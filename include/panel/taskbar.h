#pragma once

#include <QWidget>
#include <QBoxLayout>
#include <QList>
#include <QMap>
#include <QString>
#include <QPushButton>

class TaskbarButton : public QPushButton {
    Q_OBJECT
    
public:
    explicit TaskbarButton(const QString &title, const QIcon &icon, QWidget *parent = nullptr);
    void setActive(bool active);
    
protected:
    void paintEvent(QPaintEvent *event) override;
    
private:
    bool m_active;
};

class Taskbar : public QWidget {
    Q_OBJECT

public:
    explicit Taskbar(QWidget *parent = nullptr);
    ~Taskbar();
    
    void addApplication(const QString &title, const QIcon &icon);
    void removeApplication(const QString &title);
    
private:
    QHBoxLayout *m_layout;
    QMap<QString, TaskbarButton*> m_buttons;
    
private slots:
    void onTaskButtonClicked();
};

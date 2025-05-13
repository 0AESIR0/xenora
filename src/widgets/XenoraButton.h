#pragma once

#include <QPushButton>
#include <QPropertyAnimation>
#include <QGraphicsDropShadowEffect>

class XenoraButton : public QPushButton
{
    Q_OBJECT
    Q_PROPERTY(qreal elevation READ elevation WRITE setElevation)
    
public:
    enum ButtonStyle {
        Primary,
        Secondary,
        Flat,
        Icon,
        Panel
    };
    
    explicit XenoraButton(QWidget *parent = nullptr);
    explicit XenoraButton(const QString &text, QWidget *parent = nullptr);
    
    qreal elevation() const { return m_elevation; }
    void setElevation(qreal elevation);
    
    void setButtonStyle(ButtonStyle style);
    void setHoverColor(const QColor &color);
    void setIconSize(int size);
    
protected:
    void paintEvent(QPaintEvent *event) override;
    void enterEvent(QEvent *event) override;
    void leaveEvent(QEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;
    
private:
    qreal m_elevation;
    QPropertyAnimation *m_elevationAnimation;
    QColor m_hoverColor;
    ButtonStyle m_style;
    QGraphicsDropShadowEffect *m_shadowEffect;
    
    void initButton();
    void setupAnimation();
    void updateStyle();
};

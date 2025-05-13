#pragma once

#include <QWidget>
#include <QColor>

class XenoraPanel : public QWidget
{
    Q_OBJECT
    
public:
    explicit XenoraPanel(QWidget *parent = nullptr);
    
    void setBackgroundColor(const QColor &color);
    QColor backgroundColor() const;
    
    void setBlurRadius(int radius);
    int blurRadius() const;
    
    void setCornerRadius(int radius);
    int cornerRadius() const;
    
    void setBorderWidth(int width);
    int borderWidth() const;
    
    void setBorderColor(const QColor &color);
    QColor borderColor() const;
    
    void setBlurEnabled(bool enabled);
    bool blurEnabled() const;
    
protected:
    void paintEvent(QPaintEvent *event) override;
    
private:
    QColor m_backgroundColor;
    QColor m_borderColor;
    int m_blurRadius;
    int m_cornerRadius;
    int m_borderWidth;
    bool m_blurEnabled;
};

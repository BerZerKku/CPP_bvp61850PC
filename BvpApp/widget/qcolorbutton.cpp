#include "qcolorbutton.h"

QColorButton::QColorButton(QWidget *parent) : QPushButton(parent)
{
    set(false);
}

void QColorButton::set(bool enable)
{
    changeColor(enable ? mColor : colorDefault);
}

void QColorButton::setColor(Qt::GlobalColor color)
{
    mColor = color;
}

void QColorButton::changeColor(Qt::GlobalColor color)
{
    QColor col = QColor(color);
    if (col.isValid())
    {
        QString qss = QString("background-color: %1").arg(col.name());
        setStyleSheet(qss);
    }
}

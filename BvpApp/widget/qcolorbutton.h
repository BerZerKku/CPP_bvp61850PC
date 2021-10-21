#ifndef QCOLORBUTTON_H
#define QCOLORBUTTON_H

#include <QPushButton>

class QColorButton : public QPushButton
{
    const Qt::GlobalColor colorDefault = Qt::gray;

public:
    QColorButton(QWidget *parent = nullptr);

    void set(bool enable);
    void setColor(Qt::GlobalColor color);

signals:
    void changed(bool enable);

private:
    Qt::GlobalColor mColor = Qt::darkGreen;

    void changeColor(Qt::GlobalColor color);
};

#endif  // QCOLORBUTTON_H

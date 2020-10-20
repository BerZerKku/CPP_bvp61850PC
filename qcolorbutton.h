#ifndef QCOLORBUTTON_H
#define QCOLORBUTTON_H

#include <QPushButton>

class QColorButton : public QPushButton {
public:
    QColorButton();

    void set(bool enable);

signals:
    void changed(bool enable);

private:
    void setColor(Qt::GlobalColor color);
};

#endif // QCOLORBUTTON_H

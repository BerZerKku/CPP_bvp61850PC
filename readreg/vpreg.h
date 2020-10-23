#ifndef VPREG_H
#define VPREG_H

#include <QWidget>

namespace Ui {
class vpReg;
}

class vpReg : public QWidget
{
    Q_OBJECT

public:
    explicit vpReg(QWidget *parent = nullptr);
    ~vpReg();

private:
    Ui::vpReg *ui;
};

#endif // VPREG_H

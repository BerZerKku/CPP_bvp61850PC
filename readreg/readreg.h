#ifndef TREADREG_H
#define TREADREG_H

#include "qcolorbutton.h"
#include <QLineEdit>
#include <QSignalMapper>
#include <QVector>
#include <QWidget>

class TReadReg : public QWidget {
    Q_OBJECT

    /// Количество регистров для чтения
    static const quint8 kNumberOfReadRegisters = 5;

    typedef struct {
        QLineEdit value;
        std::array<QColorButton, 2*CHAR_BIT> widgets;
    } reg_t;

public:
    explicit TReadReg(QWidget *parent = nullptr);

    void setReg(quint8 number, quint16 value);

private:  
    std::array<reg_t, kNumberOfReadRegisters> rReg;

    QLayout *crtRegisterlayout(reg_t &reg);
};

#endif // TREADREG_H

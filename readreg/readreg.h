#ifndef TREADREG_H
#define TREADREG_H

#include "qcolorbutton.h"
#include <QLineEdit>
#include <QSignalMapper>
#include <QVector>
#include <QWidget>
#include "readreg/qledindicator.h"
#include "readreg/vpreg.h"

class TReadReg : public QWidget {
    Q_OBJECT

    /// Количество регистров для чтения
    static const quint8 kNumberOfReadRegisters = 5;

    typedef struct {
        QLineEdit value;
        std::array<QColorButton, 2*CHAR_BIT> button;
        std::array<QLedIndicator, 2*CHAR_BIT> ledEnabe;
        std::array<QLedIndicator, 2*CHAR_BIT> ledDisable;
    } reg_t;


public:
    explicit TReadReg(QWidget *parent = nullptr);

    void setReg(quint8 number, quint16 value);

private:  
//    std::array<reg_t, kNumberOfReadRegisters> rReg;

    QLayout *crtRegisterlayout(reg_t &reg);

    std::array<vpReg, kNumberOfReadRegisters> rReg;
};

#endif // TREADREG_H

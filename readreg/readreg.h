#ifndef TREADREG_H
#define TREADREG_H

#include "qcolorbutton.h"
#include <QLineEdit>
#include <QSignalMapper>
#include <QVector>
#include <QWidget>
#include "readreg/vpreg.h"

class TReadReg : public QWidget {
    Q_OBJECT

    /// Количество регистров для чтения
    static const quint8 kNumberOfReadRegisters = 5;

public:

    enum regFunc_t {
        REG_FUNC_LED_ENABLE = 0,
        REG_FUNC_LED_DISABLE,
        REG_FUNC_BUTTON
    };

    explicit TReadReg(QWidget *parent = nullptr);

    void setReg(vpReg::group_t group, regFunc_t func, quint16 value);

    void setCom64(bool enable);

    void clear();

private:  
    std::array<vpReg, kNumberOfReadRegisters> rReg;
};

#endif // TREADREG_H

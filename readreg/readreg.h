#ifndef TREADREG_H
#define TREADREG_H

#include <QPushButton>
#include <QVector>
#include <QWidget>

class TReadReg : public QWidget {
    Q_OBJECT
    typedef QVector<QPushButton*> reg_t;
    /// Количество регистров для чтения
    static const quint8 kNumberOfRegisters = 5;

    static const QString regNames[kNumberOfRegisters][2*CHAR_BIT];

public:
    explicit TReadReg(QWidget *parent = nullptr);

signals:

private:
    QVector<reg_t*> rReg;

};

#endif // TREADREG_H

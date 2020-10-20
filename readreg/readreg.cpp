#include "readreg.h"
#include "QDebug"
#include "QGridLayout"
#include "QLabel"


TReadReg::TReadReg(QWidget *parent) : QWidget(parent) {
    QGridLayout *l = new QGridLayout();

    rReg.at(0).widgets.at(0).setText("SAC1");
    rReg.at(0).widgets.at(1).setText("SAC2");
    rReg.at(0).widgets.at(2).setText("SB1");
    for(quint8 i = 0; i < CHAR_BIT; i++) {
        QString name = QString("SAn.%1").arg(i);
        rReg.at(0).widgets.at(8 + i).setText(name);
    }

    for(quint8 index = 1; index < kNumberOfReadRegisters; index++) {
        for(quint8 i = 0; i < rReg.at(0).widgets.size(); i++) {
            QString name = QString("SA%1").arg(16*(index-1) + i + 1);
            rReg.at(index).widgets.at(i).setText(name);
        }
    }

    for(quint8 index = 0; index < rReg.size(); index++) {
        quint8 row = index + 1;
        quint8 col = 0;
        reg_t *reg = &rReg.at(index);

        reg->value.setReadOnly(true);
        reg->value.setAlignment(Qt::AlignRight);

        l->addWidget(new QLabel("1"), row, col++);
        l->addWidget(&reg->value, row, col++);

        for(size_t bit = rReg.at(0).widgets.size(); bit > 0; bit--) {

            l->addWidget(&reg->widgets.at(bit-1), row, col++);
        }
    }

    QVBoxLayout *vl = new QVBoxLayout(this);
    vl->addWidget(new QLabel("Holding Registers"));
    vl->addLayout(l);

}

void
TReadReg::setReg(quint8 number, quint16 value) {
    Q_ASSERT((number > 0) && (number <= rReg.size()));

    reg_t *reg = &rReg.at(number - 1);
    reg->value.setText(QString("%1").arg(value, 4, 16, QLatin1Char('0')));
    for(quint16 i = 0; i < reg->widgets.size(); i++) {
        reg->widgets.at(i).set(value & (1 << i));
    }
}

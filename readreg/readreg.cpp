#include "readreg.h"
#include "QDebug"
#include "QGridLayout"
#include "QLabel"


TReadReg::TReadReg(QWidget *parent) : QWidget(parent) {
    QVBoxLayout *layout = new QVBoxLayout(this);
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

    QHBoxLayout *hl = new QHBoxLayout();
    for(uint8_t index = 1; index < rReg.size(); index++) {
        hl->addLayout(crtRegisterlayout(rReg.at(index)));
    }
    hl->addLayout(crtRegisterlayout(rReg.at(0)));

    layout->addWidget(new QLabel("Holding Registers"));
    layout->addLayout(hl);

    setFixedHeight(sizeHint().height());
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

//
QLayout*
TReadReg::crtRegisterlayout(TReadReg::reg_t &reg) {
    QVBoxLayout *layout = new QVBoxLayout();

    reg.value.setReadOnly(true);
    reg.value.setAlignment(Qt::AlignHCenter);
    layout->addWidget(&reg.value);

    QHBoxLayout *hl = new QHBoxLayout();
    for(uint8_t col = 0; col < 2; col++) {
        QVBoxLayout *vl = new QVBoxLayout();
        for(uint8_t bit = 0; bit < CHAR_BIT; bit++) {
            qDebug() << "bit - 1" << bit - 1;
            vl->addWidget(&reg.widgets.at(bit + col*CHAR_BIT));
        }
        hl->addLayout(vl);
    }

    layout->addLayout(hl);

    return layout;
}

#include "readreg.h"
#include "QDebug"
#include "QGridLayout"
#include "QLabel"


TReadReg::TReadReg(QWidget *parent) : QWidget(parent) {
    QGridLayout *l = new QGridLayout();
    rReg.append(new reg_t);
    rReg.back()->append(new QPushButton("SAC1"));
    rReg.back()->append(new QPushButton("SAC2"));
    rReg.back()->append(new QPushButton("SB1"));
    rReg.back()->append(new QPushButton(""));
    rReg.back()->append(new QPushButton(""));
    rReg.back()->append(new QPushButton(""));
    rReg.back()->append(new QPushButton(""));
    rReg.back()->append(new QPushButton(""));
    for(quint8 i = 0; i < CHAR_BIT; i++) {
        QString name = QString("SAn.%1").arg(i);
        rReg.back()->append(new QPushButton(name));
    }

    for(quint8 index = 0; index < rReg.count(); index++) {
        reg_t *reg = rReg.at(index);
        Q_ASSERT(reg->size() == 2*CHAR_BIT);
        l->addWidget(new QLabel("1"), 1+index, 0);
        for(quint8 bit = reg->size(); bit > 0; bit--) {
            qDebug() << "index = " << index << ", bit = " << bit;
            l->addWidget(reg->at(bit-1), 1+index, reg->size() - bit + 1);
        }
    }

    QVBoxLayout *vl = new QVBoxLayout(this);
    vl->addWidget(new QLabel("Holding Registers"));
    vl->addLayout(l);

}

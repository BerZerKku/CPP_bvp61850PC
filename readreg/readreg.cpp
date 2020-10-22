#include "readreg.h"
#include <QDebug>
#include <QGridLayout>
#include <QLabel>
#include <QSpacerItem>


TReadReg::TReadReg(QWidget *parent) : QWidget(parent) {
    QVBoxLayout *layout = new QVBoxLayout(this);
    quint8 index = 0;

    for(quint8 i = 0; i < rReg.at(0).button.size(); i++) {
        QString name;
        if (i == 0) {
            name = "SAC1";
        } else if (i == 1) {
            name = "SAC2";
        } else if (i == 2) {
            name = "SB1";
        } else if (i >= CHAR_BIT) {
            name = QString("SAn.%1").arg(i - CHAR_BIT + 1);
        }

        rReg.at(index).button.at(i).setEnabled(!name.isNull());
        rReg.at(index).button.at(i).setText(name);
        rReg.at(index).ledEnabe.at(i).setEnabled(!name.isNull());
        rReg.at(index).ledEnabe.at(i).setCheckable(false);
        rReg.at(index).ledDisable.at(i).setEnabled(!name.isNull());
        rReg.at(index).ledDisable.at(i).setCheckable(false);
    }

    rReg.at(index).button.at(0).setText("SAC1");
    rReg.at(index).button.at(1).setText("SAC2");
    rReg.at(index).button.at(2).setText("SB1");
    for(quint8 i = 0; i < CHAR_BIT; i++) {
        QString name = QString("SAn.%1").arg(i);
        rReg.at(index).button.at(8 + i).setText(name);
        rReg.at(index).ledEnabe.at(i).setCheckable(false);
        rReg.at(index).ledDisable.at(i).setCheckable(false);
    }

    for(index++; index < kNumberOfReadRegisters; index++) {
        for(quint8 i = 0; i < rReg.at(0).button.size(); i++) {
            QString name = QString("SA%1").arg(16*(index-1) + i + 1);
            rReg.at(index).button.at(i).setText(name);
            rReg.at(index).ledEnabe.at(i).setCheckable(false);
            rReg.at(index).ledDisable.at(i).setCheckable(false);
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
    for(quint16 i = 0; i < reg->button.size(); i++) {
        reg->button.at(i).set(value & (1 << i));
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
            QHBoxLayout *h = new QHBoxLayout();
            if (reg.ledEnabe.at(bit + col*CHAR_BIT).isEnabled()) {
                h->addWidget(&reg.ledEnabe.at(bit + col*CHAR_BIT));
            }
            if (reg.ledDisable.at(bit + col*CHAR_BIT).isEnabled()) {
                h->addWidget(&reg.ledDisable.at(bit + col*CHAR_BIT));
            }
            if (reg.button.at(bit + col*CHAR_BIT).isEnabled()) {
                h->addWidget(&reg.button.at(bit + col*CHAR_BIT));
            }
            vl->addLayout(h);
            vl->addStretch(1);
        }
        hl->addLayout(vl);
    }

    layout->addLayout(hl);

    return layout;
}

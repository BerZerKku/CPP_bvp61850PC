#include "readreg/vpreg.h"
#include "ui_vpreg.h"
#include <QDebug>
#include <QSignalMapper>

//
vpReg::vpReg(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::vpReg)
{
    ui->setupUi(this);

    QFont font = ui->sbRegButton->font();
    font.setCapitalization(QFont::AllUppercase);

    ui->sbRegButton->setFont(font);
    ui->sbRegButton->setMaximum(0xFFFF);

    ui->sbRegEnable->setFont(font);
    ui->sbRegEnable->setMaximum(0xFFFF);

    ui->sbRegDisable->setFont(font);
    ui->sbRegDisable->setMaximum(0xFFFF);

    QSignalMapper *signalMapper = new QSignalMapper(this);

    for(quint8 col = 0; col < 2; col++) {
        for(quint8 row = 0; row < CHAR_BIT; row++) {
            quint8 bit = col*CHAR_BIT + row;
            ui->bitLayout->addWidget(&bits.at(bit), row, col);
            connect(&bits.at(bit), &vpItem::pressedBtn,
                    signalMapper, QOverload<>::of(&QSignalMapper::map));
            signalMapper->setMapping(&bits.at(bit), bit);
        }
    }

    connect(signalMapper, QOverload<int>::of(&QSignalMapper::mapped),
            this, &vpReg::pressedBtnSlot);
}

//
vpReg::~vpReg() {
    delete ui;
}

void vpReg::setGroup(vpReg::group_t group) {
    switch(group) {
        case GROUP_com16to01: setGroupCom16to01(); break;
        case GROUP_com32to17: setGroupCom32to17(); break;
        case GROUP_com48to33: setGroupCom48to33(); break;
        case GROUP_com64to49: setGroupCom64to49(); break;
        case GROUP_control: setGroupControl(); break;
    }
}

//
void
vpReg::setEnLeds(quint16 value) {
    ui->sbRegEnable->setValue(value);
    for(size_t index = 0; index < bits.size(); index++) {
        bits.at(index).setLedEn(value & (1 << index));
    }
}

//
void
vpReg::setDsLeds(quint16 value) {
    ui->sbRegDisable->setValue(value);
    for(size_t index = 0; index < bits.size(); index++) {
        bits.at(index).setLedDs(value & (1 << index));
    }
}

//
void
vpReg::setBtns(quint16 value) {
    ui->sbRegButton->setValue(value);
    for(size_t index = 0; index < bits.size(); index++) {
        bits.at(index).setBtn(value & (1 << index));
    }
}

//
void
vpReg::setRegime(vpItem::regime_t regime) {
    for(vpItem &bit: bits) {
        bit.setRegime(regime);
    }
}

//
void
vpReg::setGroupCom16to01() {
    qDebug() << "bits.size()" << bits.size();
    for(size_t index = 0; index < bits.size(); index++) {
        qDebug() << "index = " << index;
        bits.at(index).setText(QString("SA%1").arg(index + 1));
    }
}

//
void
vpReg::setGroupCom32to17() {
    for(size_t index = 0; index < bits.size(); index++) {
        bits.at(index).setText(QString("SA%1").arg(index + 17));
    }
}

//
void
vpReg::setGroupCom48to33() {
    for(size_t index = 0; index < bits.size(); index++) {
        bits.at(index).setText(QString("SA%1").arg(index + 33));
    }
}

//
void
vpReg::setGroupCom64to49() {
    for(size_t index = 0; index < bits.size(); index++) {
        bits.at(index).setText(QString("SA%1").arg(index + 49));
    }
}

//
void
vpReg::setGroupControl() {
    size_t index = 0;
    bits.at(index++).setText(QString("SAC1"));
    bits.at(index++).setText(QString("SAC2"));
    bits.at(index++).setText(QString("SB1"));
    bits.at(index++).setHidden(true);
    bits.at(index++).setHidden(true);
    bits.at(index++).setHidden(true);
    bits.at(index++).setHidden(true);
    bits.at(index++).setHidden(true);
    for(;index < bits.size(); index++) {
        bits.at(index).setText(QString("SAn%1").arg(index - CHAR_BIT));
    }
}

//
void
vpReg::pressedBtnSlot(int value) {
//    quint16 v = 0;

//    for(quint8 i = 0; i < bits.size(); i++) {
//        if (bits.at(i).getBtn()) {
//            v += (1 << i);
//        }
//    }

//    ui->sbRegButton->setValue(v);
}

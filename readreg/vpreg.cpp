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
    ui->sbRegButton->setDisplayIntegerBase(16);
    ui->sbRegButton->setReadOnly(true);
    connect(ui->sbRegButton, QOverload<int>::of(&QSpinBox::valueChanged),
            this, &vpReg::changedBtns);

    ui->sbRegEnable->setFont(font);
    ui->sbRegEnable->setMaximum(0xFFFF);
    ui->sbRegEnable->setDisplayIntegerBase(16);
    ui->sbRegEnable->setReadOnly(true);
    connect(ui->sbRegEnable, QOverload<int>::of(&QSpinBox::valueChanged),
            this, &vpReg::changedEnLeds);

    ui->sbRegDisable->setFont(font);
    ui->sbRegDisable->setMaximum(0xFFFF);
    ui->sbRegDisable->setDisplayIntegerBase(16);
    ui->sbRegDisable->setReadOnly(true);
    connect(ui->sbRegDisable, QOverload<int>::of(&QSpinBox::valueChanged),
            this, &vpReg::changedDsLeds);

    for(quint8 col = 0; col < 2; col++) {
        for(quint8 row = 0; row < CHAR_BIT; row++) {
            quint8 bit = col*CHAR_BIT + row;
            ui->bitLayout->addWidget(&bits.at(bit), row, col);
            connect(&bits.at(bit), &vpItem::pressedBtn,
                    [=](bool checked) { pressedBtnSlot(bit, checked); });
        }
    }


    clear();
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
}

//
void
vpReg::setDsLeds(quint16 value) {
    ui->sbRegDisable->setValue(value);

}

//
void
vpReg::setBtns(quint16 value) {
    ui->sbRegButton->setValue(value);

}

void
vpReg::clear() {
    setBtns(0);
    setEnLeds(0);
    setDsLeds(0);
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
    for(size_t index = 0; index < bits.size(); index++) {
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
    bits.at(index).ledDsEnabled(false);
    bits.at(index).ledEnEnabled(false);
    bits.at(index++).setText(QString("SB1"));
    bits.at(index++).setHidden(true);
    bits.at(index++).setHidden(true);
    bits.at(index++).setHidden(true);
    bits.at(index++).setHidden(true);
    bits.at(index++).setHidden(true);
    Q_ASSERT(index == 8);
    for(;index < bits.size(); index++) {
        bits.at(index).setText(QString("SAn%1").arg(index - CHAR_BIT));
    }
}

//
void
vpReg::pressedBtnSlot(int bit, bool checked) {
    qDebug() << "bit = " << bit << ", checked = " << checked;
}

void
vpReg::changedEnLeds(int value) {
    for(size_t index = 0; index < bits.size(); index++) {
        bits.at(index).setLedEn(value & (1 << index));
    }
}

void
vpReg::changedDsLeds(int value) {
    for(size_t index = 0; index < bits.size(); index++) {
        bits.at(index).setLedDs(value & (1 << index));
    }
}

void
vpReg::changedBtns(int value) {
    for(size_t index = 0; index < bits.size(); index++) {
        bits.at(index).setBtn(value & (1 << index));
    }
}

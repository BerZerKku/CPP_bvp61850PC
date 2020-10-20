#include "control.h"
#include "ui_control.h"

TControl::TControl(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::TControl)
{
    ui->setupUi(this);

    connect(ui->pRead, &QPushButton::pressed,
            this, &TControl::readRegisters);

    connect(ui->pWrite, &QPushButton::pressed,
            this, &TControl::writeRegisters);

    connect(ui->pReadWrite, &QPushButton::pressed,
            this, &TControl::readAndWriteRegisters);

    setEnable(false);
}

TControl::~TControl() {
    delete ui;
}

//
void
TControl::disableSlot() {
    setEnable(false);
}

//
void
TControl::enableSlot() {
    setEnable(true);
}

//
void TControl::setEnable(bool enable) {
    ui->pRead->setEnabled(enable);
    ui->pWrite->setEnabled(enable);
    ui->pReadWrite->setEnabled(enable);
    ui->checkBox->setEnabled(enable);
}

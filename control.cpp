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

    connect(ui->pModbusStop, &QPushButton::pressed,
            this, &TControl::disableModbus);
    connect(ui->pModbusStop, &QPushButton::pressed,
            this, &TControl::modbusStop);

    connect(ui->pModbusStart, &QPushButton::pressed,
            this, &TControl::enableModbus);
    connect(ui->pModbusStart, &QPushButton::pressed,
            this, &TControl::modbusStart);

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
    if (!enable) {
        ui->pModbusStart->setEnabled(false);
        ui->pModbusStop->setEnabled(false);
    } else {
         setEnableModbus(false);
    }

    ui->pRead->setEnabled(enable);
    ui->pWrite->setEnabled(enable);
    ui->pReadWrite->setEnabled(enable);
    ui->checkBox->setEnabled(enable);
}

//
void
TControl::setEnableModbus(bool enable) {
    ui->pModbusStart->setEnabled(!enable);
    ui->pModbusStop->setEnabled(enable);
}

//
void
TControl::enableModbus() {
    setEnableModbus(true);
}

//
void
TControl::disableModbus() {
    setEnableModbus(false);
}

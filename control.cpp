#include "control.h"
#include "ui_control.h"
#include <QDebug>

TControl::TControl(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::TControl)
{
    ui->setupUi(this);

    connect(ui->pModbusStop, &QPushButton::pressed,
            this, &TControl::disableModbus);
    connect(ui->pModbusStop, &QPushButton::pressed,
            this, &TControl::modbusStop);

    connect(ui->pModbusStart, &QPushButton::pressed,
            this, &TControl::enableModbus);
    connect(ui->pModbusStart, &QPushButton::pressed,
            this, &TControl::modbusStart);

    ui->led->setEnabled(false);
    ui->led->setCheckable(false);

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
void
TControl::setModbusConnection(bool enable) {
    ui->led->setChecked(enable);
}

//
void TControl::setEnable(bool enable) {
    if (enable) {
//        setEnableModbus(false);
        ui->pModbusStart->pressed();
    } else {
        if (ui->pModbusStop->isEnabled()) {
            ui->pModbusStop->pressed();
        }
        ui->pModbusStart->setEnabled(false);
        ui->pModbusStop->setEnabled(false);
        ui->led->setChecked(false);
        ui->led->setCheckable(false);
    }
}

//
void
TControl::setEnableModbus(bool enable) {
    ui->pModbusStart->setEnabled(!enable);
    ui->pModbusStop->setEnabled(enable);
    ui->led->setChecked(false);
    ui->led->setCheckable(enable);
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



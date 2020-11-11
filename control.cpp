#include "control.h"
#include "ui_control.h"
#include <QDebug>

TControl::TControl(QWidget *parent) :
  QWidget(parent),
  ui(new Ui::TControl)
{
  ui->setupUi(this);

  bspInit();
  modbusInit();
}

TControl::~TControl() {
  delete ui;
}

//
void
TControl::disableModbusSlot() {
  setEnable(false);
}

//
void
TControl::enableModbusSlot() {
  setEnable(true);
}

//
void
TControl::disableBspSlot() {
  ui->cbBsp->setEnabled(true);
}

//
void
TControl::enableBspSlot() {
  ui->cbBsp->setEnabled(false);
}

//
TControl::settings_t
TControl::getBspSettings() const {
  return bspSettings;
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
  }
}

//
void
TControl::setEnableModbus(bool enable) {
  ui->pModbusStart->setEnabled(!enable);
  ui->pModbusStop->setEnabled(enable);
}

//
void
TControl::bspInit() {
  connect(ui->cbBsp, QOverload<int>::of(&QComboBox::currentIndexChanged),
          this, &TControl::changeBsp);

  ui->cbBsp->addItem("BSP");
  ui->cbBsp->addItem("BSP-PI");

  disableBspSlot();
}

//
void
TControl::modbusInit() {
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

//
void
TControl::changeBsp(int index) {
  switch(index) {
    case 0: {
      bspSettings.baud = 4800;
    } break;

    case 1: {
      bspSettings.baud = 19200;
    } break;
  }

  emit bspSettingsChanged();
}



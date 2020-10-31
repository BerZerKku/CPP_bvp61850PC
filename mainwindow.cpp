#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "bvpCommon/param.h"

const uint8_t MainWindow::comReadHoldingRegisters = 0x03;
const uint8_t MainWindow::comWriteMultipleRegisters = 0x10;
const uint8_t MainWindow::comReadWriteMultipleRegisters = 0x17;

//
MainWindow::MainWindow(QWidget *parent)
  : QMainWindow(parent)
  , ui(new Ui::MainWindow)
{
  ui->setupUi(this);

  connect(ui->serial, &TSerial::openPort,
          ui->control, &TControl::enableSlot);

  connect(ui->serial, &TSerial::closePort,
          ui->control, &TControl::disableSlot);

  connect(ui->serial, &TSerial::read, this, &MainWindow::readSlot);
  connect(ui->serial, &TSerial::sendFinished,
          this, &MainWindow::sendFinishedSlot);

  connect(ui->control, &TControl::modbusStart, this, &MainWindow::modbusStart);
  connect(ui->control, &TControl::modbusStop, this, &MainWindow::modbusStop);

  connect(&timer, &QTimer::timeout, this, &MainWindow::modbusProc);
  connect(&timer, &QTimer::timeout, this, &MainWindow::viewReadRegSlot);

  mParam = BVP::TParam::getInstance();
  mParam->setValue(BVP::PARAM_vpBtnSAnSbSac, BVP::SRC_pi, 0);
  mParam->setValue(BVP::PARAM_vpBtnSA32to01, BVP::SRC_pi, 0);
  mParam->setValue(BVP::PARAM_vpBtnSA64to33, BVP::SRC_pi, 0);
  mParam->setValue(BVP::PARAM_blkComPrm32to01, BVP::SRC_pi, 0x50505050);
  mParam->setValue(BVP::PARAM_blkComPrm64to33, BVP::SRC_pi, 0);
  mParam->setValue(BVP::PARAM_blkComPrd32to01, BVP::SRC_pi, 0);
  mParam->setValue(BVP::PARAM_blkComPrd64to33, BVP::SRC_pi, 0);
  mParam->setValue(BVP::PARAM_dirControl, BVP::SRC_pi, BVP::DIR_CONTROL_local);
  mParam->setValue(BVP::PARAM_blkComPrmAll, BVP::SRC_pi, BVP::ON_OFF_on);
  mParam->setValue(BVP::PARAM_blkComPrmDir, BVP::SRC_pi, 0x55);

  setFixedSize(sizeHint());
}

//
MainWindow::~MainWindow() {
  delete ui;
}

//
uint16_t
MainWindow::getUInt16(QVector<uint8_t> &pkg) {
  uint16_t value = pkg.takeFirst();
  value = static_cast<uint16_t> ((value << 8) + pkg.takeFirst());
  return value;
}

//
void
MainWindow::writePkg(QVector<uint8_t> &pkg) {
  for(auto &byte: pkg) {
    ui->serial->write(byte);
  }
}

//
void
MainWindow::modbusStart() {

    mModbus.setBuffer(sBuf, (sizeof(sBuf) / sizeof(sBuf[0])));

    mModbus.setID(BVP::SRC_vkey);
    mModbus.setup(9600, true, 1);
    mModbus.setNetAddress(deviceAddress);
    mModbus.setTimeTick(1000);
    mModbus.setEnable(true);
    timer.start(1);
}

//
void
MainWindow::modbusStop() {
  mModbus.setEnable(false);
  timer.stop();

  ui->readReg->clear();
}

//
void
MainWindow::modbusProc() {
  mModbus.tick();
  if (mModbus.read()) {

  }

  if (mModbus.write()) {
    uint8_t *data = nullptr;
    uint16_t len = mModbus.pop(&data);

    Q_ASSERT(data != nullptr);

    if ((len > 0) && (data != nullptr)) {
      QVector<uint8_t> pkg;
      for(uint16_t i = 0; i < len; i++) {
        pkg.append(data[i]);
      }
      writePkg(pkg);
      qDebug() << pkg;
    }

  }

  ui->control->setModbusConnection(mModbus.isConnection());
}

//
void
MainWindow::sendFinishedSlot() {
  qDebug() << "ok";
  mModbus.sendFinished();
}

//
void
MainWindow::readSlot(int value) {
  quint8 byte = static_cast<uint8_t> (value);
  mModbus.push(byte);
}

//
void
MainWindow::viewReadRegSlot() {
  vpReg::group_t group;
  bool ok = true;
  quint32 val32;
  quint16 value;
  BVP::TParam *params = BVP::TParam::getInstance();

  //
  group = vpReg::GROUP_control;
  val32 = params->getValue(BVP::PARAM_vpBtnSAnSbSac, BVP::SRC_vkey, ok);
  value = static_cast<quint16> (val32);
  ui->readReg->setReg(group, TReadReg::REG_FUNC_BUTTON, value);
  val32 = params->getValue(BVP::PARAM_blkComPrmAll, BVP::SRC_vkey, ok) ? 1 : 0;
  val32 |= params->getValue(BVP::PARAM_dirControl, BVP::SRC_vkey, ok) ? 2 : 0;
  val32 |= params->getValue(BVP::PARAM_blkComPrmDir, BVP::SRC_vkey, ok) << 8;
  value = static_cast<quint16> (val32);
  ui->readReg->setReg(group, TReadReg::REG_FUNC_LED_ENABLE, ~value);
  ui->readReg->setReg(group, TReadReg::REG_FUNC_LED_DISABLE, value);

  //
  group = vpReg::GROUP_com16to01;
  val32 = params->getValue(BVP::PARAM_vpBtnSA32to01, BVP::SRC_vkey, ok);
  value = static_cast<quint16> (val32);
  ui->readReg->setReg(group, TReadReg::REG_FUNC_BUTTON, value);
  val32 = params->getValue(BVP::PARAM_blkComPrm32to01, BVP::SRC_vkey, ok);
  value = static_cast<quint16> (val32);
  ui->readReg->setReg(group, TReadReg::REG_FUNC_LED_ENABLE, ~value);
  ui->readReg->setReg(group, TReadReg::REG_FUNC_LED_DISABLE, value);

  group = vpReg::GROUP_com32to17;
  val32 = params->getValue(BVP::PARAM_vpBtnSA32to01, BVP::SRC_vkey, ok);
  value = static_cast<quint16> (val32 >> 16);
  ui->readReg->setReg(group, TReadReg::REG_FUNC_BUTTON, value);
  val32 = params->getValue(BVP::PARAM_blkComPrm32to01, BVP::SRC_vkey, ok);
  value = static_cast<quint16> (val32 >> 16);
  ui->readReg->setReg(group, TReadReg::REG_FUNC_LED_ENABLE, ~value);
  ui->readReg->setReg(group, TReadReg::REG_FUNC_LED_DISABLE, value);

  group = vpReg::GROUP_com48to33;
  val32 = params->getValue(BVP::PARAM_vpBtnSA64to33, BVP::SRC_vkey, ok);
  value = static_cast<quint16> (val32);
  ui->readReg->setReg(group, TReadReg::REG_FUNC_BUTTON, value);
  val32 = params->getValue(BVP::PARAM_blkComPrm64to33, BVP::SRC_vkey, ok);
  value = static_cast<quint16> (val32);
  ui->readReg->setReg(group, TReadReg::REG_FUNC_LED_ENABLE, ~value);
  ui->readReg->setReg(group, TReadReg::REG_FUNC_LED_DISABLE, value);

  group = vpReg::GROUP_com64to49;
  val32 = params->getValue(BVP::PARAM_vpBtnSA64to33, BVP::SRC_vkey, ok);
  value = static_cast<quint16> (val32 >> 16);
  ui->readReg->setReg(group, TReadReg::REG_FUNC_BUTTON, value);
  val32 = params->getValue(BVP::PARAM_blkComPrm64to33, BVP::SRC_vkey, ok);
  value = static_cast<quint16> (val32 >> 16);
  ui->readReg->setReg(group, TReadReg::REG_FUNC_LED_ENABLE, ~value);
  ui->readReg->setReg(group, TReadReg::REG_FUNC_LED_DISABLE, value);
}


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

  ui->serialVp->setLabelText("Virtual keys panel: ");
  ui->serialVp->addDefaultPort("COM5");
  ui->serialVp->addDefaultPort("tnt0");
  ui->serialVp->setup(9600, QSerialPort::EvenParity, QSerialPort::OneStop);
  connect(ui->serialVp, &TSerial::openPort,
          ui->control, &TControl::enableSlot);

  connect(ui->serialVp, &TSerial::closePort,
          ui->control, &TControl::disableSlot);

  connect(ui->serialVp, &TSerial::read,
          [=](uint32_t value) {mModbus.push(static_cast<uint8_t> (value));});
  connect(ui->serialVp, &TSerial::sendFinished,
          [=](){mModbus.sendFinished();});

  connect(ui->control, &TControl::modbusStart, this, &MainWindow::modbusStart);
  connect(ui->control, &TControl::modbusStop, this, &MainWindow::modbusStop);

  ui->serialPi->setLabelText("BSP-Pi: ");
  ui->serialPi->addDefaultPort("COM20");
  ui->serialPi->addDefaultPort("tnt2");
  ui->serialPi->setup(4800, QSerialPort::NoParity, QSerialPort::TwoStop);
  connect(ui->serialPi, &TSerial::read,
          [=](uint32_t value) {mAvantPi.push(static_cast<uint8_t> (value));});
  connect(ui->serialPi, &TSerial::sendFinished,
          [=](){mAvantPi.sendFinished();});

  connect(ui->serialPi, &TSerial::openPort, this, &MainWindow::avantPiStart);
  connect(ui->serialPi, &TSerial::closePort, this, &MainWindow::avantPiStop);

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

  connect(&timer, &QTimer::timeout, this, &MainWindow::serialProc);
  connect(&timer, &QTimer::timeout, this, &MainWindow::viewReadRegSlot);
  timer.start(1);

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
MainWindow::writePkgVp(QVector<uint8_t> &pkg) {
  for(auto &byte: pkg) {
    ui->serialVp->write(byte);
  }
}

//
void
MainWindow::writePkgPi(QVector<uint8_t> &pkg) {
  qDebug() << "avantPi: " << showbase << hex << pkg;

  for(auto &byte: pkg) {
    ui->serialPi->write(byte);
  }
}

//
void
MainWindow::modbusStart() {

    mModbus.setBuffer(bufAvantPi, (sizeof(bufAvantPi) / sizeof(bufAvantPi[0])));

    mModbus.setID(BVP::SRC_vkey);
    mModbus.setup(9600, true, 1);
    mModbus.setNetAddress(deviceAddress);
    mModbus.setTimeTick(1000);
    mModbus.setEnable(true);
}

//
void
MainWindow::modbusStop() {
  mModbus.setEnable(false);

  ui->readReg->clear();
}

//
void
MainWindow::serialProc() {
  if (mModbus.isEnable()) {
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
        writePkgVp(pkg);
//        qDebug() << pkg;
      }
    }
  }

  if (mAvantPi.isEnable()) {
    mAvantPi.tick();
    if (mAvantPi.read()) {

    }

    if (mAvantPi.write()) {
      uint8_t *data = nullptr;
      uint16_t len = mAvantPi.pop(&data);

      Q_ASSERT(data != nullptr);

      if ((len > 0) && (data != nullptr)) {
        QVector<uint8_t> pkg;
        for(uint16_t i = 0; i < len; i++) {
          pkg.append(data[i]);
        }
        writePkgPi(pkg);
      }
    }
  }

  ui->control->setModbusConnection(mModbus.isConnection());
}

//
void
MainWindow::avantPiStart() {
  mAvantPi.setBuffer(bufAvantPi, (sizeof(bufAvantPi) / sizeof(bufAvantPi[0])));

  mAvantPi.setID(BVP::SRC_pi);
  mAvantPi.setup(4800, false, 2);
  mAvantPi.setNetAddress(1);
  mAvantPi.setTimeTick(1000);
  mAvantPi.setEnable(true);
}

//
void
MainWindow::avantPiStop() {
  mAvantPi.setEnable(false);
}

//
void
MainWindow::viewReadRegSlot() {
  vpReg::group_t group;
  bool ok = true;
  quint32 val32;
  quint16 value;
  BVP::param_t param = BVP::PARAM_MAX;
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

  param = BVP::PARAM_control;
}


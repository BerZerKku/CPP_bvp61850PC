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
  setWindowTitle("bvp61850 STM32");

  initVp();
  initAvantPi();
  initAvantPc();

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

  connect(&timer1ms, &QTimer::timeout, this, &MainWindow::serialProc);
  timer1ms.start(1);

  connect(&timer100ms, &QTimer::timeout, this, &MainWindow::viewReadRegSlot);
  connect(&timer100ms, &QTimer::timeout,
          ui->paramTree, &TParamTree::updateParameters);
  timer100ms.start(100);

  connect(ui->control, &TControl::bspSettingsChanged,
          this, &MainWindow::setBspPiCfg);

  connect(ui->serialPi, &TSerial::openPort, ui->control, &TControl::enableBspSlot);
  connect(ui->serialPi, &TSerial::closePort, ui->control, &TControl::disableBspSlot);

  setBspPiCfg();

  setFixedSize(sizeHint());
}

//
MainWindow::~MainWindow()
{
  delete ui;
}

//
void MainWindow::initAvantPc()
{
  serialCfg_t *cfg = new serialCfg_t;

  cfg->label = "PC";
  cfg->defaultPorts.append("COM30");
  cfg->defaultPorts.append("tnt4");
  cfg->protocol = new BVP::TAvantPc(BVP::TSerialProtocol::REGIME_slave);
  cfg->srcId = BVP::SRC_pc;
  cfg->netAddr = 1;
  cfg->baudrate = 19200;
  cfg->parity = QSerialPort::NoParity;
  cfg->stopBits = QSerialPort::TwoStop;

  initSerial(ui->serialPc, cfg);
}


//
void MainWindow::initAvantPi()
{
  serialCfg_t *cfg = new serialCfg_t;

  cfg->label = "BSP-PI";
  cfg->defaultPorts.append("COM20");
  cfg->defaultPorts.append("tnt0");
  cfg->protocol = new BVP::TAvantPi(BVP::TSerialProtocol::REGIME_master);
  cfg->srcId = BVP::SRC_pi;
  cfg->netAddr = 1;
  cfg->baudrate = 19200;
  cfg->parity = QSerialPort::NoParity;
  cfg->stopBits = QSerialPort::TwoStop;

  initSerial(ui->serialPi, cfg);
}


//
void MainWindow::initVp()
{
  serialCfg_t *cfg = new serialCfg_t;

  cfg->label = "Virtual keys panel";
  cfg->defaultPorts.append("COM5");
  cfg->defaultPorts.append("tnt2");
  cfg->protocol = new BVP::TModbusVp(BVP::TModbusVp::REGIME_master);
  cfg->srcId = BVP::SRC_vkey;
  cfg->netAddr = 10;
  cfg->baudrate = 9600;
  cfg->parity = QSerialPort::EvenParity;
  cfg->stopBits = QSerialPort::OneStop;

  initSerial(ui->serialVp, cfg);
}


//
void MainWindow::initSerial(TSerial *serial, MainWindow::serialCfg_t *cfg)
{
  sPort.insert(serial, cfg);

  serial->setLabelText(cfg->label + ":");

  for(auto& port: cfg->defaultPorts) {
    serial->addDefaultPort(port);
  }

  serial->setup(cfg->baudrate, cfg->parity, cfg->stopBits);

  connect(serial, &TSerial::read,
          [=](uint32_t value) {cfg->protocol->push(static_cast<uint8_t> (value));});
  connect(serial, &TSerial::sendFinished,
          [=](){cfg->protocol->sendFinished();});

  connect(serial, &TSerial::openPort, this,
          [=]() {protocolStart(cfg);});
  connect(serial, &TSerial::closePort, this,
          [=]() {protocolStop(sPort.value(serial));});
}


//
uint16_t MainWindow::getUInt16(QVector<uint8_t> &pkg)
{
  uint16_t value = pkg.takeFirst();
  value = static_cast<uint16_t> ((value << 8) + pkg.takeFirst());
  return value;
}


//
void MainWindow::protocolStart(MainWindow::serialCfg_t *cfg)
{
  cfg->protocol->setBuffer(cfg->buf, std::end(cfg->buf) - std::begin(cfg->buf));
  cfg->protocol->setID(cfg->srcId);

  Q_ASSERT(cfg->protocol->setup(cfg->baudrate,
                               cfg->parity != QSerialPort::NoParity,
                               cfg->stopBits));
  Q_ASSERT(cfg->protocol->setNetAddress(cfg->netAddr));
  Q_ASSERT(cfg->protocol->setTimeTick(1000));
  Q_ASSERT(cfg->protocol->setEnable(true));
}


//
void MainWindow::protocolStop(MainWindow::serialCfg_t *cfg)
{
  Q_ASSERT(!cfg->protocol->setEnable(false));
}


//
void MainWindow::setBspPiCfg()
{
  Q_ASSERT(sPort.count(ui->serialPi) ==1);

  serialCfg_t *cfg = sPort.value(ui->serialPi);
  TControl::settings_t settings = ui->control->getBspSettings();

  cfg->baudrate = settings.baud;
  cfg->parity = settings.parity;
  cfg->stopBits = settings.stopBits;
}


//
void MainWindow::serialProc() {
  for(auto& serial: sPort.keys()) {
    serialCfg_t *cfg = sPort.value(serial);

    if (cfg->protocol->isEnable()) {
      cfg->protocol->tick();
      cfg->protocol->read();

      if (cfg->protocol->write()) {
        uint8_t *data = nullptr;
        uint16_t len = cfg->protocol->pop(&data);

        Q_ASSERT(data != nullptr);

        if ((len > 0) && (data != nullptr)) {
          QVector<uint8_t> pkg;
          for(uint16_t i = 0; i < len; i++) {
            pkg.append(data[i]);
          }

//          if (serial == ui->serialVp) {
//            qDebug() << "Tx to VP: " << Qt::hex << pkg << Qt::endl;
//          }

          for(auto &byte: pkg) {
            serial->write(byte);
          }
        }
      }
    }

    serial->setLedLink(cfg->protocol->isConnection());
  }
}


//
void MainWindow::viewReadRegSlot() {
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

#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "bvpCommon/clock.hpp"
#include "bvpCommon/param.h"
#include "bvpCommon/bvpBase.hpp"

const uint8_t MainWindow::comReadHoldingRegisters = 0x03;
const uint8_t MainWindow::comWriteMultipleRegisters = 0x10;
const uint8_t MainWindow::comReadWriteMultipleRegisters = 0x17;

static TAlarm *wAlarm = nullptr;
static MainWindow *mw = nullptr;

using namespace BVP;

bool getExtAlarmSignals(BVP::extAlarm_t signal) {
    Q_ASSERT(wAlarm != nullptr);

    return wAlarm->getSignal(signal);
}

void setExtAlarmSignal(BVP::extAlarm_t signal, bool value) {
    Q_ASSERT(wAlarm != nullptr);

    wAlarm->setSignal(signal, value);
}

void sendDataToSerial(BVP::src_t src, uint16_t len, const uint8_t *data)
{
    Q_ASSERT(mw != nullptr);

    if (mw->mSerial.count(src) != 0) {
        QVector<uint8_t> pkg;
        while(len > 0) {
            uint8_t byte = *data++;
            pkg.append(byte);
            emit mw->mSerial.value(src)->write(byte);
            len--;
        }
    }
}

//
MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    setWindowTitle("bvp61850 STM32");

    QTextCodec::setCodecForLocale(QTextCodec::codecForName("Windows-1251"));

    mw = this;
    wAlarm = ui->fAlarm; // const_cast<MainWindow*> (this);

    initVp();
    initAvantPi();
    initAvantPc();    

    connect(&timer1ms, &QTimer::timeout, this, &MainWindow::serialProc);
    connect(&timer1ms, &QTimer::timeout, [this]() {mBvpBase.tick1ms();});
    connect(&timer1ms, &QTimer::timeout, []() {TClock::tick();});
    mBvpBase.protSetTick(1000);
    connect(&timer1ms, &QTimer::timeout, [this]() {mBvpBase.protTick();});
    TClock::setTickInMs(1);
    timer1ms.start(1);

    connect(&timer100ms, &QTimer::timeout, this, &MainWindow::viewReadRegSlot);
    connect(&timer100ms, &QTimer::timeout,
            ui->paramTree, &TParamTree::updateParameters);
    connect(&timer100ms, &QTimer::timeout, this, &MainWindow::serialProc);

    mBvpBase.init();
    connect(&timer100ms, &QTimer::timeout, [this]() {mBvpBase.loop();});

    timer100ms.start(100);

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
    TSerial *serial = new TSerial;
    serialCfg_t *cfg = new serialCfg_t;

    cfg->label = "PC";
    cfg->defaultPorts.append("COM30");
    cfg->defaultPorts.append("tnt3");
    cfg->baudList.append({19200});
    cfg->parityList.append({QSerialPort::NoParity});
    cfg->stopList.append({QSerialPort::TwoStop});
    cfg->srcId = SRC_pc;
    cfg->baudrate = 19200;
    cfg->parity = QSerialPort::NoParity;
    cfg->stopBits = QSerialPort::TwoStop;

    initSerial(serial, cfg);
    addSerialToFrame(serial);
}


//
void MainWindow::initAvantPi()
{
    TSerial *serial = new TSerial;
    serialCfg_t *cfg = new serialCfg_t;

    cfg->label = "BSP-PI";
    cfg->defaultPorts.append("COM6"); // COM20, COM6,
    cfg->defaultPorts.append("COM3"); //
    cfg->defaultPorts.append("tnt3");
    cfg->baudList.append({4800, 19200});
    cfg->parityList.append({QSerialPort::NoParity});
    cfg->stopList.append({QSerialPort::TwoStop});
    cfg->srcId = SRC_pi;
    cfg->baudrate = 19200;
    cfg->parity = QSerialPort::NoParity;
    cfg->stopBits = QSerialPort::TwoStop;

    Q_ASSERT(serialPi == nullptr);
    serialPi = serial;

    initSerial(serial, cfg);
    addSerialToFrame(serial);
}


//
void MainWindow::initVp()
{
    TSerial *serial = new TSerial;
    serialCfg_t *cfg = new serialCfg_t;

    cfg->label = "Virtual keys panel";
    cfg->defaultPorts.append("COM5");
    cfg->defaultPorts.append("tnt5");
    cfg->baudList.append({9600});
    cfg->parityList.append({QSerialPort::EvenParity});
    cfg->stopList.append({QSerialPort::OneStop});
    cfg->srcId = SRC_vkey;
    cfg->baudrate = 9600;
    cfg->parity = QSerialPort::EvenParity;
    cfg->stopBits = QSerialPort::OneStop;

    initSerial(serial, cfg);
    addSerialToFrame(serial);
}


//
void MainWindow::initSerial(TSerial *serial, MainWindow::serialCfg_t *cfg)
{
    sPort.insert(serial, cfg);
    mSerial.insert(cfg->srcId, serial);

    serial->setLabelText(cfg->label + ":");

    for(auto& port: cfg->defaultPorts) {
        serial->addDefaultPort(port);
    }

    serial->setBaudRateList(cfg->baudList);
    serial->setParityList(cfg->parityList);
    serial->setStopBitList(cfg->stopList);

    serial->setup(cfg->baudrate, cfg->parity, cfg->stopBits);

    connect(serial, &TSerial::read,
            [=](uint32_t value) {mBvpBase.protPush(cfg->srcId, uint8_t(value));});

    connect(serial, &TSerial::sendFinished,
            [=](){mBvpBase.protSendFinished(cfg->srcId);});

    connect(serial, &TSerial::openPort, [=]() {serialStart(serial);});
    connect(serial, &TSerial::closePort, [=]() {serialStop(serial);});
}

void MainWindow::addSerialToFrame(TSerial *serial)
{
    ui->serialFrame->layout()->addWidget(serial);
}


//
uint16_t MainWindow::getUInt16(QVector<uint8_t> &pkg)
{
    uint16_t value = pkg.takeFirst();
    value = static_cast<uint16_t> ((value << 8) + pkg.takeFirst());
    return value;
}

//
void MainWindow::serialStart(TSerial *serial)
{
    Q_ASSERT(sPort.count(serial) == 1);
    serialCfg_t *cfg = sPort.value(serial);

//    cfg->baudrate = serial->getBaudRate();
//    cfg->parity = serial->getParity();
//    cfg->stopBits = serial->getStopBits();
}


//
void MainWindow::serialStop(TSerial *serial)
{
    Q_ASSERT(sPort.count(serial) == 1);

//    serialCfg_t *cfg = sPort.value(serial);

//    Q_ASSERT(!cfg->protocol->setEnable(false));
}


//
void MainWindow::serialProc() {
    for(src_t src = src_t(0); src < src_t::SRC_MAX; src = src_t(src + 1)) {
        if (mSerial.count(src)) {
            mSerial.value(src)->setLedLink(mBvpBase.protIsConnection(src));
        }
    }
}

//
void MainWindow::viewReadRegSlot() {
//    vpReg::group_t group;
//    bool ok = true;
//    quint32 val32;
//    quint16 value;
//    TParam *params = TParam::getInstance();

//    //
//    group = vpReg::GROUP_control;
//    val32 = params->getValue(PARAM_vpBtnSAnSbSac, SRC_vkey, ok);
//    value = static_cast<quint16> (val32);
//    ui->readReg->setReg(group, TReadReg::REG_FUNC_BUTTON, value);
//    val32 = params->getValue(PARAM_blkComPrmAll, SRC_vkey, ok) ? 0 : 1;
//    val32 |= params->getValue(PARAM_dirControl, SRC_vkey, ok) ? 2 : 0;
//    val32 |= params->getValue(PARAM_blkComPrmDir, SRC_vkey, ok) << 8;
//    value = static_cast<quint16> (val32);
//    ui->readReg->setReg(group, TReadReg::REG_FUNC_LED_ENABLE, ~value);
//    ui->readReg->setReg(group, TReadReg::REG_FUNC_LED_DISABLE, value);

//    //
//    // Переключатели с 16 по 1
//    //
//    group = vpReg::GROUP_com16to01;
//    // FIXME Для Казань MPLSTP сделана блокировка команд передатчика
//    param_t p1 = PARAM_comPrdBlk08to01; // PARAM_comPrdBlk08to01;
//    param_t p2 = PARAM_comPrdBlk16to09; // PARAM_comPrmBlk16to09;

//    val32 = params->getValue(PARAM_vpBtnSA32to01, SRC_vkey, ok);
//    value = static_cast<quint16> (val32);
//    ui->readReg->setReg(group, TReadReg::REG_FUNC_BUTTON, value);

//    value = mModbus->getSwitchLed(p2, p1, ON_OFF_off);
//    ui->readReg->setReg(group, TReadReg::REG_FUNC_LED_ENABLE, value);

//    value = mModbus->getSwitchLed(p2, p1, ON_OFF_on);
//    ui->readReg->setReg(group, TReadReg::REG_FUNC_LED_DISABLE, value);

//    //
//    // Переключатели с 32 по 17
//    //
//    group = vpReg::GROUP_com32to17;
//    // FIXME Для Казань MPLSTP сделана блокировка команд передатчика
//    p1 = PARAM_comPrmBlk08to01; // PARAM_comPrmBlk24to17;
//    p2 = PARAM_comPrmBlk16to09; // PARAM_comPrmBlk32to25;

//    val32 = params->getValue(PARAM_vpBtnSA32to01, SRC_vkey, ok);
//    value = static_cast<quint16> (val32 >> 16);
//    ui->readReg->setReg(group, TReadReg::REG_FUNC_BUTTON, value);

//    value = mModbus->getSwitchLed(p2, p1, ON_OFF_off);
//    ui->readReg->setReg(group, TReadReg::REG_FUNC_LED_ENABLE, value);

//    value = mModbus->getSwitchLed(p2, p1, ON_OFF_on);
//    ui->readReg->setReg(group, TReadReg::REG_FUNC_LED_DISABLE, value);
}

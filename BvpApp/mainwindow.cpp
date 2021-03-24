#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "bvpCommon/clock.hpp"
#include "bvpCommon/param.h"

const uint8_t MainWindow::comReadHoldingRegisters = 0x03;
const uint8_t MainWindow::comWriteMultipleRegisters = 0x10;
const uint8_t MainWindow::comReadWriteMultipleRegisters = 0x17;

static TAlarm *wAlarm = nullptr;

using namespace BVP;

bool getExtAlarmSignals(extAlarm_t signal) {
    Q_ASSERT(wAlarm != nullptr);

    return wAlarm->getSignal(signal);
}

void setExtAlarmSignal(extAlarm_t signal, bool value) {
    Q_ASSERT(wAlarm != nullptr);

    wAlarm->setSignal(signal, value);
}

//
MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    setWindowTitle("bvp61850 STM32");

    QTextCodec::setCodecForLocale(QTextCodec::codecForName("Windows-1251"));

    wAlarm = ui->fAlarm; // const_cast<MainWindow*> (this);

    mModbus = new TModbusVp(TModbusVp::REGIME_master);
    mParam = TParam::getInstance();

    initVp();
    initAvantPi();
    initAvantPc();    

    TClock::setTickInMs(1);
    connect(&timer1ms, &QTimer::timeout, this, &MainWindow::serialProc);
    connect(&timer1ms, &QTimer::timeout, []() {TClock::tick();});
    timer1ms.start(1);

    connect(&timer100ms, &QTimer::timeout, this, &MainWindow::viewReadRegSlot);
    connect(&timer100ms, &QTimer::timeout,
            ui->paramTree, &TParamTree::updateParameters);
    connect(&timer100ms, &QTimer::timeout, this, &MainWindow::alarmLoop);

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
    cfg->protocol = new TAvantPc(TSerialProtocol::REGIME_slave);
    cfg->srcId = SRC_pc;
    cfg->netAddr = 1;
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
    cfg->protocol = new TAvantPi(TSerialProtocol::REGIME_master);
    cfg->srcId = SRC_pi;
    cfg->netAddr = 1;
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
    cfg->protocol = mModbus;
    cfg->srcId = SRC_vkey;
    cfg->netAddr = 10;
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

    serial->setLabelText(cfg->label + ":");

    for(auto& port: cfg->defaultPorts) {
        serial->addDefaultPort(port);
    }

    serial->setBaudRateList(cfg->baudList);
    serial->setParityList(cfg->parityList);
    serial->setStopBitList(cfg->stopList);

    serial->setup(cfg->baudrate, cfg->parity, cfg->stopBits);

    connect(serial, &TSerial::read,
            [=](uint32_t value) {cfg->protocol->push(static_cast<uint8_t> (value));});
    connect(serial, &TSerial::sendFinished,
            [=](){cfg->protocol->sendFinished();});

    connect(serial, &TSerial::openPort, this,
            [=]() {serialStart(serial);});
    connect(serial, &TSerial::closePort, this,
            [=]() {serialStop(serial);});
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

    cfg->baudrate = serial->getBaudRate();
    cfg->parity = serial->getParity();
    cfg->stopBits = serial->getStopBits();

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
void MainWindow::serialStop(TSerial *serial)
{
    Q_ASSERT(sPort.count(serial) == 1);

    serialCfg_t *cfg = sPort.value(serial);

    Q_ASSERT(!cfg->protocol->setEnable(false));
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

//                    if (cfg->protocol->getID() == SRC_pi) {
//                        qDebug() << "Tx to PI: " << Qt::hex << pkg << Qt::endl;
//                    }

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
void MainWindow::alarmLoop()
{
    const src_t src = SRC_int;
    bool ok;
    uint32_t uval32;

    alarmResetLoop();

    // Обработка входных и установка выходных сигналов

    uval32 = mParam->getValue(PARAM_alarmResetMode, src, ok);
    if (ok) {
        mAlarm.setAlarmReset(alarmReset_t(uval32));
    }

    for(uint8_t i = 0; i < EXT_ALARM_MAX; i++) {
        bool value;
        extAlarm_t signal = static_cast<extAlarm_t> (i);

        if (signal == EXT_ALARM_disablePrm) {
            uval32 = mParam->getValue(PARAM_blkComPrmAll,
                                      SRC_int, ok);
            if (!ok) {
                uval32 = DISABLE_PRM_enable;
            }

            value = (uval32 == DISABLE_PRM_disable);
        } else {
            value = getExtAlarmSignals(signal);
        }

        mAlarm.setAlarmInputSignal(signal, value);
    }

    for(uint8_t i = 0; i < EXT_ALARM_MAX; i++) {
        extAlarm_t signal = static_cast<extAlarm_t> (i);

        bool value = mAlarm.getAlarmOutputSignal(signal);
        setExtAlarmSignal(signal, value);
    }
}

void MainWindow::alarmResetLoop()
{
    static clockPoint_t last = TClock::getClockPoint();
    const src_t src = SRC_int;
    bool ok = true;
    uint32_t uval32;

    // Обработка нажатия кнопки сброса

    uval32 = mParam->getValue(PARAM_alarmRstCtrl, src, ok);
    if ((ok) && (uval32 != ALARM_RST_CTRL_no)) {
        if (uval32 == ALARM_RST_CTRL_pressed) {
            uval32 = mParam->getValue(PARAM_control, src, ok);
            if (!ok) {
                uval32 = 0;
            }

            uval32 |= (1 << CTRL_resetComInd);
            if (mAlarm.isSignalForDeviceReset()) {
                qDebug() << "Reset signal";
                uval32 |= (1 << CTRL_resetFault);
            }

            mParam->setValue(PARAM_control, src, uval32);

            // Сброс сигнализации и начало отсчета времени
            mAlarm.reset(true);
            last = TClock::getClockPoint();
        }
        uval32 = ALARM_RST_CTRL_no;
    }
    mParam->setValue(PARAM_alarmRstCtrl, src, uval32);

    // Отключение сброса сигнализации, если прошло заданное время.
    if (mAlarm.isReset() && (TClock::getDurationS(last) >= 2)) {
        mAlarm.reset(false);
    }
}

//
void MainWindow::viewReadRegSlot() {
    vpReg::group_t group;
    bool ok = true;
    quint32 val32;
    quint16 value;
    TParam *params = TParam::getInstance();

    //
    group = vpReg::GROUP_control;
    val32 = params->getValue(PARAM_vpBtnSAnSbSac, SRC_vkey, ok);
    value = static_cast<quint16> (val32);
    ui->readReg->setReg(group, TReadReg::REG_FUNC_BUTTON, value);
    val32 = params->getValue(PARAM_blkComPrmAll, SRC_vkey, ok) ? 0 : 1;
    val32 |= params->getValue(PARAM_dirControl, SRC_vkey, ok) ? 2 : 0;
    val32 |= params->getValue(PARAM_blkComPrmDir, SRC_vkey, ok) << 8;
    value = static_cast<quint16> (val32);
    ui->readReg->setReg(group, TReadReg::REG_FUNC_LED_ENABLE, ~value);
    ui->readReg->setReg(group, TReadReg::REG_FUNC_LED_DISABLE, value);

    //
    // Переключатели с 16 по 1
    //
    group = vpReg::GROUP_com16to01;
    // FIXME Для Казань MPLSTP сделана блокировка команд передатчика
    param_t p1 = PARAM_comPrdBlk08to01; // PARAM_comPrdBlk08to01;
    param_t p2 = PARAM_comPrdBlk16to09; // PARAM_comPrmBlk16to09;

    val32 = params->getValue(PARAM_vpBtnSA32to01, SRC_vkey, ok);
    value = static_cast<quint16> (val32);
    ui->readReg->setReg(group, TReadReg::REG_FUNC_BUTTON, value);

    value = mModbus->getSwitchLed(p2, p1, ON_OFF_off);
    ui->readReg->setReg(group, TReadReg::REG_FUNC_LED_ENABLE, value);

    value = mModbus->getSwitchLed(p2, p1, ON_OFF_on);
    ui->readReg->setReg(group, TReadReg::REG_FUNC_LED_DISABLE, value);

    //
    // Переключатели с 32 по 17
    //
    group = vpReg::GROUP_com32to17;
    // FIXME Для Казань MPLSTP сделана блокировка команд передатчика
    p1 = PARAM_comPrmBlk08to01; // PARAM_comPrmBlk24to17;
    p2 = PARAM_comPrmBlk16to09; // PARAM_comPrmBlk32to25;

    val32 = params->getValue(PARAM_vpBtnSA32to01, SRC_vkey, ok);
    value = static_cast<quint16> (val32 >> 16);
    ui->readReg->setReg(group, TReadReg::REG_FUNC_BUTTON, value);

    value = mModbus->getSwitchLed(p2, p1, ON_OFF_off);
    ui->readReg->setReg(group, TReadReg::REG_FUNC_LED_ENABLE, value);

    value = mModbus->getSwitchLed(p2, p1, ON_OFF_on);
    ui->readReg->setReg(group, TReadReg::REG_FUNC_LED_DISABLE, value);
}

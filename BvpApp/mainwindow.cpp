#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "bvpCommon/param.h"

const uint8_t MainWindow::comReadHoldingRegisters = 0x03;
const uint8_t MainWindow::comWriteMultipleRegisters = 0x10;
const uint8_t MainWindow::comReadWriteMultipleRegisters = 0x17;

static TAlarm *wAlarm = nullptr;

bool getExtAlarmSignals(BVP::extAlarm_t signal) {
    Q_ASSERT(wAlarm != nullptr);

    return wAlarm->getSignal(signal);
}

void setExtAlarmSignal(BVP::extAlarm_t signal, bool value) {
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

    mModbus = new BVP::TModbusVp(BVP::TModbusVp::REGIME_master);
    mParam = BVP::TParam::getInstance();

    initVp();
    initAvantPi();
    initAvantPc();    

    connect(&timer1ms, &QTimer::timeout, this, &MainWindow::serialProc);
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
    cfg->protocol = new BVP::TAvantPc(BVP::TSerialProtocol::REGIME_slave);
    cfg->srcId = BVP::SRC_pc;
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
    cfg->protocol = new BVP::TAvantPi(BVP::TSerialProtocol::REGIME_master);
    cfg->srcId = BVP::SRC_pi;
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
    cfg->srcId = BVP::SRC_vkey;
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

//                    if (cfg->protocol->getID() == BVP::SRC_pi) {
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
    const BVP::src_t src = BVP::SRC_int;
    bool ok;
    uint32_t uval32;

    alarmResetLoop();

    // Обработка входных и установка выходных сигналов

    uval32 = mParam->getValue(BVP::PARAM_alarmResetMode, src, ok);
    if (!ok || (uval32 >= BVP::ALARM_RESET_MAX)) {
        uval32 = mAlarm.kAlarmResetModeDefault;
    }
    mAlarm.setAlarmReset(BVP::alarmReset_t(uval32));

    for(uint8_t i = 0; i < BVP::EXT_ALARM_MAX; i++) {
        bool value;
        BVP::extAlarm_t signal = static_cast<BVP::extAlarm_t> (i);

        if (signal == BVP::EXT_ALARM_disablePrm) {
            uval32 = mParam->getValue(BVP::PARAM_blkComPrmAll,
                                      BVP::SRC_int, ok);
            if (!ok) {
                uval32 = mAlarm.kDisablePrmDefault;
            }

            value = (uval32 == BVP::DISABLE_PRM_disable);
        } else {
            value = getExtAlarmSignals(signal);
        }

        mAlarm.setAlarmInputSignal(signal, value);
    }

    for(uint8_t i = 0; i < BVP::EXT_ALARM_MAX; i++) {
        BVP::extAlarm_t signal = static_cast<BVP::extAlarm_t> (i);

        bool value = mAlarm.getAlarmOutputSignal(signal);

        setExtAlarmSignal(signal, value);
    }
}

void MainWindow::alarmResetLoop()
{
    const BVP::src_t src = BVP::SRC_int;
    bool ok;
    uint32_t uval32;

    // Обработка нажатия кнопки сброса

    uval32 = mParam->getValue(BVP::PARAM_alarmRstCtrl, src, ok);
    uval32 &= (1 << BVP::ALARM_RST_CTRL_MAX) - 1;
    if (ok) {
        BVP::alarmRstCtrl_t i = static_cast<BVP::alarmRstCtrl_t> (0);
        uint32_t tval = uval32;
        while((uval32 > 0) && (i < BVP::ALARM_RST_CTRL_MAX)) {
            if (uval32 & (1 << i)) {
                switch(i) {
                    case BVP::ALARM_RST_CTRL_resetIndWait: break;

                    case BVP::ALARM_RST_CTRL_pressed: {
                        mParam->setValue(BVP::PARAM_alarmRstCtrl, src, false);

                        uval32 = mParam->getValue(BVP::PARAM_control, src, ok);
                        if (!ok) {
                            uval32 = 0;
                        }
                        uval32 |= (1 << BVP::CTRL_resetComInd);
                        tval |= (1 << BVP::ALARM_RST_CTRL_resetIndWait);

                        if (mAlarm.getAlarmOutputSignal(BVP::EXT_ALARM_fault) ||
                            mAlarm.getAlarmOutputSignal(BVP::EXT_ALARM_warning)) {
                            uval32 |= (1 << BVP::CTRL_resetFault);
                        }

                        mParam->setValue(BVP::PARAM_control, src, uval32);
                    } break;
                    case BVP::ALARM_RST_CTRL_resetInd: {
                        if (tval & (1 << BVP::ALARM_RST_CTRL_resetIndWait)) {
                            mAlarm.resetSignal(BVP::EXT_ALARM_comPrd);
                            mAlarm.resetSignal(BVP::EXT_ALARM_comPrm);
                            tval &= ~(1 << BVP::ALARM_RST_CTRL_resetIndWait);
                        }
                        tval &= ~(1 << i);
                    } break;
                    case BVP::ALARM_RST_CTRL_device: {
                        // TODO
                    } break;

                    case BVP::ALARM_RST_CTRL_MAX: break;
                }
                uval32 &= ~(1 << BVP::ALARM_RST_CTRL_resetInd);
            }
        }
        uval32 = tval;
    }
    mParam->setValue(BVP::PARAM_alarmRstCtrl, src, uval32);

    if ((ok) && (uval32 != false)) {

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
    val32 = params->getValue(BVP::PARAM_blkComPrmAll, BVP::SRC_vkey, ok) ? 0 : 1;
    val32 |= params->getValue(BVP::PARAM_dirControl, BVP::SRC_vkey, ok) ? 2 : 0;
    val32 |= params->getValue(BVP::PARAM_blkComPrmDir, BVP::SRC_vkey, ok) << 8;
    value = static_cast<quint16> (val32);
    ui->readReg->setReg(group, TReadReg::REG_FUNC_LED_ENABLE, ~value);
    ui->readReg->setReg(group, TReadReg::REG_FUNC_LED_DISABLE, value);

    //
    // Переключатели с 16 по 1
    //
    group = vpReg::GROUP_com16to01;
    // FIXME Для Казань MPLSTP сделана блокировка команд передатчика
    BVP::param_t p1 = BVP::PARAM_comPrdBlk08to01; // BVP::PARAM_comPrdBlk08to01;
    BVP::param_t p2 = BVP::PARAM_comPrdBlk16to09; // BVP::PARAM_comPrmBlk16to09;

    val32 = params->getValue(BVP::PARAM_vpBtnSA32to01, BVP::SRC_vkey, ok);
    value = static_cast<quint16> (val32);
    ui->readReg->setReg(group, TReadReg::REG_FUNC_BUTTON, value);

    value = mModbus->getSwitchLed(p2, p1, BVP::ON_OFF_off);
    ui->readReg->setReg(group, TReadReg::REG_FUNC_LED_ENABLE, value);

    value = mModbus->getSwitchLed(p2, p1, BVP::ON_OFF_on);
    ui->readReg->setReg(group, TReadReg::REG_FUNC_LED_DISABLE, value);

    //
    // Переключатели с 32 по 17
    //
    group = vpReg::GROUP_com32to17;
    // FIXME Для Казань MPLSTP сделана блокировка команд передатчика
    p1 = BVP::PARAM_comPrmBlk08to01; // BVP::PARAM_comPrmBlk24to17;
    p2 = BVP::PARAM_comPrmBlk16to09; // BVP::PARAM_comPrmBlk32to25;

    val32 = params->getValue(BVP::PARAM_vpBtnSA32to01, BVP::SRC_vkey, ok);
    value = static_cast<quint16> (val32 >> 16);
    ui->readReg->setReg(group, TReadReg::REG_FUNC_BUTTON, value);

    value = mModbus->getSwitchLed(p2, p1, BVP::ON_OFF_off);
    ui->readReg->setReg(group, TReadReg::REG_FUNC_LED_ENABLE, value);

    value = mModbus->getSwitchLed(p2, p1, BVP::ON_OFF_on);
    ui->readReg->setReg(group, TReadReg::REG_FUNC_LED_DISABLE, value);
}

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

    wAlarm = ui->fAlarm; // const_cast<MainWindow*> (this);

    initVp();
    initAvantPi();
    initAvantPc();

    mParam = BVP::TParam::getInstance();

    // FIXME Без установки этих параметров нет связи с Панелью ВК
//    mParam->setValue(BVP::PARAM_vpBtnSAnSbSac, BVP::SRC_pi, 0);
//    mParam->setValue(BVP::PARAM_vpBtnSA32to01, BVP::SRC_pi, 0);
//    mParam->setValue(BVP::PARAM_vpBtnSA64to33, BVP::SRC_pi, 0);
    mParam->setValue(BVP::PARAM_blkComPrm32to01, BVP::SRC_pi, 0x50505050);
//    mParam->setValue(BVP::PARAM_blkComPrm64to33, BVP::SRC_pi, 0);
//    mParam->setValue(BVP::PARAM_blkComPrd32to01, BVP::SRC_pi, 0);
//    mParam->setValue(BVP::PARAM_blkComPrd64to33, BVP::SRC_pi, 0);
    mParam->setValue(BVP::PARAM_dirControl, BVP::SRC_pi, BVP::DIR_CONTROL_local);
    mParam->setValue(BVP::PARAM_blkComPrmAll, BVP::SRC_pi, BVP::ON_OFF_off);
    mParam->setValue(BVP::PARAM_blkComPrmDir, BVP::SRC_pi, 0x55);

    connect(&timer1ms, &QTimer::timeout, this, &MainWindow::serialProc);
    timer1ms.start(1);

    connect(&timer100ms, &QTimer::timeout, this, &MainWindow::viewReadRegSlot);
    connect(&timer100ms, &QTimer::timeout,
            ui->paramTree, &TParamTree::updateParameters);
    connect(&timer100ms, &QTimer::timeout, this, &MainWindow::procAlarm);

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
    cfg->defaultPorts.append("tnt4");
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
    cfg->defaultPorts.append("COM20");
    cfg->defaultPorts.append("tnt0");
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
    cfg->defaultPorts.append("tnt2");
    cfg->baudList.append({9600});
    cfg->parityList.append({QSerialPort::EvenParity});
    cfg->stopList.append({QSerialPort::OneStop});
    cfg->protocol = new BVP::TModbusVp(BVP::TModbusVp::REGIME_master);
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
void MainWindow::procAlarm()
{
    static bool resetbtn = false;
    bool ok;
    uint32_t uval32;

    uval32 = mParam->getValue(BVP::PARAM_vpBtnSAnSbSac, BVP::SRC_int, ok);
    if (ok) {
        bool tresetbtn = (uval32 & BVP::VP_BTN_CONTROL_sb) > 0;

        // FIXME Не работает!

        if (!resetbtn && tresetbtn) {
            mParam->setValue(BVP::PARAM_control, BVP::SRC_int,
                             (1 << BVP::CTRL_resetIndication));

            if (mAlarm.getAlarmOutputSignal(BVP::EXT_ALARM_fault)) {
                mParam->setValue(BVP::PARAM_control, BVP::SRC_int,
                                 (1 << BVP::CTRL_resetErrors));
            }

            if (mAlarm.getAlarmOutputSignal(BVP::EXT_ALARM_warning)) {
                mParam->setValue(BVP::PARAM_control, BVP::SRC_int,
                                 (1 << BVP::CTRL_resetErrors));
            }

            // FIXME Почему вылетает в ошибку ?!
            //    Q_ASSERT(ok);
            if (ok) {
                // TODO Может отслеживать текущее состояния сигнализаций с МК, а не принятые значения по стандартному протоколу?
                // И уже здесь принимать решение какие сигналы посылать.
                if (uval32 & (1 << BVP::CTRL_resetErrors)) {
                    // TODO Добавить реакцию на сброс ошибок.
                    // Т.е. несиправности/предупреждения на данный момент есть!
                }  else if (uval32 & (1 << BVP::CTRL_resetIndication)) {
                    // TODO Добавить рекцию на сброс индикации! Т.е. неисправностей уже нет!
                }
                // TODO добавить в TExtAlarm метод для сброса?!
            }
        }

        resetbtn = tresetbtn;
    }


    uval32 = mParam->getValue(BVP::PARAM_alarmReset, BVP::SRC_int, ok);
    if (!ok || (uval32 >= BVP::ALARM_RESET_MAX)) {
        uval32 = mAlarm.kAlarmResetDefault;
    }
    mAlarm.setAlarmReset(BVP::alarmReset_t(uval32));

    for(BVP::extAlarm_t signal = BVP::extAlarm_t(0);
        signal < BVP::EXT_ALARM_MAX; signal = BVP::extAlarm_t(signal + 1)) {
        bool value;

        if (signal == BVP::EXT_ALARM_disablePrm) {
            uval32 = mParam->getValue(BVP::PARAM_blkComPrmAll,
                                          BVP::SRC_int, ok);
            if (!ok) {
                uval32 = mAlarm.kDisablePrmDefault;
            }

            value = (uval32 == BVP::ON_OFF_on);
        } else {
            value = getExtAlarmSignals(signal);
        }

        mAlarm.setAlarmInputSignal(signal, value);
    }

    for(BVP::extAlarm_t signal = BVP::extAlarm_t(0);
        signal < BVP::EXT_ALARM_MAX; signal = BVP::extAlarm_t(signal + 1)) {
        bool value = mAlarm.getAlarmOutputSignal(signal);

        setExtAlarmSignal(signal, value);
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

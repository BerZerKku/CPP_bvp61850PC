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

    mModbus.setBuffer(sBuf, (sizeof(sBuf) / sizeof(sBuf[0])));

    connect(ui->serial, &TSerial::openPort,
            ui->control, &TControl::enableSlot);

    connect(ui->serial, &TSerial::closePort,
            ui->control, &TControl::disableSlot);

    connect(ui->serial, &TSerial::read, this, &MainWindow::readSlot);

    connect(ui->control, &TControl::modbusStart, this, &MainWindow::modbusStart);
    connect(ui->control, &TControl::modbusStop, this, &MainWindow::modbusStop);

    connect(&timer, &QTimer::timeout, this, &MainWindow::modbusProc);
    connect(&timer, &QTimer::timeout, this, &MainWindow::viewReadRegSlot);



    mParam = BVP::TParam::getInstance();
    mParam->setValue(BVP::PARAM_vpBtnSAnSbSac, 0);
    mParam->setValue(BVP::PARAM_vpBtnSA32to01, 0);
    mParam->setValue(BVP::PARAM_vpBtnSA64to33, 0);
    mParam->setValue(BVP::PARAM_blkComPrm32to01, 0);
    mParam->setValue(BVP::PARAM_blkComPrm64to33, 0);
    mParam->setValue(BVP::PARAM_blkComPrd32to01, 0);
    mParam->setValue(BVP::PARAM_blkComPrd64to33, 0);

    setMinimumHeight(sizeHint().height());
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
}

//
void
MainWindow::modbusProc() {
    mModbus.tick();
    if (mModbus.read()) {

    }

    if (mModbus.write()) {
        QVector<uint8_t> pkg;
        uint8_t byte;
        while(mModbus.pop(byte)) {
            pkg.append(byte);
        }
        writePkg(pkg);
    }

    ui->control->setModbusConnection(mModbus.isConnection());
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
    quint16 value;
    BVP::TParam *params = BVP::TParam::getInstance();

    group = vpReg::GROUP_control;
    value = static_cast<quint16> (params->getValue(BVP::PARAM_vpBtnSAnSbSac));
    ui->readReg->setReg(group, TReadReg::REG_FUNC_BUTTON, value);

    group = vpReg::GROUP_com16to01;
    value = static_cast<quint16> (params->getValue(BVP::PARAM_vpBtnSA32to01));
    ui->readReg->setReg(group, TReadReg::REG_FUNC_BUTTON, value);
    value = static_cast<quint16> (params->getValue(BVP::PARAM_blkComPrm32to01));
    ui->readReg->setReg(group, TReadReg::REG_FUNC_LED_ENABLE, ~value);
    ui->readReg->setReg(group, TReadReg::REG_FUNC_LED_DISABLE, value);

    group = vpReg::GROUP_com32to17;
    value = static_cast<quint16> (params->getValue(BVP::PARAM_vpBtnSA32to01) >> 16);
    ui->readReg->setReg(group, TReadReg::REG_FUNC_BUTTON, value);
    value = static_cast<quint16> (params->getValue(BVP::PARAM_blkComPrm32to01) >> 16);
    ui->readReg->setReg(group, TReadReg::REG_FUNC_LED_ENABLE, ~value);
    ui->readReg->setReg(group, TReadReg::REG_FUNC_LED_DISABLE, value);

    group = vpReg::GROUP_com48to33;
    value = static_cast<quint16> (params->getValue(BVP::PARAM_vpBtnSA64to33));
    ui->readReg->setReg(group, TReadReg::REG_FUNC_BUTTON, value);
    value = static_cast<quint16> (params->getValue(BVP::PARAM_blkComPrm64to33));
    ui->readReg->setReg(group, TReadReg::REG_FUNC_LED_ENABLE, ~value);
    ui->readReg->setReg(group, TReadReg::REG_FUNC_LED_DISABLE, value);

    group = vpReg::GROUP_com64to49;
    value = static_cast<quint16> (params->getValue(BVP::PARAM_vpBtnSA64to33) >> 16);
    ui->readReg->setReg(group, TReadReg::REG_FUNC_BUTTON, value);
    value = static_cast<quint16> (params->getValue(BVP::PARAM_blkComPrm64to33) >> 16);
    ui->readReg->setReg(group, TReadReg::REG_FUNC_LED_ENABLE, ~value);
    ui->readReg->setReg(group, TReadReg::REG_FUNC_LED_DISABLE, value);
}


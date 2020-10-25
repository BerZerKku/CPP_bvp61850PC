#include "mainwindow.h"
#include "ui_mainwindow.h"

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

    connect(ui->control, &TControl::readRegisters,
            this, &MainWindow::readRegistersSlot);

    connect(ui->control, &TControl::writeRegisters,
            this, &MainWindow::writeRegistersSlot);

    connect(ui->control, &TControl::readAndWriteRegisters,
            this, &MainWindow::readAndWriteRegistersSlot);

    connect(ui->serial, &TSerial::read, this, &MainWindow::readSlot);

    connect(ui->control, &TControl::modbusStart, this, &MainWindow::modbusStart);
    connect(ui->control, &TControl::modbusStop, this, &MainWindow::modbusStop);

    mParam = BVP::TParam::getInstance();
    mParam->setValue(BVP::PARAM_vpEnSanSbSac, 0);
    mParam->setValue(BVP::PARAM_vpEnSa16to01, 0);
    mParam->setValue(BVP::PARAM_vpEnSa32to17, 0);
    mParam->setValue(BVP::PARAM_vpEnSa38to33, 0);
    mParam->setValue(BVP::PARAM_vpEnSa64to49, 0);
    mParam->setValue(BVP::PARAM_vpDsSanSbSac, 0);
    mParam->setValue(BVP::PARAM_vpDsSa16to01, 0);
    mParam->setValue(BVP::PARAM_vpDsSa32to17, 0);
    mParam->setValue(BVP::PARAM_vpDsSa38to33, 0);
    mParam->setValue(BVP::PARAM_vpDsSa64to49, 0);

    setMinimumHeight(sizeHint().height());

    ui->readReg->setReg(vpReg::GROUP_control, TReadReg::REG_FUNC_LED_ENABLE, 0xe5f6);
    ui->readReg->setReg(vpReg::GROUP_control, TReadReg::REG_FUNC_LED_DISABLE, 0xc3d4);
     ui->readReg->setReg(vpReg::GROUP_control, TReadReg::REG_FUNC_BUTTON, 0xa1b2);
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
//    ui->textBrowser->append("\nTX >> ");
    for(auto &byte: pkg) {
       ui->serial->write(byte);
//       ui->textBrowser->insertPlainText(
//                   QString("%1 ").arg(byte, 2, 16, QLatin1Char('0')));
    }
}

//
void
MainWindow::modbusStart() {
    mModbus.setup(9600, true, 1);
    mModbus.setNetAddress(deviceAddress);
    mModbus.setTimeTick(1000);
    mModbus.setEnable(true);

    connect(&timer, &QTimer::timeout, this, &MainWindow::modbusProc);
    timer.start(1);
}

//
void
MainWindow::modbusStop() {
    mModbus.setEnable(false);
    disconnect(&timer, &QTimer::timeout, this, &MainWindow::modbusProc);
    timer.stop();
}

//
void
MainWindow::readRegistersSlot() {
    QVector<uint8_t> pkg;

    pkg.append(deviceAddress);
    pkg.append(comReadHoldingRegisters);
    // register address = register number - 1
    pkg.append(0x00);   // starting address: High
    pkg.append(0x00);   // starting address: Low
    // quantity of registers
    pkg.append(0x00);   // quantity of registers: High
    pkg.append(0x05);   // quantity of registers: Low
    // crc
    pkg.append(0x84);   // crc: Low
    pkg.append(0xB2);   // crc: High

    writePkg(pkg);
}


//
void
MainWindow::writeRegistersSlot() {
// 00 01 02 03 04 05 06 07 08 09 10 20 30 40 50 60 70
    QVector<uint8_t> pkg;

    pkg.append(deviceAddress);
    pkg.append(comWriteMultipleRegisters);
    // register address = register number - 1
    pkg.append(0x00);   // starting address: High
    pkg.append(0x0A);   // starting address: Low
    // quantity of registers
    pkg.append(0x00);   // quantity of registers: High
    pkg.append(0x0A);   // quantity of registers: Low
    // byte count
    pkg.append(0x14);
    // data
    pkg.append(0x00); // Hi
    pkg.append(0x01);
    pkg.append(0x02); // Hi
    pkg.append(0x03);
    pkg.append(0x04); // Hi
    pkg.append(0x05);
    pkg.append(0x06); // Hi
    pkg.append(0x07);
    pkg.append(0x08); // Hi
    pkg.append(0x09);
    pkg.append(0x00); // Hi
    pkg.append(0x10);
    pkg.append(0x20); // Hi
    pkg.append(0x30);
    pkg.append(0x40); // Hi
    pkg.append(0x50);
    pkg.append(0x60); // Hi
    pkg.append(0x70);
    pkg.append(0x80); // Hi
    pkg.append(0x90);
    //crc
    pkg.append(0xF6);   // crc: Low
    pkg.append(0xF8);   // crc: High

    writePkg(pkg);
}

//
void
MainWindow::readAndWriteRegistersSlot() {

}

//
void
MainWindow::modbusProc() {
    mModbus.tick();
    if (mModbus.read()) {
//        ui->readReg->setReg(1, mParam->getValue(BVP::PARAM_vpBtnSAnSbSac));
//        ui->readReg->setReg(2, mParam->getValue(BVP::PARAM_vpBtnSA16to01));
//        ui->readReg->setReg(3, mParam->getValue(BVP::PARAM_vpBtnSA32to17));
//        ui->readReg->setReg(4, mParam->getValue(BVP::PARAM_vpBtnSA38to33));
//        ui->readReg->setReg(5, mParam->getValue(BVP::PARAM_vpBtnSA64to49));
    }

    if (mModbus.write()) {
        QVector<uint8_t> pkg;
        uint8_t byte;
        while(mModbus.pop(byte)) {
            pkg.append(byte);
        }
        writePkg(pkg);
    }


}

//
void
MainWindow::readSlot(int value) {
    static QVector<uint8_t> rxPkg;
    static quint8 size = 0;
    qsizetype len = rxPkg.size();
    quint8 byte = static_cast<uint8_t> (value);

    // TODO здесь должен быть обработчик принятого сообщения Modbus

    mModbus.push(byte);

    if (len == 0) {
        if (value == 0x0A) {
            rxPkg.append(byte);
//            ui->textBrowser->append("\nRx << ");
        }
    } else if (len == 1) {
        rxPkg.append(byte);
        if (byte == comReadHoldingRegisters) {
            size = 15;
        } else if (byte == comWriteMultipleRegisters) {
            size = 8;
        } else if (byte == comReadWriteMultipleRegisters) {
            size = 15;
        } else {
            rxPkg.clear();
        }
    } else {
        rxPkg.append(byte);
        if (rxPkg.size() == size) {
            rxPkg.takeFirst(); // address
            switch(rxPkg.takeFirst()) {
                case comReadHoldingRegisters: {
                    rxPkg.takeFirst();  // num bytes
//                    ui->readReg->setReg(1, getUInt16(rxPkg));
//                    ui->readReg->setReg(2, getUInt16(rxPkg));
//                    ui->readReg->setReg(3, getUInt16(rxPkg));
//                    ui->readReg->setReg(4, getUInt16(rxPkg));
//                    ui->readReg->setReg(5, getUInt16(rxPkg));
                    rxPkg.takeFirst();  // crc
                    rxPkg.takeFirst();
                } break;
                case comWriteMultipleRegisters: {
                    rxPkg.clear();
                } break;
            }

            if (rxPkg.size() != 0) {
                qWarning() << "Package is not empty!" << hex << rxPkg;
                rxPkg.clear();
            }
        }
    }

//    ui->textBrowser->insertPlainText(
//                QString("%1 ").arg(byte, 2, 16, QLatin1Char('0')));
}


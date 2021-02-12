#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QMap>
#include <QTimer>
#include "bvpCommon/param.h"
#include "bvpCommon/serial/avantpc.h"
#include "bvpCommon/serial/avantpi.h"
#include "bvpCommon/serial/modbusVp.h"
#include "serial.h"


QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
  Q_OBJECT

  const uint8_t deviceAddress = 0x0A;
  static const uint8_t comReadHoldingRegisters;
  static const uint8_t comWriteMultipleRegisters;
  static const uint8_t comReadWriteMultipleRegisters;

  struct serialCfg_t {
    uint8_t buf[256] = {0};
    QString label;
    QVector<QString> defaultPorts;
    BVP::TSerialProtocol *protocol = nullptr;
    BVP::src_t srcId = BVP::SRC_MAX;
    uint16_t netAddr = 0;
    uint16_t baudrate = 0;
    QSerialPort::Parity parity = QSerialPort::NoParity;
    QSerialPort::StopBits stopBits = QSerialPort::TwoStop;
  };

public:
  MainWindow(QWidget *parent = nullptr);
  ~MainWindow();

private:
  Ui::MainWindow *ui;

  QTimer timer1ms;
  QTimer timer100ms;

  void initAvantPc();
  void initAvantPi();
  void initVp();
  void initSerial(TSerial *serial, serialCfg_t *cfg);

  uint16_t getUInt16(QVector<uint8_t> &pkg);

  BVP::TParam *mParam;

  QMap<TSerial*, serialCfg_t*> sPort;

 private slots:

  void protocolStart(serialCfg_t *cfg);
  void protocolStop(serialCfg_t *cfg);

  void setBspPiCfg();

  void serialProc();

  void viewReadRegSlot();
};
#endif // MAINWINDOW_H

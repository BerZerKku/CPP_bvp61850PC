#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QMap>
#include <QTimer>
#include "bvpCommon/param.h"
#include "bvpCommon/extAlarm.hpp"
#include "bvpCommon/serial/avantpc.h"
#include "bvpCommon/serial/avantpi.h"
#include "bvpCommon/serial/modbusVp.h"
#include "serial/serial.h"


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
    QVector<qint32> baudList;
    QVector<QSerialPort::Parity> parityList;
    QVector<QSerialPort::StopBits> stopList;
    BVP::TSerialProtocol *protocol = nullptr;
    BVP::src_t srcId = BVP::SRC_MAX;
    uint16_t netAddr = 0;
    uint16_t baudrate = 0;
    QSerialPort::Parity parity = QSerialPort::NoParity;
    QSerialPort::StopBits stopBits = QSerialPort::TwoStop;
  };

public:
  MainWindow(QWidget *parent = nullptr);
  ~MainWindow() override;

private:
  Ui::MainWindow *ui;
  TSerial *serialPi = nullptr;

  QTimer timer1ms;
  QTimer timer100ms;

  BVP::TParam *mParam;
  BVP::TExtAlarm mAlarm;
  BVP::TModbusVp *mModbus;

  QMap<TSerial*, serialCfg_t*> sPort;

  void initAvantPc();
  void initAvantPi();
  void initVp();
  void initSerial(TSerial *serial, serialCfg_t *cfg);
  void addSerialToFrame(TSerial *serial);

  uint16_t getUInt16(QVector<uint8_t> &pkg);



 private slots:

  void serialStart(TSerial *serial);
  void serialStop(TSerial *serial);

  void serialProc();

  void procAlarm();
  void viewReadRegSlot();

  friend bool getExtAlarmSignal(BVP::extAlarm_t signal);
  friend void setExtAlarmSignal(BVP::extAlarm_t signal, bool value);
};

#endif // MAINWINDOW_H

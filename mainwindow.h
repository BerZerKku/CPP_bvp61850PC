#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTimer>
#include "bvpCommon/modbus.h"
#include "bvpCommon/param.h"

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

public:
  MainWindow(QWidget *parent = nullptr);
  ~MainWindow();

private:
  Ui::MainWindow *ui;
  uint8_t sBuf[128];
  QTimer timer;

  uint16_t getUInt16(QVector<uint8_t> &pkg);
  void writePkg(QVector<uint8_t> &pkg);

  BVP::TModbus mModbus;
  BVP::TParam *mParam;

private slots:
  void modbusStart();
  void modbusStop();
  void modbusProc();

  void readSlot(int value);

  void viewReadRegSlot();
};
#endif // MAINWINDOW_H

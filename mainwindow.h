#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTimer>
#include "bvpCommon/param.h"
#include "bvpCommon/serial/avantpi.h"
#include "bvpCommon/serial/modbusVp.h"


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
  uint8_t bufModbus[256];
  uint8_t bufAvantPi[256];
  QTimer timer;

  uint16_t getUInt16(QVector<uint8_t> &pkg);
  void writePkgVp(QVector<uint8_t> &pkg);
  void writePkgPi(QVector<uint8_t> &pkg);

  BVP::TModbusVp mModbus;
  BVP::TAvantPi mAvantPi;
  BVP::TParam *mParam;

private slots:
  void modbusStart();
  void modbusStop();

  void serialProc();

  void avantPiStart();
  void avantPiStop();

  void viewReadRegSlot();
};
#endif // MAINWINDOW_H

#ifndef SERIAL_H
#define SERIAL_H

#include <QPointer>
#include <QThread>
#include <QTimer>
#include <QWidget>

#include "serial/serialport.h"

namespace Ui {
class TSerial;
}

class TSerial : public QWidget
{
  Q_OBJECT

  // Время горения светодиода при передаче/приеме байта
  const uint16_t kTimeLedOnMs = 200;

public:
  explicit TSerial(QWidget *parent = nullptr);
  ~TSerial();

  void setLabelText(QString text);
  bool setup(uint32_t baudrate, QSerialPort::Parity parity,
             QSerialPort::StopBits stopbits);
  void addDefaultPort(QString portname);

  void setLedLink(bool enable);

signals:
  void write(int value);
  void openPort();
  void closePort();
  void read(int byte);
  void sendFinished();

private:
  Ui::TSerial *ui;
  QPointer<TSerialPort> sport;    // Последовательный порт.
  QPointer<QThread> thread;       // Поток для работы последовательного порта.
  QVector<QString> defaultPorts;  // Список портов устанавливаемых по умолчанию.
  QTimer timerLedRx;              // Таймер светодиода принятых байт.
  QTimer timerLedTx;              // Таймер светодиода переданных байт.

  uint32_t mBaudrate = 0;
  QSerialPort::Parity mParity = QSerialPort::NoParity;
  QSerialPort::StopBits mStopBits = QSerialPort::OneStop;

private slots:
  void refreshPortList();
  void connectSerialPort();
  void closeSerialPort();
  void setLedRx(bool enable);
  void setLedTx(bool enable);
};

#endif // SERIAL_H

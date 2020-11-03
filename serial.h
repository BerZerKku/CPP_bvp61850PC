#ifndef SERIAL_H
#define SERIAL_H

#include <QPointer>
#include <QThread>
#include <QWidget>

#include "serial/serialport.h"

namespace Ui {
class TSerial;
}

class TSerial : public QWidget
{
  Q_OBJECT

public:
  explicit TSerial(QWidget *parent = nullptr);
  ~TSerial();

  void setLabelText(QString text);
  bool setup(uint32_t baudrate, QSerialPort::Parity parity,
             QSerialPort::StopBits stopbits);
  void addDefaultPort(QString portname);

signals:
  void write(int value);
  void openPort();
  void closePort();
  void read(int byte);
  void sendFinished();

private:
  Ui::TSerial *ui;
  QPointer<TSerialPort> sport;
  QPointer<QThread> thread;
  QVector<QString> defaultPorts;

  uint32_t mBaudrate = 0;
  QSerialPort::Parity mParity = QSerialPort::NoParity;
  QSerialPort::StopBits mStopBits = QSerialPort::OneStop;

private slots:
  void refreshPortList();
  void connectSerialPort();
  void closeSerialPort();
};

#endif // SERIAL_H

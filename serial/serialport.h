#ifndef SERIALPORT_H
#define SERIALPORT_H

#include <QObject>
#include <QtSerialPort>

class TSerialPort : public QObject {
  Q_OBJECT

public:
  explicit TSerialPort(QString portname, qint32 baudrate);
  ~TSerialPort();

public slots:
  void start();
  void stop();
  void writeByteSlot(int byte);

signals:
  void finished();
  void readByte(int byte);
  void sendFinished();

private:
  qint32 m_baudrate = 0;
  QSerialPort *port = nullptr;
  QString m_portName;
  QPointer<QTimer> timer;
  double m_timeToFinishSendMs = 0.0;
  double m_byteSendMs = 0.0;

  QVector<uint8_t> bufTx;

  double calcOneByteSendTime() const;

private slots:
  void error(QSerialPort::SerialPortError err);
  void readyReadSlot();
  void timeoutSlot();
};

#endif // SERIALPORT_H

#include "serial/serialport.h"

TSerialPort::TSerialPort(QString portname, qint32 baudrate) :
  m_baudrate(baudrate), m_portName(portname) {

}

TSerialPort::~TSerialPort() {
}

//
void
TSerialPort::start() {
  if (port == nullptr) {
    port = new QSerialPort(m_portName);
    if (!port->setBaudRate(m_baudrate)) {
      emit finished();
    }
    port->setParity(QSerialPort::Parity::EvenParity);
    port->setStopBits(QSerialPort::StopBits::OneStop);
  }

  if (timer.isNull()) {
    timer = new QTimer();
    timer->setInterval(1);
    connect(timer, &QTimer::timeout, this, &TSerialPort::timeoutSlot);
  } else {
    timer->stop();
  }

  if (port->open(QSerialPort::ReadWrite)) {
    connect(port, &QSerialPort::errorOccurred, this, &TSerialPort::error);
    connect(port, &QSerialPort::readyRead,
            this, &TSerialPort::readyReadSlot);
    m_timeToFinishSendMs = 0.0;
    bufTx.clear();
    m_byteSendMs = calcOneByteSendTime();
    port->clear();
  } else {
    emit finished();
  }
}

//
void
TSerialPort::stop() {
  timer->stop();
  port->close();
  emit finished();
}

//
void
TSerialPort::writeByteSlot(int byte) {
  if (port->isOpen()) {
    bufTx.append(static_cast<quint8> (byte));
    if (!timer->isActive()) {
      m_timeToFinishSendMs = 0.0;
      timer->start();
    }
  }
}

//
double
TSerialPort::calcOneByteSendTime() const {
  double numbites = 1.0;

  switch(port->dataBits()) {
    case QSerialPort::Data5: {
      numbites += 5.0;
    } break;
    case QSerialPort::Data6: {
      numbites += 6.0;
    } break;
    case QSerialPort::Data7: {
      numbites += 7.0;
    } break;
    case QSerialPort::Data8: {
      numbites += 8.0;
    } break;
    default: {
      numbites += 8.0;
      qCritical() << "Error data bits value";
    }
  }

  switch(port->stopBits()){
    case QSerialPort::OneStop: {
      numbites += 1.0;
    } break;
    case QSerialPort::OneAndHalfStop: {
      numbites += 1.5;
    } break;
    case  QSerialPort::TwoStop: {
      numbites += 2.0;
    }break;
    default: {
      numbites += 8.0;
      qCritical() << "Error stop bits value";
    }
  }

  return (1000 * numbites) / port->baudRate();
}

//
void
TSerialPort::error(QSerialPort::SerialPortError err) {
  if (err == QSerialPort::ResourceError) {
    stop();
  }
}

//
void
TSerialPort::readyReadSlot() {
  for(auto &byte: port->readAll()) {
    emit readByte(static_cast<uint8_t> (byte));
  }
}

//
void
TSerialPort::timeoutSlot() {
  m_timeToFinishSendMs += 1.0;    // 1.0 шаг таймера

  while(m_timeToFinishSendMs >= m_byteSendMs) {
    if (bufTx.isEmpty()) {
      timer->stop();
      emit sendFinished();
      break;
    }

    char byte = static_cast<char> (bufTx.takeFirst());
    port->write(&byte, 1);
  }
}

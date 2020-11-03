#include "serial.h"
#include "ui_serial.h"

//
bool
comp(const QString &s1, const QString &s2) {
  bool less;

  if (s1.size() != s2.size()) {
    less = (s1.size() < s2.size());
  } else {
    less = (s1.compare(s2) < 0);
  }

  return less;
}

//
TSerial::TSerial(QWidget *parent) :
  QWidget(parent),
  ui(new Ui::TSerial)
{
  ui->setupUi(this);

  refreshPortList();
  connect(ui->pbRefresh, &QPushButton::clicked,
          this, &TSerial::refreshPortList);

  connect(ui->pbOpen, &QPushButton::clicked,
          this, &TSerial::connectSerialPort);

  ui->pbOpen->setFixedSize(ui->pbOpen->sizeHint());
  ui->pbRefresh->setFixedSize(ui->pbOpen->sizeHint());
  ui->cbPort->setFixedHeight(ui->pbOpen->sizeHint().height());

  setFixedHeight(sizeHint().height());
}

//
TSerial::~TSerial() {
  // Не решило проблему! Ошибка все равно появляется, просто не всегда!

  if (!thread.isNull()) {
    thread->quit();
  }

  if (!sport.isNull()) {
    delete sport;
  }

  delete ui;
}

//
void
TSerial::setLabelText(QString text) {
  ui->lPort->setText(text);
}

//
bool
TSerial::setup(uint32_t baudrate, QSerialPort::Parity parity,
               QSerialPort::StopBits stopbits) {

  mBaudrate = baudrate;
  mParity = parity;
  mStopBits = stopbits;

  return true;
}

//
void
TSerial::addDefaultPort(QString portname) {
  if (!portname.isEmpty() && !defaultPorts.contains(portname)) {
    defaultPorts.append(portname);
  }

  ui->cbPort->setCurrentText(portname);
}

//
void
TSerial::refreshPortList() {
  QString portname;
  QList<QString> ports;
  QList<QSerialPortInfo> infos = QSerialPortInfo::availablePorts();


  if (ui->cbPort->isEnabled()) {
    portname = ui->cbPort->currentText();
    ui->cbPort->clear();


    for(const QSerialPortInfo &info :infos) {
      QString port = info.portName();
      if (!info.isBusy()) {
        ports.append(port);
      }
    }

    std::sort(ports.begin(), ports.end(), comp);
    for(const QString &port: ports) {
      ui->cbPort->addItem(port);
    }

    if (portname.isEmpty()) {
      for(QString &portname: defaultPorts) {
        ui->cbPort->setCurrentText(portname);
      }
    } else {
      ui->cbPort->setCurrentText(portname);
    }
    ui->pbOpen->setEnabled(ui->cbPort->count() != 0);
  }
}

//
void
TSerial::connectSerialPort() {
  if (sport.isNull() && thread.isNull()) {
    sport = new TSerialPort(ui->cbPort->currentText(), mBaudrate,
                            mParity, mStopBits);

    thread = new QThread(this);

    connect(thread, &QThread::started, sport, &TSerialPort::start);
    connect(thread, &QThread::finished, sport, &TSerialPort::stop);
    connect(thread, &QThread::finished, thread, &QThread::deleteLater);

    connect(sport, &TSerialPort::finished, this, &TSerial::closeSerialPort);
    connect(sport, &TSerialPort::finished, thread, &QThread::quit);
    connect(sport, &TSerialPort::finished, sport, &TSerialPort::deleteLater);
    connect(sport, &TSerialPort::readByte, this, &TSerial::read);
    connect(sport, &TSerialPort::sendFinished, this, &TSerial::sendFinished);

    connect(this, &TSerial::write, sport, &TSerialPort::writeByteSlot);

    disconnect(ui->pbOpen, &QPushButton::clicked,
               this, &TSerial::connectSerialPort);
    connect(ui->pbOpen, &QPushButton::clicked, sport, &TSerialPort::stop);


    ui->cbPort->setEnabled(false);
    ui->pbOpen->setText("Close");
    ui->pbRefresh->setEnabled(false);

    sport->moveToThread(thread);
    thread->start();
    emit openPort();
  } else {
    qDebug() << "portBSP.isNull() && threadBSP.isNull()";
  }
}

//
void
TSerial::closeSerialPort() {
  connect(ui->pbOpen, &QPushButton::clicked,
          this, &TSerial::connectSerialPort);

  refreshPortList();
  ui->cbPort->setEnabled(true);
  ui->pbOpen->setText("Open");
  ui->pbOpen->setEnabled(true);
  ui->pbRefresh->setEnabled(true);

  emit closePort();
}

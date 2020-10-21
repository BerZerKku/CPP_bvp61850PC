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
}

//
TSerial::~TSerial() {
    if (!thread.isNull()) {
        thread->exit();
    }

    if (!sport.isNull()) {
        delete sport;
    }

    delete ui;
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
            ui->cbPort->setCurrentText("COM5");
            ui->cbPort->setCurrentText("tnt0");
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
        sport = new TSerialPort(ui->cbPort->currentText(), 9600);
        thread = new QThread(this);

        connect(thread, &QThread::started, sport, &TSerialPort::start);
        connect(thread, &QThread::finished, sport, &TSerialPort::stop);
        connect(thread, &QThread::finished, thread, &QThread::deleteLater);

        connect(sport, &TSerialPort::finished, this, &TSerial::closeSerialPort);
        connect(sport, &TSerialPort::finished, thread, &QThread::quit);
        connect(sport, &TSerialPort::finished, sport, &TSerialPort::deleteLater);
        connect(sport, &TSerialPort::readByte, this, &TSerial::read);

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

#include "serial.h"
#include "ui_serial.h"

#include <QLineEdit>
#include <QPair>

//
bool comp(const QString &s1, const QString &s2)
{
    bool less;

    if (s1.size() != s2.size())
    {
        less = (s1.size() < s2.size());
    }
    else
    {
        less = (s1.compare(s2) < 0);
    }

    return less;
}

//
TSerial::TSerial(QWidget *parent) : QWidget(parent), ui(new Ui::TSerial)
{
    ui->setupUi(this);

    refreshPortList();

    connect(ui->pbOpen, &QPushButton::clicked, this, &TSerial::connectSerialPort);

    connect(this, &TSerial::write, [=]() { setLedTx(true); });
    connect(this, &TSerial::write, [=]() { timerLedTx.start(kTimeLedOnMs); });
    connect(&timerLedTx, &QTimer::timeout, [=]() { setLedTx(false); });

    connect(this, &TSerial::read, [=]() { setLedRx(true); });
    connect(this, &TSerial::read, [=]() { timerLedRx.start(kTimeLedOnMs); });
    connect(&timerLedRx, &QTimer::timeout, [=]() { setLedRx(false); });

    connect(ui->cbPort, &TComboBox::popuped, this, &TSerial::refreshPortList);
    connect(ui->pbOpen, &QPushButton::clicked, this, &TSerial::changeConfigEnabled);


    ui->pbOpen->setFixedSize(ui->pbOpen->sizeHint());
    ui->cbPort->setFixedHeight(ui->pbOpen->sizeHint().height());

    ui->ledLink->setDisabled(true);
    ui->ledRx->setDisabled(true);
    ui->ledTx->setDisabled(true);

    changeConfigEnabled();

    setFixedHeight(sizeHint().height());
}

//
TSerial::~TSerial()
{
    // Не решило проблему! Ошибка все равно появляется,
    // просто не всегда!

    if (!thread.isNull())
    {
        thread->quit();
    }

    if (!sport.isNull())
    {
        delete sport;
    }

    delete ui;
}

//
void TSerial::setLabelText(QString text)
{
    ui->lPort->setText(text);
}

//
bool TSerial::setup(uint32_t baudrate, QSerialPort::Parity parity, QSerialPort::StopBits stopbits)
{
    Q_ASSERT(ui->cbBaudRate->findData(baudrate) != -1);
    ui->cbBaudRate->setCurrentIndex(ui->cbBaudRate->findData(baudrate));

    Q_ASSERT(ui->cbParity->findData(parity) != -1);
    ui->cbParity->setCurrentIndex(ui->cbParity->findData(parity));

    Q_ASSERT(ui->cbStopBit->findData(stopbits) != -1);
    ui->cbStopBit->setCurrentIndex(ui->cbStopBit->findData(stopbits));

    return true;
}

//
void TSerial::addDefaultPort(QString portname)
{
    if (!portname.isEmpty() && !defaultPorts.contains(portname))
    {
        defaultPorts.append(portname);
    }

    int index = ui->cbPort->findText(portname);
    if (index >= 0)
    {
        ui->cbPort->setCurrentIndex(index);
    }
}

//
void TSerial::setLedLink(bool enable)
{
    ui->ledLink->setChecked(enable);
}

bool TSerial::setBaudRateList(QVector<qint32> &values)
{
    QComboBox *cb = ui->cbBaudRate;

    cb->clear();
    for (auto value : values)
    {
        cb->addItem(QString("%1").arg(value), value);
    }

    Q_ASSERT(cb->count() >= 1);

    return cb->count() != 0;
}

bool TSerial::setStopBitList(QVector<QSerialPort::StopBits> &values)
{
    QComboBox *cb = ui->cbStopBit;

    cb->clear();
    for (auto value : values)
    {
        cb->addItem(QString("%1").arg(value), value);
    }

    Q_ASSERT(cb->count() >= 1);

    return cb->count() != 0;
}

bool TSerial::setParityList(QVector<QSerialPort::Parity> &values)
{
    QComboBox *cb = ui->cbParity;

    QMap<QSerialPort::Parity, QString> parityString;
    parityString.insert(QSerialPort::NoParity, "  Нет (N)");
    parityString.insert(QSerialPort::EvenParity, "  Чет (E)");
    parityString.insert(QSerialPort::OddParity, "Нечет (O)");

    cb->clear();
    for (auto &value : values)
    {
        Q_ASSERT(parityString.count(value) == 1);
        cb->addItem(parityString.value(value), value);
    }

    Q_ASSERT(cb->count() >= 1);

    return cb->count() != 0;
}


//
qint32 TSerial::getBaudRate() const
{
    return ui->cbBaudRate->currentData().toInt();
}


//
QSerialPort::Parity TSerial::getParity() const
{
    qint16 value = ui->cbParity->currentData().toInt();
    return static_cast<QSerialPort::Parity>(value);
}


//
QSerialPort::StopBits TSerial::getStopBits() const
{
    qint16 value = ui->cbStopBit->currentData().toInt();
    return static_cast<QSerialPort::StopBits>(value);
}


//
void TSerial::refreshPortList()
{
    QString                portname;
    QList<QString>         ports;
    QMap<QString, QString> portsInfo;
    QList<QSerialPortInfo> infos = QSerialPortInfo::availablePorts();

    if (ui->cbPort->isEnabled())
    {
        portname = ui->cbPort->currentText();
        ui->cbPort->clear();

        for (const QSerialPortInfo &info : infos)
        {
            if (!info.isBusy())
            {
                ports.append(info.portName());
                QString deviceInfo = QString("%1 %2\n%3:%4# %5")
                                         .arg(info.manufacturer())
                                         .arg(info.description())
                                         .arg(info.vendorIdentifier())
                                         .arg(info.productIdentifier())
                                         .arg(info.serialNumber());

                portsInfo.insert(info.portName(), deviceInfo);
            }
        }

        std::sort(ports.begin(), ports.end(), comp);
        int index = 0;
        for (auto &port : ports)
        {
            ui->cbPort->addItem(port, portsInfo.value(port));
            ui->cbPort->setItemData(index, portsInfo.value(port), Qt::ToolTipRole);
            index++;
        }

        if (portname.isEmpty())
        {
            for (QString &portname : defaultPorts)
            {
                ui->cbPort->setCurrentText(portname);
            }
        }
        else
        {
            ui->cbPort->setCurrentText(portname);
        }
        ui->pbOpen->setEnabled(ui->cbPort->count() != 0);
    }
}


//
void TSerial::connectSerialPort()
{
    if (sport.isNull() && thread.isNull())
    {
        QSerialPort::Parity parity =
            static_cast<QSerialPort::Parity>(ui->cbParity->currentData().toInt());
        QSerialPort::StopBits stopbits =
            static_cast<QSerialPort::StopBits>(ui->cbStopBit->currentData().toInt());

        sport = new TSerialPort(ui->cbPort->currentText(),
                                ui->cbBaudRate->currentData().toInt(),
                                parity,
                                stopbits);

        thread = new QThread(this);

        connect(thread, &QThread::started, sport, &TSerialPort::start);
        connect(thread, &QThread::finished, sport, &TSerialPort::stop);
        connect(thread, &QThread::finished, thread, &QThread::deleteLater);

        connect(sport, &TSerialPort::finished, this, &TSerial::closeSerialPort);
        connect(sport, &TSerialPort::finished, thread, &QThread::quit);
        connect(sport, &TSerialPort::finished, sport, &TSerialPort::deleteLater);
        connect(sport, &TSerialPort::finished, this, &TSerial::changeConfigEnabled);
        connect(sport, &TSerialPort::readByte, this, &TSerial::read);
        connect(sport, &TSerialPort::sendFinished, this, &TSerial::sendFinished);

        connect(this, &TSerial::write, sport, &TSerialPort::writeByteSlot);

        disconnect(ui->pbOpen, &QPushButton::clicked, this, &TSerial::connectSerialPort);
        connect(ui->pbOpen, &QPushButton::clicked, sport, &TSerialPort::stop);

        sport->moveToThread(thread);
        thread->start();
        emit openPort();
    }
    else
    {
        qDebug() << "portBSP.isNull() && threadBSP.isNull()";
    }
}

//
void TSerial::closeSerialPort()
{
    connect(ui->pbOpen, &QPushButton::clicked, this, &TSerial::connectSerialPort);

    refreshPortList();
    ui->cbPort->setEnabled(true);
    ui->pbOpen->setEnabled(true);

    emit closePort();
}


//
void TSerial::setLedRx(bool enable)
{
    ui->ledRx->setChecked(enable);
}


//
void TSerial::setLedTx(bool enable)
{
    ui->ledTx->setChecked(enable);
}


//
void TSerial::changeConfigEnabled()
{
    bool enable = sport.isNull();

    if (enable)
    {
        connect(ui->pbOpen,
                &QPushButton::clicked,
                this,
                &TSerial::changeConfigEnabled,
                Qt::UniqueConnection);
    }
    else
    {
        disconnect(ui->pbOpen, &QPushButton::clicked, this, &TSerial::changeConfigEnabled);
    }

    ui->pbOpen->setText(enable ? "Open" : "Close");
    ui->cbPort->setEnabled(enable);
    ui->cbBaudRate->setEnabled(enable);
    ui->cbParity->setEnabled(enable);
    ui->cbStopBit->setEnabled(enable);
}

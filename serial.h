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

signals:
    void write(int value);
    void openPort();
    void closePort();
    void read(int byte);

private:
    Ui::TSerial *ui;
    QPointer<TSerialPort> sport;
    QPointer<QThread> thread;

private slots:
    void refreshPortList();
    void connectSerialPort();
    void closeSerialPort();
};

#endif // SERIAL_H

#include "alarm.h"
#include "ui_alarm.h"

TAlarm::TAlarm(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::TAlarm)
{
    ui->setupUi(this);

    setLedYellow(ui->ledModel61850);
    setLedYellow(ui->ledTest61850);
    setLedRed(ui->ledFault);
    setLedYellow(ui->ledWarning);
    setLedRed(ui->ledFaultChannel);

    setButtonRed(ui->pbComPrd);
    setButtonRed(ui->pbComPrm);
    setButtonRed(ui->pbFault);
    setButtonRed(ui->pbPrmOut);
    setButtonYellow(ui->pbWarning);
}

TAlarm::~TAlarm()
{
    delete ui;
}

void TAlarm::setSignal(BVP::extAlarm_t signal, bool value)
{
    Q_ASSERT((signal >= 0) && (signal < BVP::EXT_ALARM_MAX));

    switch(signal) {
        case BVP::EXT_ALARM_model61850:
            ui->ledModel61850->setChecked(value);
            break;
        case BVP::EXT_ALARM_test61850:
            ui->ledTest61850->setChecked(value);
            break;
        case BVP::EXT_ALARM_channelFault:
            ui->ledFaultChannel->setChecked(value);
            break;
        case BVP::EXT_ALARM_warning:
            ui->ledWarning->setChecked(value);
            ui->pbWarning->set(value);
            break;
        case BVP::EXT_ALARM_fault:
            ui->ledFault->setChecked(value);
            ui->pbFault->set(value);
            break;
        case BVP::EXT_ALARM_comPrd:
            ui->pbComPrd->set(value);
            break;
        case BVP::EXT_ALARM_comPrm:
            ui->pbComPrm->set(value);
            break;
        case BVP::EXT_ALARM_disablePrm:
            ui->pbPrmOut->set(value);
            break;
        case BVP::EXT_ALARM_MAX:
            break;
    }
}

bool TAlarm::getSignal(BVP::extAlarm_t signal)
{
    bool value = false;

    Q_ASSERT((signal >= 0) && (signal < BVP::EXT_ALARM_MAX));

    switch(signal) {
        case BVP::EXT_ALARM_model61850:
            break;
        case BVP::EXT_ALARM_test61850:
            break;
        case BVP::EXT_ALARM_channelFault:
            value = ui->chbFaultChannel->isChecked();
            break;
        case BVP::EXT_ALARM_comPrm:
            value = ui->chbComPrm->isChecked();
            break;
        case BVP::EXT_ALARM_comPrd:
            value = ui->chbComPrd->isChecked();
            break;
        case BVP::EXT_ALARM_warning:
            value = ui->chbWarning->isChecked();
            break;
        case BVP::EXT_ALARM_fault:
            value = ui->chbFault->isChecked();
            break;
        case BVP::EXT_ALARM_disablePrm:
            break;
        case BVP::EXT_ALARM_MAX:
            break;
    }

    return value;
}

void TAlarm::setButton(QColorButton *pb, Qt::GlobalColor color)
{
    pb->set(false);
    pb->setColor(color);
}

void TAlarm::setButtonGreen(QColorButton *pb)
{
    setButton(pb, Qt::green);
}

void TAlarm::setButtonRed(QColorButton *pb)
{
    setButton(pb, Qt::red);
}

void TAlarm::setButtonYellow(QColorButton *pb)
{
    setButton(pb, Qt::yellow);
}

void TAlarm::setLed(QLedIndicator *led, QColor on1, QColor on2, QColor off2)
{
    led->setOnColor1(on1);
    led->setOnColor2(on2);
    led->setOffColor2(off2);

    led->setCheckable(true);
    led->setDisabled(true);
}

void TAlarm::setLedYellow(QLedIndicator *led)
{
    const QColor on1 = QColor(255, 255, 0);
    const QColor on2 = QColor(192, 192, 0);
    const QColor off2 = QColor(128, 128, 0);

    setLed(led, on1, on2, off2);
}

void TAlarm::setLedRed(QLedIndicator *led)
{
    const QColor on1 = QColor(255, 0 , 0);
    const QColor on2 = QColor(192, 0 , 0);
    const QColor off2 = QColor(128, 0 , 0);

    setLed(led, on1, on2, off2);
}

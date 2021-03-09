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

    setPushButton(ui->pbComPrd);
    setPushButton(ui->pbComPrm);
    setPushButton(ui->pbFault);
    setPushButton(ui->pbPrmOut);
    setPushButton(ui->pbWarning);
}

TAlarm::~TAlarm()
{
    delete ui;
}

void TAlarm::setSignal(BVP::extAlarmOut_t signal, bool value)
{
    Q_ASSERT((signal > 0) && (signal <= BVP::EXT_ALARM_OUT_disablePrm));

    switch(signal) {
        case BVP::EXT_ALARM_OUT_model61850:
            ui->ledModel61850->setChecked(value);
            break;
        case BVP::EXT_ALARM_OUT_test61850:
            ui->ledTest61850->setChecked(value);
            break;
        case BVP::EXT_ALARM_OUT_channelFault:
            ui->ledFaultChannel->setChecked(value);
            break;
        case BVP::EXT_ALARM_OUT_warning:
            ui->ledWarning->setChecked(value);
            break;
        case BVP::EXT_ALARM_OUT_fault:
            ui->ledFault->setChecked(value);
            break;
        case BVP::EXT_ALARM_OUT_comPrd:

            break;
        case BVP::EXT_ALARM_OUT_comPrm:
            break;
        case BVP::EXT_ALARM_OUT_disablePrm:
            break;
    }
}

bool TAlarm::getSignal(BVP::extAlarmIn_t signal)
{
    bool value = false;

    Q_ASSERT((signal > 0) && (signal <= BVP::EXT_ALARM_IN_fault));

    switch(signal) {
        case BVP::EXT_ALARM_IN_channelFault:
            value = ui->chbFaultChannel->isChecked();
            break;
        case BVP::EXT_ALARM_IN_comPrm:
            value = ui->chbComPrm->isChecked();
            break;
        case BVP::EXT_ALARM_IN_comPrd:
            value = ui->chbComPrd->isChecked();
            break;
        case BVP::EXT_ALARM_IN_warning:
            value = ui->chbWarning->isChecked();
            break;
        case BVP::EXT_ALARM_IN_fault:
            value = ui->chbFault->isChecked();
            break;
    }

    return value;
}

void TAlarm::setPushButton(QPushButton *pb)
{
    pb->setEnabled(false);
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

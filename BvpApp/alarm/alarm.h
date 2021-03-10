#ifndef ALARM_H
#define ALARM_H

#include <QPushButton>
#include <QWidget>
#include "widget/qledindicator.h"
#include "bvpCommon/extAlarm.hpp"
#include "widget/qcolorbutton.h"

namespace Ui {
class TAlarm;
}

class TAlarm : public QWidget
{
    Q_OBJECT

public:
    explicit TAlarm(QWidget *parent = nullptr);
    ~TAlarm() override;

    void setSignal(BVP::extAlarm_t signal, bool value);
    bool getSignal(BVP::extAlarm_t signal);

private:
    Ui::TAlarm *ui;

    void setButton(QColorButton *pb, Qt::GlobalColor color);
    void setButtonGreen(QColorButton *pb);
    void setButtonRed(QColorButton *pb);
    void setButtonYellow(QColorButton *pb);


    void setLed(QLedIndicator *led, QColor on1, QColor on2, QColor off2);
    void setLedRed(QLedIndicator *led);
    void setLedYellow(QLedIndicator *led);
};

#endif // ALARM_H

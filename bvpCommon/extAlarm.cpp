/*
 * extAlarm.cpp
 *
 *  Created on: March 23, 2021
 *      Author: bear
 */

#include "extAlarm.hpp"

namespace BVP {

TExtAlarm::signal_t TExtAlarm::mSignal[] = {
    {
        .signal     = EXT_ALARM_model61850,
        .resetMode  = RESET_MODE_off,
        .alarmReset = ALARM_RESET_MAX,
        .mode       = MODE_work,
        .input      = false,
        .output     = false,
        .valDef     = false
    },
    //
    {
        .signal     = EXT_ALARM_test61850,
        .resetMode  = RESET_MODE_off,
        .alarmReset = ALARM_RESET_auto,
        .mode       = MODE_work,
        .input      = false,
        .output     = false,
        .valDef     = false
    },
    //
    {
        .signal     = EXT_ALARM_channelFault,
        .resetMode  = RESET_MODE_off,
        .alarmReset = ALARM_RESET_MAX,
        .mode       = MODE_work,
        .input      = false,
        .output     = false,
        .valDef     = false
    },
    //
    {
        .signal     = EXT_ALARM_warning,
        .resetMode  = RESET_MODE_off,
        .alarmReset = ALARM_RESET_MAX,
        .mode       = MODE_work,
        .input      = false,
        .output     = false,
        .valDef     = false
    },
    //
    {
        .signal     = EXT_ALARM_fault,
        .resetMode  = RESET_MODE_off,
        .alarmReset = ALARM_RESET_MAX,
        .mode       = MODE_work,
        .input      = false,
        .output     = false,
        .valDef     = true
    },
    //
    {
        .signal     = EXT_ALARM_comPrd,
        .resetMode  = RESET_MODE_direct,
        .alarmReset = ALARM_RESET_manual,
        .mode       = MODE_work,
        .input      = false,
        .output     = false,
        .valDef     = false
    },
    //
    {
        .signal     = EXT_ALARM_comPrm,
        .resetMode  = RESET_MODE_direct,
        .alarmReset = ALARM_RESET_manual,                
        .mode       = MODE_work,
        .input      = false,
        .output     = false,
        .valDef     = false
    },
    //
    {
        .signal     = EXT_ALARM_disablePrm,
        .resetMode  = RESET_MODE_off,
        .alarmReset = ALARM_RESET_MAX,
        .mode       = MODE_work,
        .input      = false,
        .output     = false,
        .valDef     = false
    }
};

TExtAlarm::TExtAlarm()
{
    //        Q_ASSERT(mResetMode[EXT_ALARM_model61850] == RESET_MODE_direct);
    //        Q_ASSERT(mResetMode[EXT_ALARM_test61850] == RESET_MODE_direct);
    //        Q_ASSERT(mResetMode[EXT_ALARM_channelFault] == RESET_MODE_direct);
    //        Q_ASSERT(mResetMode[EXT_ALARM_warning] == RESET_MODE_direct);
    //        Q_ASSERT(mResetMode[EXT_ALARM_fault] == RESET_MODE_off);
    //        Q_ASSERT(mResetMode[EXT_ALARM_comPrd] == RESET_MODE_direct);
    //        Q_ASSERT(mResetMode[EXT_ALARM_comPrm] == RESET_MODE_direct);
    //        Q_ASSERT(mResetMode[EXT_ALARM_disablePrm] == RESET_MODE_direct);

  for(uint8_t i = 0; i < EXT_ALARM_MAX; i++) {
    signal_t *s = &mSignal[i];
    s->output = s->valDef;
  }
}

bool TExtAlarm::setAlarmInputSignal(extAlarm_t signal, bool value)
{
    Q_ASSERT(signal <= EXT_ALARM_MAX);

    if (signal < EXT_ALARM_MAX) {
        setSignal(signal, value);
    }

    return signal < EXT_ALARM_MAX;
}

//
bool TExtAlarm::isSignalForDeviceReset()
{   
    const extAlarm_t signal[] = {
        EXT_ALARM_channelFault,
        EXT_ALARM_warning,
        EXT_ALARM_fault
    };

    bool check = false;
    for(uint8_t i = 0; i < EXT_ALARM_MAX; i++) {
        signal_t *s = &mSignal[i];
        for(uint8_t j = 0; j < (sizeof(signal) / sizeof(signal[0])); j++) {
            check = check || (s->input && (s->signal == signal[j]));
        }
    }

    return check;
}

//
bool TExtAlarm::getAlarmOutputSignal(extAlarm_t signal) const
{
    bool value = false;

    Q_ASSERT(signal < EXT_ALARM_MAX);

    if (signal < EXT_ALARM_MAX) {
        value = mSignal[signal].output;
    }

    return value;
}

//
void TExtAlarm::resetSignal(extAlarm_t signal)
{
    Q_ASSERT(signal <= EXT_ALARM_MAX);

    if (signal < EXT_ALARM_MAX) {
        signal_t *s = &mSignal[signal];
//        s->input = s->valDef;
        s->output = s->input; // s->valDef;
    }
}

//
void TExtAlarm::resetSignalAll()
{
    for(uint8_t i = 0; i < EXT_ALARM_MAX; i++) {
        resetSignal(extAlarm_t(i));
    }
}

//
void TExtAlarm::reset(bool enable)
{
    if (enable) {
        resetSignalAll();
    }
    mReset = enable;
}

//
void TExtAlarm::setSignal(extAlarm_t signal, bool value)
{
    Q_ASSERT(signal < EXT_ALARM_MAX);

    if (signal < EXT_ALARM_MAX) {
        signal_t *s= &mSignal[signal];

        s->input = value;

        if (value) {
            s->output = value;
        } else {
            if ((s->alarmReset != ALARM_RESET_manual) &&
                (mAlarmReset != ALARM_RESET_manual)) {
                s->output = value;
            }
        }
    }
}


}

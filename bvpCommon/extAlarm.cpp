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
        .resetMode  = RESET_MODE_direct,
        .alarmReset = ALARM_RESET_MAX,
        .input      = false,
        .output     = false,
        .valDef     = false
    },
    //
    {
        .signal     = EXT_ALARM_test61850,
        .resetMode  = RESET_MODE_direct,
        .alarmReset = ALARM_RESET_MAX,
        .input      = false,
        .output     = false,
        .valDef     = false
    },
    //
    {
        .signal     = EXT_ALARM_channelFault,
        .resetMode  = RESET_MODE_direct,
        .alarmReset = ALARM_RESET_MAX,
        .input      = false,
        .output     = false,
        .valDef     = false
    },
    //
    {
        .signal     = EXT_ALARM_warning,
        .resetMode  = RESET_MODE_direct,
        .alarmReset = ALARM_RESET_MAX,
        .input      = false,
        .output     = false,
        .valDef     = false
    },
    //
    {
        .signal     = EXT_ALARM_fault,
        .resetMode  = RESET_MODE_off,
        .alarmReset = ALARM_RESET_MAX,
        .input      = false,
        .output     = false,
        .valDef     = true
    },
    //
    {
        .signal     = EXT_ALARM_comPrd,
        .resetMode  = RESET_MODE_direct,
        .alarmReset = ALARM_RESET_manual,
        .input      = false,
        .output     = false,
        .valDef     = false
    },
    //
    {
        .signal     = EXT_ALARM_comPrm,
        .resetMode  = RESET_MODE_direct,
        .alarmReset = ALARM_RESET_manual,                
        .input      = false,
        .output     = false,
        .valDef     = false
    },
    //
    {
        .signal     = EXT_ALARM_disablePrm,
        .resetMode  = RESET_MODE_direct,
        .alarmReset = ALARM_RESET_MAX,
        .input      = false,
        .output     = false,
        .valDef     = false
    }
};

TExtAlarm::TExtAlarm()
{
  for(uint8_t i = 0; i < EXT_ALARM_MAX; i++) {
    signal_t *s = &mSignal[i];
    s->input = s->valDef;
    s->output = s->input;
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
void TExtAlarm::reset(bool enable)
{
    for(uint8_t i = 0; i < EXT_ALARM_MAX; i++) {
       resetSignal(extAlarm_t(i));
    }

    mReset = enable;
}

//
void TExtAlarm::resetSignal(extAlarm_t signal)
{
    Q_ASSERT(signal <= EXT_ALARM_MAX);

    if (signal < EXT_ALARM_MAX) {
        signal_t *s = &mSignal[signal];
        s->output = s->input;
    }
}

//
void TExtAlarm::setSignal(extAlarm_t signal, bool value)
{
    Q_ASSERT(signal < EXT_ALARM_MAX);

    if (signal < EXT_ALARM_MAX) {
        signal_t *s= &mSignal[signal];

        s->input = value;

        if (mReset ) {
            if (s->resetMode != RESET_MODE_off) {
                s->output = value;
            }
        } else {
            if (value) {
                s->output = value;
            } else {
                if ((mAlarmReset != ALARM_RESET_manual) &&
                    (s->alarmReset != ALARM_RESET_manual)) {
                    s->output = value;
                }
            }
        }


    }
}


}

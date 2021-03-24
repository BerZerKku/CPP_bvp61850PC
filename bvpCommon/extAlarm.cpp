/*
 * extAlarm.cpp
 *
 *  Created on: March 23, 2021
 *      Author: bear
 */

#include <climits>
#include "extAlarm.hpp"

namespace BVP {

const TExtAlarm::signal_t TExtAlarm::mSignal[] = {
    {
        .signal     = EXT_ALARM_model61850,
        .resetMode  = RESET_MODE_direct,
        .alarmReset = ALARM_RESET_MAX,
        .valDef     = false
    },
    //
    {
        .signal     = EXT_ALARM_test61850,
        .resetMode  = RESET_MODE_direct,
        .alarmReset = ALARM_RESET_MAX,
        .valDef     = false
    },
    //
    {
        .signal     = EXT_ALARM_channelFault,
        .resetMode  = RESET_MODE_direct,
        .alarmReset = ALARM_RESET_MAX,
        .valDef     = false
    },
    //
    {
        .signal     = EXT_ALARM_warning,
        .resetMode  = RESET_MODE_direct,
        .alarmReset = ALARM_RESET_MAX,
        .valDef     = false
    },
    //
    {
        .signal     = EXT_ALARM_fault,
        .resetMode  = RESET_MODE_off,
        .alarmReset = ALARM_RESET_MAX,
        .valDef     = true
    },
    //
    {
        .signal     = EXT_ALARM_comPrd,
        .resetMode  = RESET_MODE_direct,
        .alarmReset = ALARM_RESET_manual,
        .valDef     = false
    },
    //
    {
        .signal     = EXT_ALARM_comPrm,
        .resetMode  = RESET_MODE_direct,
        .alarmReset = ALARM_RESET_manual,
        .valDef     = false
    },
    //
    {
        .signal     = EXT_ALARM_disablePrm,
        .resetMode  = RESET_MODE_direct,
        .alarmReset = ALARM_RESET_MAX,
        .valDef     = false
    }
};

TExtAlarm::TExtAlarm()
{
    Q_STATIC_ASSERT(sizeof(mAlarmIn) == sizeof(mAlarmOut));
    Q_STATIC_ASSERT((sizeof(mAlarmIn) * CHAR_BIT) >= EXT_ALARM_MAX);

    mAlarmIn = 0;
    for(uint8_t i = 0; i < EXT_ALARM_MAX; i++) {
        if (mSignal[i].valDef) {
            mAlarmIn |= (1 << i);
        }
    }

    mAlarmOut = mAlarmIn;
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
    constexpr uint16_t mask = (1 << EXT_ALARM_channelFault) |
                              (1 << EXT_ALARM_warning) |
                              (1 << EXT_ALARM_fault);

    Q_STATIC_ASSERT(sizeof(mask) == sizeof(mAlarmIn));

    return mAlarmIn & mask;
}

//
bool TExtAlarm::getAlarmOutputSignal(extAlarm_t signal) const
{
    bool value = false;

    Q_ASSERT(signal < EXT_ALARM_MAX);

    if (signal < EXT_ALARM_MAX) {
        value = mAlarmOut & (1 << signal);
    }

    return value;
}

//
void TExtAlarm::reset(bool enable)
{
    mAlarmOut = mAlarmIn;
    mReset = enable;
}

//
uint16_t TExtAlarm::setBitValue(uint16_t src, uint8_t num, bool value) const
{
    constexpr uint8_t maxnum = sizeof(src) * CHAR_BIT;
    uint16_t mask;

    Q_STATIC_ASSERT(sizeof (mask) == sizeof (src));
    Q_ASSERT(num < maxnum);

    if (num < maxnum) {
        mask =  uint16_t(1 << num);
        if (value) {
            src |= mask;
        } else {
            src &= ~mask;
        }
    }

    return src;
}

//
void TExtAlarm::setSignal(extAlarm_t signal, bool value)
{
    Q_ASSERT(signal < EXT_ALARM_MAX);

    if (signal < EXT_ALARM_MAX) {
        const signal_t *s= &mSignal[signal];


        mAlarmIn = setBitValue(mAlarmIn, signal, value);

        if (mReset ) {
            if (s->resetMode != RESET_MODE_off) {
                mAlarmOut = setBitValue(mAlarmOut, signal, value);
            }
        } else {
            if (value) {
                mAlarmOut = setBitValue(mAlarmOut, signal, value);
            } else {
                if ((mAlarmReset != ALARM_RESET_manual) &&
                    (s->alarmReset != ALARM_RESET_manual)) {
                    mAlarmOut = setBitValue(mAlarmOut, signal, value);
                }
            }
        }
    }
}


}

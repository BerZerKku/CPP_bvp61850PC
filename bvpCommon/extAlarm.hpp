#ifndef TEXT_ALARM_H
#define TEXT_ALARM_H

#include <cstdint>
#include "debug.hpp"
#include "hardware.hpp"
#include "param.h"

extern uint8_t getExtAlarmSignals();
extern void setExtAlarmSignals(uint16_t alarm);

namespace BVP {

enum extAlarmIn_t : uint8_t {
    EXT_ALARM_IN_channelFault   = 0x01,
    EXT_ALARM_IN_comPrm         = 0x02,
    EXT_ALARM_IN_comPrd         = 0x04,
    EXT_ALARM_IN_warning        = 0x08,
    EXT_ALARM_IN_fault          = 0x10,
};

enum extAlarmOut_t : uint16_t {
    EXT_ALARM_OUT_model61850    = 0x0001,
    EXT_ALARM_OUT_test61850     = 0x0002,
    EXT_ALARM_OUT_channelFault  = 0x0004,
    EXT_ALARM_OUT_warning       = 0x0008,
    EXT_ALARM_OUT_fault         = 0x0010,
    EXT_ALARM_OUT_comPrd        = 0x0020,
    EXT_ALARM_OUT_comPrm        = 0x0040,
    EXT_ALARM_OUT_disablePrm    = 0x0080
};

class TExtAlarm {

   public:
    ///
    TExtAlarm() : mParam(TParam::getInstance())  {
        assert(mParam != nullptr);
    }

    ///
    ~TExtAlarm() {}

    /**
     * @brief proc
     */
    uint16_t getAlarmOutSignals(uint8_t insignals) {
        bool check;
        alarmReset_t alarmreset = getAlarmReset();

        check = insignals & EXT_ALARM_IN_fault;
        setSignal(alarmreset, check, EXT_ALARM_OUT_fault, alarmreset);

        check = insignals & EXT_ALARM_IN_warning;
        setSignal(alarmreset, check, EXT_ALARM_OUT_warning, alarmreset);

        check = insignals & EXT_ALARM_IN_comPrd;
        setSignal(alarmreset, check, EXT_ALARM_OUT_comPrd, alarmreset);

        check = insignals & EXT_ALARM_IN_comPrm;
        setSignal(alarmreset, check, EXT_ALARM_OUT_comPrm, alarmreset);

        check = insignals & EXT_ALARM_IN_channelFault;
        setSignal(alarmreset, check, EXT_ALARM_OUT_channelFault, alarmreset);

        setSignal(alarmreset, false, EXT_ALARM_OUT_model61850, alarmreset);

        setSignal(alarmreset, false, EXT_ALARM_OUT_test61850, alarmreset);

        check = getDisablePrm() == ON_OFF_on;
        setSignal(alarmreset, check, EXT_ALARM_OUT_disablePrm , ALARM_RESET_auto);

        assert(mParam->setValue(PARAM_alarmReset, SRC_int, alarmreset));

        QDEBUG("alarmreset = " << alarmreset << ", insignals = " << insignals);

        return alarmreset;
    }

   private:
    ///
    TParam * const mParam = nullptr;

    /**
     * @brief getAlarmReset
     * @return
     */
    alarmReset_t getAlarmReset() const {
        alarmReset_t reset = ALARM_RESET_manual;

        bool ok = true;
        uint32_t value = mParam->getValue(PARAM_alarmReset, SRC_int, ok);

        if ((ok) && (value < ALARM_RESET_MAX)) {
            reset = static_cast<alarmReset_t> (value);
        }

        return reset;
    }

    /**
     * @brief getDisablePrm
     * @return
     */
    switchOff_t getDisablePrm() const {
        bool ok = true;
        uint32_t value = mParam->getValue(PARAM_blkComPrmAll, SRC_int, ok);

        return ok ? switchOff_t (value) : ON_OFF_off;
    }

    /**
     * @brief getExtAlarm
     * @return
     */
    uint16_t getExtAlarm() const {
        bool ok = true;
        uint16_t value = mParam->getValue(PARAM_extAlarm, SRC_int, ok);

        return ok ? value : static_cast<uint16_t> (EXT_ALARM_OUT_fault);
    }

    /**
     * @brief setSignal
     * @param value
     * @param signal
     * @param reset
     * @return
     */
    uint16_t setSignal(uint16_t value, bool check,
                       extAlarmOut_t signal, alarmReset_t reset) const {
        if (check) {
            value |= signal;
        } else {
            if (reset == ALARM_RESET_auto) {
                value &= ~signal;
            }
        }

        return value;
    }

#ifdef TEST_FRIENDS
    TEST_FRIENDS;
#endif
};

} // namespace BVP

#endif // TEXT_ALARM_H

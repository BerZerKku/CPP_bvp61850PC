#ifndef TEXT_ALARM_H
#define TEXT_ALARM_H

#include <cstdint>
#include "debug.hpp"
#include "hardware.hpp"
#include "param.h"

namespace BVP {

/// Сигналы сигнализации
enum extAlarm_t {
    EXT_ALARM_model61850 = 0,
    EXT_ALARM_test61850,
    EXT_ALARM_channelFault,
    EXT_ALARM_warning,
    EXT_ALARM_fault,
    EXT_ALARM_comPrd,
    EXT_ALARM_comPrm,
    EXT_ALARM_disablePrm,
    //
    EXT_ALARM_MAX
};

class TExtAlarm {

    /// Состояние сигналов по умолчанию
    const uint16_t kAlarmDefault = (1 << EXT_ALARM_fault);
    /// Маска для всех задействованных сигналов
    const uint16_t kAlarmMask = (1 << EXT_ALARM_MAX) - 1;

public:

    const alarmReset_t kAlarmResetDefault = ALARM_RESET_manual;
    const switchOff_t kDisablePrmDefault = ON_OFF_off;

    ///
    TExtAlarm() {}

    ///
    ~TExtAlarm() {}

    /**
     * @brief Устанавливает режим сброса сигнализации.
     *
     *  В случае установки ошибочного значения оно будет заменено на
     *  значение по умолчанию \a kAlarmResetDefault.
     *
     * @param[in] reset Режим сброса сигнализации.
     */
    void setAlarmReset(alarmReset_t reset) {
        if (reset >= ALARM_RESET_MAX) {
            reset = kAlarmResetDefault;
        }

        mAlarmReset = reset;
    }

    /// Возвращает режим сброса сигнализации.
    alarmReset_t getAlarmReset() const {
        return mAlarmReset;
    }

    /**
     * @brief Устанавливает все сигналы сигнализации разом.
     *
     * Значения передаются побитно в соответствии с \a extAlarm_t.
     * Все "лишние" биты будут обнулены.
     *
     * @param[in] alarm Синалы сигнализации.
     */
    void setAlarmOutput(uint16_t alarm) {
        mAlarm = alarm & kAlarmMask;
    }

    /**
     * @brief Возвращает сигналы сигнализации.
     *
     * Значения установлены побитно в соответствии с \a extAlarm_t.
     *
     * @return Сигналы сигнализации.
     */
    uint16_t getAlarmOutput() const {
        return mAlarm;
    }

    /**
     * @brief Устанавливает значение сигнала сигнализации.
     * @param[in] signal Сигнал.
     * @param[in] value Значение (bool - активный)
     * @return Возвращает true для корректного значения сигнала.
     */
    bool setAlarmInputSignal(extAlarm_t signal, bool value) {
        alarmReset_t reset = getAlarmReset(signal);

        setSignal(signal, value, reset);

        return (signal <= EXT_ALARM_MAX);
    }

    /**
     * @brief Возвращает значение сигнала сигнализации.
     * @param[in] signal Сигнал.
     * @return Возвращает true если сигнал активен, иначе false.
     */
    bool getAlarmOutputSignal(extAlarm_t signal) const {
        return signal < EXT_ALARM_MAX ? (mAlarm & (1 << signal)) : false;

    }

private:

    /// Режим сброса сигнализации
    alarmReset_t mAlarmReset = kAlarmResetDefault;
    /// Сигналы сигнализации
    uint16_t mAlarm = kAlarmDefault;

    /**
     * @brief Возвращает режим сброса сигнализации для сигнала.
     *
     * Особенные сигналы:
     * - \a EXT_ALARM_comPrd всегда \a ALARM_RESET_manual
     * - \a EXT_ALARM_comPrm всегда \a ALARM_RESET_manual
     *
     * @param[in] signal Сигнал.
     * @return Режим сброса.
     */
    alarmReset_t getAlarmReset(extAlarm_t signal) const {
        alarmReset_t reset = mAlarmReset;

        if ((signal == EXT_ALARM_comPrd) || (signal == EXT_ALARM_comPrm)) {
            reset = ALARM_RESET_manual;
        }

        return reset;
    }

    /**
     * @brief Устанавливает новое значение сигнала
     * @param[in] signal Сигнал.
     * @param[in] value Значение (bool - активный)
     * @param[in] reset Режим сброса для сигнала.
     * @return Значение всех сигналов побитно согласно \a extAlarm_t.
     */
    uint16_t setSignal(extAlarm_t signal, bool value, alarmReset_t reset) {
        assert(signal <= EXT_ALARM_MAX);

        if (signal < EXT_ALARM_MAX) {
            if (value) {
                mAlarm |= (1 << signal);
            } else {
                if (reset == ALARM_RESET_auto) {
                    mAlarm &= ~(1 << signal);
                }
            }
        }

        return mAlarm;
    }

#ifdef TEST_FRIENDS
    TEST_FRIENDS;
#endif
};

} // namespace BVP

#endif // TEXT_ALARM_H

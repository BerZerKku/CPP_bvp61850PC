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

    /// Режим сигнала во время сброса
    enum resetMode_t {
        RESET_MODE_off = 0, ///< Запоминает последний сигнал на входе.
        RESET_MODE_direct,  ///< Выход сигнала повторяет вход
        ///
        RESET_MODE_MAX
    };

    /// Свойства сигнала
    struct signal_t {
        const extAlarm_t signal;        ///< Сигнал.
        const resetMode_t resetMode;    ///< Режим работы сигнала во время сброса.
        const alarmReset_t alarmReset;  ///< Режим сброса сигнала (ALARM_RESET_MAX - согласно текущим настройкам).
        const bool valDef;              ///< Значение сигнала по умолчанию.
    };

public:

    ///
    TExtAlarm();

    ///
    ~TExtAlarm() {}

    /**
     * @brief Устанавливает режим сброса сигнализации.
     * @param[in] reset Режим сброса сигнализации.
     */
    void setAlarmReset(alarmReset_t reset) {
        if (reset > ALARM_RESET_MAX) {
            reset = ALARM_RESET_MAX;
        }

        mAlarmReset = reset;
    }

    /**
     * @brief Устанавливает значение сигнала сигнализации.
     * @param[in] signal Сигнал.
     * @param[in] value Значение (bool - активный)
     * @return Возвращает true для корректного значения сигнала.
     */
    bool setAlarmInputSignal(extAlarm_t signal, bool value);

    /**
     * @brief Проверяет наличие сигналов которые требуют сброса устройства.
     * @param[in] signal Сигнал.
     * @return Возвращает true для корректного значения сигнала.
     */
    bool isSignalForDeviceReset();

    /**
     * @brief Возвращает значение сигнала сигнализации.
     * @param[in] signal Сигнал.
     * @return Возвращает true если сигнал активен, иначе false.
     */
    bool getAlarmOutputSignal(extAlarm_t signal) const;

    /**
     * @brief Сброс сигнализции.
     * Во время сброса сигналы ведут себя согласно установленным для них
     * режимам \a resetMode_t.
     * @param[in] enable Сброс сигнализации
     */
    void reset(bool enable);

    /**
     * @brief Проверка начличия сброса.
     * @return true если сигнализация в сбросе, иначе false.
     */
    bool isReset() const {
        return mReset;
    }

private:

    static const signal_t mSignal[EXT_ALARM_MAX];   /// Настройки сигналов.
    bool mReset = false;                            /// Сброс сигнализации.
    alarmReset_t mAlarmReset = ALARM_RESET_MAX;     /// Режим сброса сигнализации
    uint16_t mAlarmIn;                              /// Состояние сигналов на входе.
    uint16_t mAlarmOut;                             /// Состояние сигналов на выходе.

    /**
     * @brief Устанавливает значение бита.
     * @param[in] src Начальное значение.
     * @param[in] num Номер бита [0..15].
     * @param[in] value Значение бита.
     * @return Значение с устновленным битом.
     */
    uint16_t setBitValue(uint16_t src, uint8_t num, bool value) const;

    /**
     * @brief Устанавливает новое значение сигнала
     * Во время сброса поведение выходных сигналов зависит от resetMode.
     * В другое время от alarmReset.
     * @param[in] signal Сигнал.
     * @param[in] value Значение (bool - активный)
     */
    void setSignal(extAlarm_t signal, bool value);

#ifdef TEST_FRIENDS
    TEST_FRIENDS;
#endif
};

} // namespace BVP

#endif // TEXT_ALARM_H

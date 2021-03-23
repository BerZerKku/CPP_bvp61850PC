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
        RESET_MODE_off = 0, ///< Всегда сброшен.
        RESET_MODE_direct,  ///< Выход сигнала повторяет вход
        ///
        RESET_MODE_MAX
    };

    /// Режим работы сигнала
    enum mode_t {
        MODE_reset = 0,    /// Сброс
        MODE_work          /// Работа
    };

    /// Свойства сигнала
    struct signal_t {
        const extAlarm_t signal;
        const resetMode_t resetMode;
        const alarmReset_t alarmReset;
        mode_t mode;
        bool input;
        bool output;
        const bool valDef;
    };

    /// Состояние сигналов по умолчанию
    const uint16_t kAlarmDefault = 0;

public:

    /// Состояние режима сброса по умолчанию
    const disablePrm_t kDisablePrmDefault = DISABLE_PRM_enable;

    ///
    TExtAlarm();

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
     * @brief Сбрасывает значение для указанного сигнала.
     * @param[in] signal Сигнал.
     */
    void resetSignal(extAlarm_t signal);

    /**
     * @brief Сбрасывает значение для всех сигналов.
     */
    void resetSignalAll();

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

    /// Настройки сигналов.
    static signal_t mSignal[EXT_ALARM_MAX];

    /// Сброс сигнализации.
    bool mReset = false;

    /// Режим сброса сигнализации
    alarmReset_t mAlarmReset = ALARM_RESET_MAX;

    /**
     * @brief Устанавливает новое значение сигнала
     * Во время сброса все сигналы работают в автоматическом сигнале.
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

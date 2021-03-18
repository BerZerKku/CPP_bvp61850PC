#ifndef TEXT_ALARM_H
#define TEXT_ALARM_H

#include <cstdint>
#include "debug.hpp"
#include "hardware.hpp"
#include "param.h"

namespace BVP {

/// ������� ������������
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

    /// ��������� �������� �� ���������
    const uint16_t kAlarmDefault = 0;

    /// ����� ��� ���� ��������������� ��������
    const uint16_t kAlarmMask = (1 << EXT_ALARM_MAX) - 1;

public:

    /// ����� ������ �������� �� ���������
    const alarmReset_t kAlarmResetModeDefault = ALARM_RESET_manual;

    /// ��������� ������ ������ �� ���������
    const disablePrm_t kDisablePrmDefault = DISABLE_PRM_enable;

    ///
    TExtAlarm() {}

    ///
    ~TExtAlarm() {}

    /**
     * @brief ������������� ����� ������ ������������.
     *
     *  � ������ ��������� ���������� �������� ��� ����� �������� ��
     *  �������� �� ��������� \a kAlarmResetDefault.
     *
     * @param[in] reset ����� ������ ������������.
     */
    void setAlarmReset(alarmReset_t reset) {
        if (reset >= ALARM_RESET_MAX) {
            reset = kAlarmResetModeDefault;
        }

        mAlarmReset = reset;
    }

    /// ���������� ����� ������ ������������.
    alarmReset_t getAlarmReset() const {
        return mAlarmReset;
    }

    /**
     * @brief ������������� ��� ������� ������������ �����.
     *
     * �������� ���������� ������� � ������������ � \a extAlarm_t.
     * ��� "������" ���� ����� ��������.
     *
     * @param[in] alarm ������ ������������.
     */
    void setAlarmOutput(uint16_t alarm) {
        mAlarm = alarm & kAlarmMask;
    }

    /**
     * @brief ���������� ������� ������������.
     *
     * �������� ����������� ������� � ������������ � \a extAlarm_t.
     *
     * @return ������� ������������.
     */
    uint16_t getAlarmOutput() const {
        return mAlarm;
    }

    /**
     * @brief ������������� �������� ������� ������������.
     * @param[in] signal ������.
     * @param[in] value �������� (bool - ��������)
     * @return ���������� true ��� ����������� �������� �������.
     */
    bool setAlarmInputSignal(extAlarm_t signal, bool value) {
        alarmReset_t reset = getAlarmReset(signal);

        setSignal(signal, value, reset);

        return (signal <= EXT_ALARM_MAX);
    }

    /**
     * @brief ���������� �������� ������� ������������.
     * @param[in] signal ������.
     * @return ���������� true ���� ������ �������, ����� false.
     */
    bool getAlarmOutputSignal(extAlarm_t signal) const {
        return signal < EXT_ALARM_MAX ? (mAlarm & (1 << signal)) : false;

    }

private:

    /// ����� ������ ������������
    alarmReset_t mAlarmReset = kAlarmResetModeDefault;
    /// ������� ������������
    uint16_t mAlarm = kAlarmDefault;

    /**
     * @brief ���������� ����� ������ ������������ ��� �������.
     *
     * ��������� �������:
     * - \a EXT_ALARM_comPrd ������ \a ALARM_RESET_manual
     * - \a EXT_ALARM_comPrm ������ \a ALARM_RESET_manual
     *
     * @param[in] signal ������.
     * @return ����� ������.
     */
    alarmReset_t getAlarmReset(extAlarm_t signal) const {
        alarmReset_t reset = mAlarmReset;

        if ((signal == EXT_ALARM_comPrd) || (signal == EXT_ALARM_comPrm)) {
            reset = ALARM_RESET_manual;
        }

        return reset;
    }

    /**
     * @brief ������������� ����� �������� �������
     * @param[in] signal ������.
     * @param[in] value �������� (bool - ��������)
     * @param[in] reset ����� ������ ��� �������.
     * @return �������� ���� �������� ������� �������� \a extAlarm_t.
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

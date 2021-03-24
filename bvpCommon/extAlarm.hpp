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

    /// ����� ������� �� ����� ������
    enum resetMode_t {
        RESET_MODE_off = 0, ///< ���������� ��������� ������ �� �����.
        RESET_MODE_direct,  ///< ����� ������� ��������� ����
        ///
        RESET_MODE_MAX
    };

    /// �������� �������
    struct signal_t {
        const extAlarm_t signal;        ///< ������.
        const resetMode_t resetMode;    ///< ����� ������ ������� �� ����� ������.
        const alarmReset_t alarmReset;  ///< ����� ������ ������� (ALARM_RESET_MAX - �������� ������� ����������).
        const bool valDef;              ///< �������� ������� �� ���������.
    };

public:

    ///
    TExtAlarm();

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
        if (reset > ALARM_RESET_MAX) {
            reset = ALARM_RESET_MAX;
        }

        mAlarmReset = reset;
    }

    /**
     * @brief ������������� �������� ������� ������������.
     * @param[in] signal ������.
     * @param[in] value �������� (bool - ��������)
     * @return ���������� true ��� ����������� �������� �������.
     */
    bool setAlarmInputSignal(extAlarm_t signal, bool value);

    /**
     * @brief ��������� ������� �������� ������� ������� ������ ����������.
     * @param[in] signal ������.
     * @return ���������� true ��� ����������� �������� �������.
     */
    bool isSignalForDeviceReset();

    /**
     * @brief ���������� �������� ������� ������������.
     * @param[in] signal ������.
     * @return ���������� true ���� ������ �������, ����� false.
     */
    bool getAlarmOutputSignal(extAlarm_t signal) const;

    /**
     * @brief ����� �����������.
     * �� ����� ������ ������� ����� ���� �������� ������������� ��� ���
     * ������� \a resetMode_t.
     * @param[in] enable ����� ������������
     */
    void reset(bool enable);

    /**
     * @brief �������� �������� ������.
     * @return true ���� ������������ � ������, ����� false.
     */
    bool isReset() const {
        return mReset;
    }

private:


    static const signal_t mSignal[EXT_ALARM_MAX];   /// ��������� ��������.
    bool mReset = false;                            /// ����� ������������.
    alarmReset_t mAlarmReset = ALARM_RESET_MAX;     /// ����� ������ ������������
    uint16_t mAlarmIn;                              /// ��������� �������� �� �����.
    uint16_t mAlarmOut;                             /// ��������� �������� �� ������.

    /**
     * @brief ������������� �������� ����.
     * @param[in] src ��������� ��������.
     * @param[in] num ����� ���� [0..15].
     * @param[in] value �������� ����.
     * @return �������� � ������������ �����.
     */
    uint16_t setBitValue(uint16_t src, uint8_t num, bool value) const;

    /**
     * @brief ������������� ����� �������� �������
     * �� ����� ������ ��������� �������� �������� ������� �� resetMode.
     * � ������ ����� �� alarmReset.
     * @param[in] signal ������.
     * @param[in] value �������� (bool - ��������)
     */
    void setSignal(extAlarm_t signal, bool value);

#ifdef TEST_FRIENDS
    TEST_FRIENDS;
#endif
};

} // namespace BVP

#endif // TEXT_ALARM_H

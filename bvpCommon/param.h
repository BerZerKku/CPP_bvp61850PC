#ifndef TPARAM_H
#define TPARAM_H

#include <stdint.h>
#include "debug.hpp"
#include "hardware.hpp"

namespace BVP {

enum param_t {
    //
    PARAM_control = 0,      ///< ������� ����������.
    // ������� ���������
    PARAM_error,            ///< ���� ������� �������������.
    PARAM_warning,          ///< ���� ������� ��������������.
    PARAM_defError,         ///< ������������� ������.
    PARAM_defWarning,       ///< �������������� ������.
    PARAM_prmError,         ///< ������������� ���������.
    PARAM_prmWarning,       ///< ������������� ���������.
    PARAM_prm2Error,        ///< ������������� ��������� 2.
    PARAM_prm2Warning,      ///> ������������� ��������� 2.
    PARAM_prdError,         ///< ������������� �����������.
    PARAM_prdWarning,       ///< ������������� �����������.
    PARAM_glbError,         ///< ������������� �����.
    PARAM_glbWarning,       ///< ������������� �����.
    PARAM_defRemoteError,   ///< ������������� ������ ���������� ��������.
    PARAM_prmRemoteError,   ///< ������������� ��������� ���������� ��������.
    PARAM_prdRemoteError,   ///< ������������� ����������� ���������� ��������.
    PARAM_glbRemoteError,   ///< ������������� ����� ���������� ��������.
    // ���� � �����
    PARAM_dateYear,         ///< ���.
    PARAM_dateMonth,        ///< �����.
    PARAM_dateDay,          ///< ����.
    PARAM_timeHour,         ///< ����.
    PARAM_timeMin,          ///< ������.
    PARAM_timeSec,          ///< �������.
    PARAM_timeMSec,         ///< ������������.
    // ����� ���������
    PARAM_alarmReset,       ///< ����� ������ ������������.
    // ���������� ���������
    PARAM_extAlarm,         ///< ������� ������������.
    // ��������� ������ ����������� ������
    PARAM_dirControl,       ///< ���������� ������� (SAC2).
    PARAM_blkComPrmAll,     ///< ���������� ���� ������� ��������� (SAC1).
    PARAM_blkComPrmDir,     ///< ���������� ����������� ������ ��������� (SAnn.x)
    PARAM_blkComPrm32to01,  ///< ������������� ������� ��������� � 1 �� 32.
    PARAM_blkComPrm64to33,  ///< ������������� ������� ��������� � 33 �� 64.
    PARAM_blkComPrd32to01,  ///< ������������� ������� ����������� � 1 �� 32.
    PARAM_blkComPrd64to33,  ///< ������������� ������� ����������� � 33 �� 64.
    // ������ ������ ����������� ������.
    PARAM_vpBtnSAnSbSac,
    PARAM_vpBtnSA32to01,
    PARAM_vpBtnSA64to33,
    //
    PARAM_MAX
};

/// �������� ������� � ����������.
enum src_t {
    SRC_pi = 0, ///< ���-��
    SRC_pc,     ///< ������������ ��
    SRC_acs,    ///< ���
    SRC_vkey,   ///< ������ ����������� ������
    SRC_int,    ///< ���������� ��������� ���������
    //
    SRC_MAX
};

/// ������� ����������.
enum ctrl_t {
    CTRL_resetIndication = 0,
    CTRL_resetErrors,
    CTRL_reset,
    //
    CTRL_MAX

};

/// ���������� �������
enum dirControl_t {
    DIR_CONTROL_local = 0,
    DIR_CONTROL_remote,
    //
    DIR_CONTROL_MAX
};

/// ����� ������������
enum alarmReset_t {
    ALARM_RESET_auto = 0,
    ALARM_RESET_manual,
    //
    ALARM_RESET_MAX
};

///
enum switchOff_t {
    ON_OFF_off = 0, ///<
    ON_OFF_on,
    //
    ON_OFF_MAX
};

///

/** ����� ����������.
 *
 *  ������ �� ������� �������� (single-tone).����������� ������� � protected,
 *  � �������� ������� new ���������� ��� ������ ������ getInstance).
 *  �� �.�. ������ ������ ������������ ������, ��� ����������� ������ �
 *  ��� ������ getInstance ��� ������������� �������� ������� �������.
 *
 *  � ������ ������ ��� ������������ ��������� ���������� ����� ���������
 *  �� ��������� ������ (getInstance).
 *
 * ��������� ������ ���-���� ������, �� ��������� �����!!!
 */
class TParam {
    /// ���������.
    static TParam mParam;

    /// ���� �������� ���������.
    struct paramFields_t {
        param_t param;    ///< ��������
        bool isValue;     ///< ���� ������� ���������� ��������.
        uint32_t rValue;  ///< ��������� ��������.
        uint32_t wValue;  ///< �������� ��� ������.
        ///< ���. ��������� ��� ��������� �������� ���������.
        bool (*set) (param_t, BVP::src_t, uint32_t&);
        ///< ���. ��������� ��� ������ �������� ���������.
        bool (*get) (param_t, BVP::src_t, uint32_t&);
    };

    enum vpBtnControl_t {
        VP_BTN_CONTROL_sac1 = 0x00000001,
        VP_BTN_CONTROL_sac2 = 0x00000002,
        VP_BTN_CONTROL_sb   = 0x00000004,
        VP_BTN_CONTROL_san  = 0x0000FF00
    };

public:
    /// �����������.
    TParam();
    /// ����������� ����������� ��������.
    TParam(TParam &other) = delete;
    /// ����������� ������������ ��������.
    void operator=(const TParam&) = delete;

    /** ���������� ��������� ������ ����������.
   *
   *  @return ����� ����������.
   */
    static TParam* getInstance();

    /** ��������� ������� �������������� �������� ���������.
   *
   *  @param[in] param ��������.
   *  @return true ���� �������� ���� �����������, ����� false.
   */
    bool isValueSet(param_t param) const;

    /** ��������� ����������� ������ ��������� �� ���������� ���������.
   *
   *  @param[in] param ��������.
   *  @param[in] src �������� �������.
   *  @return true ���� ������ ���������, ����� false.
   */
    bool isAccessRead(param_t param, src_t src) const;

    /** ��������� ����������� ��������� ��������� �� ���������� ���������.
   *
   *  @param[in] param ��������.
   *  @param[in] src �������� �������.
   *  @return true ���� ������ ���������, ����� false.
   */
    bool isAccessSet(param_t param, src_t src) const;

    /** ���������� �������� ���������.
   *
   *  �������� �� ����� ��������, ���� �������� ��� �� ���������� ���
   *  �� ���������� ���� �������.
   *
   *  @param[in] param ��������.
   *  @param[in] src �������� �������.
   *  @param[out] ok true ���� �������� �������, ����� false.
   *  @return �������� ���������.
   */
    uint32_t getValue(param_t param, src_t src, bool &ok);

    /** ���������� ��������� �������� ���������.
   *
   *  @param[in] param ��������.
   *  @return �������� ���������.
   */
    uint32_t getValueR(param_t param);

    /** ���������� �������� ��������� ��� ������.
   *
   *  @param[in] param ��������.
   *  @return  �������� ���������.
   */
    uint32_t getValueW(param_t param);

    /** ������������� �������� ���������.
   *
   *  @param[in] param ��������.
   *  @param[in] src �������� �������.
   *  @param[in] value �������� ���������.
   *  @return true ���� ����������� ����� ��������, ����� false.
   */
    bool setValue(param_t param, src_t src, uint32_t value);

private:
    /// �������� ����������.
    static paramFields_t params[PARAM_MAX];

    friend bool getBlkComPrm(param_t param, src_t src, uint32_t &value);
    friend bool getControl(param_t param, src_t src, uint32_t &value);

    friend bool setBlkComPrmAll(param_t param, src_t src, uint32_t &value);
    friend bool setBlkComPrm(param_t param, src_t src, uint32_t &value);
    friend bool setBtnSA(param_t param, src_t src, uint32_t &value);
    friend bool setControl(param_t param, src_t src, uint32_t &value);
    friend bool setError(param_t param, src_t src, uint32_t &value);
    friend bool setDirControl(param_t param, src_t src, uint32_t &value);
    friend bool setVpBtnSAnSbSac(param_t param, src_t src, uint32_t &value);
    friend bool setWarning(param_t param, src_t src, uint32_t &value);


    void setLocalValue(param_t param, uint32_t value);
};

} // namespace BVP

#endif // TPARAM_H
#ifndef TPARAM_H
#define TPARAM_H

#include <stdint.h>
#include "debug.hpp"
#include "hardware.hpp"

namespace BVP {

// FIXME ��� ������ ������ ���� � �� ��� ���!!!
// NOTE ������ �� ������� �� ���� ���� (major << 8) + (minor)
static const uint8_t versionMajor = 100;    ///< ������ �� (major)
static const uint8_t versionMinor = 1;      ///< ������ �� (minor)


enum param_t {
    PARAM_version = 0,      ///< ������ �������� ����������
    // ������� ���������
    PARAM_error,            ///< ���� ������� �������������
    PARAM_warning,          ///< ���� ������� ��������������
    PARAM_defError,         ///< ������������� ������
    PARAM_defWarning,       ///< �������������� ������
    PARAM_prmError,         ///< ������������� ���������
    PARAM_prmWarning,       ///< ������������� ���������
    PARAM_prm2Error,        ///< ������������� ��������� 2
    PARAM_prm2Warning,      ///> ������������� ��������� 2
    PARAM_prdError,         ///< ������������� �����������
    PARAM_prdWarning,       ///< ������������� �����������
    PARAM_glbError,         ///< ������������� �����
    PARAM_glbWarning,       ///< ������������� �����
    PARAM_defRemoteError,   ///< ������������� ������ ���������� ��������
    PARAM_prmRemoteError,   ///< ������������� ��������� ���������� ��������
    PARAM_prdRemoteError,   ///< ������������� ����������� ���������� ��������
    PARAM_glbRemoteError,   ///< ������������� ����� ���������� ��������
    // ���� � �����
    PARAM_dateYear,         ///< ���
    PARAM_dateMonth,        ///< �����
    PARAM_dateDay,          ///< ����
    PARAM_timeHour,         ///< ����
    PARAM_timeMin,          ///< ������
    PARAM_timeSec,          ///< �������
    PARAM_timeMSec,         ///< ������������
    // ����� ���������
    PARAM_alarmResetMode,   ///< ����� ������ ������������
    // ���������� ���������
    PARAM_extAlarm,         ///< ������� ������������
    PARAM_alarmRstCtrl,     ///< ���������� "����� ������������"
    PARAM_control,          ///< ������� ����������
    PARAM_debug1,           ///< �������� ��� ������� �� 1
    PARAM_debug2,           ///< �������� ��� ������� �� 2
    // ��������� ������ ����������� ������
    PARAM_versionVp,        ///< ������ �� ������ ��
    PARAM_dirControl,       ///< ���������� ������� (SAC2)
    PARAM_blkComPrmAll,     ///< ���������� ���� ������� ��������� (SAC1)
    PARAM_blkComPrmDir,     ///< ���������� ����������� ������ ��������� (SAnn.x)
    PARAM_comPrmBlk08to01,  ///< ������������� ������� ��������� � 8 �� 1
    PARAM_comPrmBlk16to09,  ///< ������������� ������� ��������� � 16 �� 9
    PARAM_comPrmBlk24to17,  ///< ������������� ������� ��������� � 24 �� 17
    PARAM_comPrmBlk32to25,  ///< ������������� ������� ��������� � 32 �� 25
    PARAM_blkComPrm64to33,  ///< ������������� ������� ��������� � 33 �� 64
    PARAM_comPrdBlk08to01,  ///< ������������� ������� ����������� � 8 �� 1
    PARAM_comPrdBlk16to09,  ///< ������������� ������� ����������� � 16 �� 9
    PARAM_comPrdBlk24to17,  ///< ������������� ������� ����������� � 24 �� 17
    PARAM_comPrdBlk32to25,  ///< ������������� ������� ����������� � 32 �� 25
    PARAM_blkComPrd64to33,  ///< ������������� ������� ����������� � 33 �� 64
    // ������ ������ ����������� ������.
    PARAM_vpBtnSAnSbSac,    ///< ������� ��������� ������ SA.m, SB, SAC
    PARAM_vpBtnSA32to01,    ///< ������� ��������� ������ c SA32  �� SA1
    PARAM_vpBtnSA64to33,    ///< ������� ��������� ������ c SA64  �� SA33
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

/** ������� ����������.
 *
 *  ������� ������������ �������� ������ ������ � �����!
 *  ����� ����� ��� ���� �������� ��� ���������.
 */
enum ctrl_t {
    CTRL_resetComInd = 0,   ///< ����� ��������� ������.
    CTRL_resetFault,        ///< ����� ��������������. ���� == CTRL_reset
    CTRL_resetSelf,         ///< ����� �������. ���������� ������� � �����.
    //
    CTRL_MAX
};

/// ���������� �������
enum dirControl_t {
    DIR_CONTROL_local = 0,  ///< �������
    DIR_CONTROL_remote,     ///< �������������
    //
    DIR_CONTROL_MAX
};

/// ����� ������������
enum alarmReset_t {
    ALARM_RESET_auto = 0,   ///< ��������������
    ALARM_RESET_manual,     ///< ������
    //
    ALARM_RESET_MAX
};

/// ���������� ������� ������������
enum alarmRstCtrl_t {
    ALARM_RST_CTRL_no = 0,  ///< ���.
    ALARM_RST_CTRL_pressed, ///< ����� ������������� "����� ������������".
    //
    ALARM_RST_CTRL_MAX
};

/// ��������� ��������� "����� ��� (SAC1)"
enum disablePrm_t {
    DISABLE_PRM_disable = 0,    ///< ��� �������
    DISABLE_PRM_enable,         ///< ��� ������
    //
    DISABLE_PRM_MAX
};

///
enum switchOff_t {
    ON_OFF_off = 0, ///< ������������� ��������
    ON_OFF_on,      ///< ������������� �������
    //
    ON_OFF_MAX
};

///
enum vpBtnControl_t {
    VP_BTN_CONTROL_sac1 = 0,
    VP_BTN_CONTROL_sac2,
    VP_BTN_CONTROL_sb,
    VP_BTN_CONTROL_san1  = 8,
    VP_BTN_CONTROL_san2,
    VP_BTN_CONTROL_san3,
    VP_BTN_CONTROL_san4,
    VP_BTN_CONTROL_san5,
    VP_BTN_CONTROL_san6,
    VP_BTN_CONTROL_san7,
    VP_BTN_CONTROL_san8,
    //
    VP_BTN_CONTROL_MAX
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
        const param_t param;      ///< ��������
        const uint32_t source;    ///< �������� ��������� ������� �������� (����).
        bool isSet;         ///< ���� ������� ���������� ��������.
        bool isModified;    ///< ���� ������� ������ �������.
        uint32_t rValue;    ///< ��������� ��������.
        uint32_t wValue;    ///< �������� ��� ������.
        ///< ���. ��������� ��� ��������� �������� ���������.
        bool (* const set) (param_t, BVP::src_t, uint32_t&);
        ///< ���. ��������� ��� ������ �������� ���������.
        bool (* const get) (param_t, BVP::src_t, uint32_t&);
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
    bool isSet(param_t param) const;

    /**
     * @brief isModified
     * @param param
     * @return
     */
    bool isModified(param_t param) const;

    /** ��������� ������� ������� � ��������� �� ���������� ���������.
     *
     *  @param[in] param ��������.
     *  @param[in] src �������� �������.
     *  @return true ���� ������ ��������, ����� false.
     */
    bool isAccess(param_t param, src_t src) const;

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
    uint32_t getValueR(param_t param) const;

    /** ���������� �������� ��������� ��� ������.
     *
     *  @param[in] param ��������.
     *  @return  �������� ���������.
     */
    uint32_t getValueW(param_t param) const;

    /** ������������� �������� ���������.
     *
     *  @param[in] param ��������.
     *  @param[in] src �������� �������.
     *  @param[in] value �������� ���������.
     *  @return true ���� ����������� ����� ��������, ����� false.
     */
    bool setValue(param_t param, src_t src, uint32_t value);

    /** ����� ���� ���������� � �������� ���������.
     *  ����� ������ ����� ����������� ��� ���������� ���������.
     */
    void reset();

private:
    /// �������� ����������.
    static paramFields_t params[PARAM_MAX];
    const src_t mSrc = SRC_int;

    friend bool setError(param_t param, src_t src, uint32_t &value);
    friend bool setWarning(param_t param, src_t src, uint32_t &value);

    /**
     * @brief ������������ �������� ���������.
     * ���� �������� ������� ������� � ���������� ��������� ���������:
     * - �� ����� ����������� ����� �������� rValue;
     * - ����� ����� ����������� �������� ��� ������ wValue.
     * @param[in] param ��������.
     * @param[in] src �������� �������.
     * @param[in] value �������� ���������
     */
    void setLocalValue(param_t param, src_t src, uint32_t value);
};

} // namespace BVP

#endif // TPARAM_H

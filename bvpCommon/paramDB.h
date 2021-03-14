#ifndef TPARAM_DB_H
#define TPARAM_DB_H

#include "param.h"

namespace BVP {

bool setControl(param_t param, src_t src, uint32_t &value);
bool setError(param_t param, src_t src, uint32_t &value);
bool setWarning(param_t param, src_t src, uint32_t &value);

//
TParam::paramFields_t TParam::params[PARAM_MAX] = {
    //
    {.param = PARAM_control, .source = (1 << SRC_int) | (1 << SRC_pi),
     .isSet = false, .isModified = false,
     .rValue = 0, .wValue = 0,
     .set = nullptr, .get = nullptr},
    //
    // ������� ���������
    //
    {.param =  PARAM_error,  .source = (1 << SRC_int),
     .isSet = false, .isModified = false,
     .rValue = 0, .wValue = 0,
     .set = nullptr, .get = nullptr},
    {.param = PARAM_warning, .source = (1 << SRC_int),
     .isSet = false, .isModified = false,
     .rValue = 0, .wValue = 0,
     .set = nullptr, .get = nullptr},
    {.param = PARAM_defError, .source = (1 << SRC_pi),
     .isSet = false, .isModified = false,
     .rValue = 0, .wValue = 0,
     .set = setError, .get = nullptr},
    {.param = PARAM_defWarning, .source = (1 << SRC_pi),
     .isSet = false, .isModified = false,
     .rValue = 0, .wValue = 0,
     .set = setWarning, .get = nullptr},
    {.param = PARAM_prmError, .source = (1 << SRC_pi),
     .isSet = false, .isModified = false,
     .rValue = 0, .wValue = 0,
     .set = setError, .get = nullptr},
    {.param = PARAM_prmWarning, .source = (1 << SRC_pi),
     .isSet = false, .isModified = false,
     .rValue = 0, .wValue = 0,
     .set = setWarning, .get = nullptr},
    {.param = PARAM_prm2Error, .source = (1 << SRC_pi),
     .isSet = false, .isModified = false,
     .rValue = 0, .wValue = 0,
     .set = setError, .get = nullptr},
    {.param = PARAM_prm2Warning, .source = (1 << SRC_pi),
     .isSet = false, .isModified = false,
     .rValue = 0, .wValue = 0,
     .set = setWarning, .get = nullptr},
    {.param = PARAM_prdError, .source = (1 << SRC_pi),
     .isSet = false, .isModified = false,
     .rValue = 0, .wValue = 0,
     .set = setError, .get = nullptr},
    {.param = PARAM_prdWarning, .source = (1 << SRC_pi),
     .isSet = false, .isModified = false,
     .rValue = 0, .wValue = 0,
     .set = setWarning, .get = nullptr},
    {.param = PARAM_glbError, .source = (1 << SRC_pi),
     .isSet = false, .isModified = false,
     .rValue = 0, .wValue = 0,
     .set = setError, .get = nullptr},
    {.param = PARAM_glbWarning, .source = (1 << SRC_pi),
     .isSet = false, .isModified = false,
     .rValue = 0, .wValue = 0,
     .set = setWarning, .get = nullptr},
    {.param = PARAM_defRemoteError, .source = (1 << SRC_pi),
     .isSet = false, .isModified = false,
     .rValue = 0, .wValue = 0,
     .set = nullptr, .get = nullptr},
    {.param = PARAM_prmRemoteError, .source = (1 << SRC_pi),
     .isSet = false, .isModified = false,
     .rValue = 0, .wValue = 0,
     .set = nullptr, .get = nullptr},
    {.param = PARAM_prdRemoteError, .source = (1 << SRC_pi),
     .isSet = false, .isModified = false,
     .rValue = 0, .wValue = 0,
     .set = nullptr, .get = nullptr},
    {.param = PARAM_glbRemoteError, .source = (1 << SRC_pi),
     .isSet = false, .isModified = false,
     .rValue = 0, .wValue = 0,
     .set = nullptr, .get = nullptr},
    //
    // ���� � �����
    //
    {.param = PARAM_dateYear, .source = (1 << SRC_pi),
     .isSet = false, .isModified = false,
     .rValue = 0, .wValue = 0,
     .set = nullptr, .get = nullptr},
    {.param = PARAM_dateMonth, .source = (1 << SRC_pi),
     .isSet = false, .isModified = false,
     .rValue = 0, .wValue = 0,
     .set = nullptr, .get = nullptr},
    {.param = PARAM_dateDay, .source = (1 << SRC_pi),
     .isSet = false, .isModified = false,
     .rValue = 0, .wValue = 0,
     .set = nullptr, .get = nullptr},
    {.param = PARAM_timeHour, .source = (1 << SRC_pi),
     .isSet = false, .isModified = false,
     .rValue = 0, .wValue = 0,
     .set = nullptr, .get = nullptr},
    {.param = PARAM_timeMin, .source = (1 << SRC_pi),
     .isSet = false, .isModified = false,
     .rValue = 0, .wValue = 0,
     .set = nullptr, .get = nullptr},
    {.param = PARAM_timeSec, .source = (1 << SRC_pi),
     .isSet = false, .isModified = false,
     .rValue = 0, .wValue = 0,
     .set = nullptr, .get = nullptr},
    {.param = PARAM_timeMSec, .source = (1 << SRC_pi),
     .isSet = false, .isModified = false,
     .rValue = 0, .wValue = 0,
     .set = nullptr, .get = nullptr},
    //
    // ����� ���������
    //
    {.param = PARAM_alarmReset, .source = (1 << SRC_pi),
     .isSet = false, .isModified = false,
     .rValue = 0, .wValue = 0,
     .set = nullptr, .get = nullptr},
    //
    // ���������� ���������
    //
    {.param = PARAM_extAlarm, .source = (1 << SRC_int),
     .isSet = false, .isModified = false,
     .rValue = 0, .wValue = 0,
     .set = nullptr, .get = nullptr},
    {.param = PARAM_alarmResetBtn, .source = (1 << SRC_int) | (1 << SRC_vkey),
     .isSet = false, .isModified = false,
     .rValue = 0, .wValue = 0,
     .set = nullptr, .get = nullptr},
    //
    // ��������� ������ ����������� ������
    //
    {.param = PARAM_dirControl, .source = (1 << SRC_pi),
     .isSet = false, .isModified = false,
     .rValue = 0, .wValue = 0,
     .set = nullptr, .get = nullptr},
    //
    {.param = PARAM_blkComPrmAll, .source = (1 << SRC_pi),
     .isSet = false, .isModified = false,
     .rValue = 0, .wValue = 0,
     .set = nullptr, .get = nullptr},
    //
    {.param = PARAM_blkComPrmDir, .source = (1 << SRC_pi),
     .isSet = false, .isModified = false,
     .rValue = 0, .wValue = 0,
     .set = nullptr, .get = nullptr},
    //
    {.param = PARAM_comPrmBlk08to01, .source = (1 << SRC_pi),
     .isSet = false, .isModified = false,
     .rValue = 0, .wValue = 0,
     .set = nullptr, .get = nullptr},
    //
    {.param = PARAM_comPrmBlk16to09, .source = (1 << SRC_pi),
     .isSet = false, .isModified = false,
     .rValue = 0, .wValue = 0,
     .set = nullptr, .get = nullptr},
    //
    {.param = PARAM_comPrmBlk24to17, .source = (1 << SRC_pi),
     .isSet = false, .isModified = false,
     .rValue = 0, .wValue = 0,
     .set = nullptr, .get = nullptr},
    //
    {.param = PARAM_comPrmBlk32to25, .source = (1 << SRC_pi),
     .isSet = false, .isModified = false,
     .rValue = 0, .wValue = 0,
     .set = nullptr, .get = nullptr},
    //
    {.param = PARAM_blkComPrm64to33, .source = (1 << SRC_pi),
     .isSet = false, .isModified = false,
     .rValue = 0, .wValue = 0,
     .set = nullptr, .get = nullptr},
    //
    {.param = PARAM_comPrdBlk08to01, .source = (1 << SRC_pi),
     .isSet = false, .isModified = false,
     .rValue = 0, .wValue = 0,
     .set = nullptr, .get = nullptr},
    //
    {.param = PARAM_comPrdBlk16to09, .source = (1 << SRC_pi),
     .isSet = false, .isModified = false,
     .rValue = 0, .wValue = 0,
     .set = nullptr, .get = nullptr},
    //
    {.param = PARAM_comPrdBlk24to17, .source = (1 << SRC_pi),
     .isSet = false, .isModified = false,
     .rValue = 0, .wValue = 0,
     .set = nullptr, .get = nullptr},
    //
    {.param = PARAM_comPrdBlk32to25, .source = (1 << SRC_pi),
     .isSet = false, .isModified = false,
     .rValue = 0, .wValue = 0,
     .set = nullptr, .get = nullptr},
    //
    {.param = PARAM_blkComPrd64to33, .source = (1 << SRC_pi),
     .isSet = false, .isModified = false,
     .rValue = 0, .wValue = 0,
     .set = nullptr, .get = nullptr},
    //
    // ������ ������ ����������� ������.
    //
    {.param = PARAM_vpBtnSAnSbSac, .source = (1 << SRC_vkey) | (1 << SRC_int),
     .isSet = false, .isModified = false,
     .rValue = 0, .wValue = 0,
     .set = nullptr, .get = nullptr},
    //
    {.param = PARAM_vpBtnSA32to01, .source = (1 << SRC_vkey),
     .isSet = false, .isModified = false,
     .rValue = 0, .wValue = 0,
     .set = nullptr, .get = nullptr},
    //
    {.param = PARAM_vpBtnSA64to33, .source = (1 << SRC_vkey),
     .isSet = false, .isModified = false,
     .rValue = 0, .wValue = 0,
     .set = nullptr, .get = nullptr},
};

} // namespace BVP

#endif // TPARAM_DB_H

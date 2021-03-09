#ifndef TPARAM_DB_H
#define TPARAM_DB_H

#include "param.h"

namespace BVP {

bool getBlkComPrm(param_t param, src_t src, uint32_t &value);
bool getControl(param_t param, src_t src, uint32_t &value);

bool setBlkComPrmAll(param_t param, src_t src, uint32_t &value);
bool setBlkComPrm(param_t param, src_t src, uint32_t &value);
bool setBtnSA(param_t param, src_t src, uint32_t &value);
bool setControl(param_t param, src_t src, uint32_t &value);
bool setError(param_t param, src_t src, uint32_t &value);
bool setDirControl(param_t param, src_t src, uint32_t &value);
bool setVpBtnSAnSbSac(param_t param, src_t src, uint32_t &value);
bool setWarning(param_t param, src_t src, uint32_t &value);

//
TParam::paramFields_t TParam::params[PARAM_MAX] = {
    //
    {.param = PARAM_control,
     .isValue = false, .rValue = 0, .wValue = 0,
     .set = setControl, .get = getControl},
    //
    // Текущее состояние
    //
    {.param =  PARAM_error,
     .isValue =  false, .rValue = 0, .wValue = 0,
     .set = nullptr, .get = nullptr},
    {.param = PARAM_warning,
     .isValue = false, .rValue = 0, .wValue = 0,
     .set = nullptr, .get = nullptr},
    {.param = PARAM_defError,
     .isValue = false, .rValue = 0, .wValue = 0,
     .set = setError, .get = nullptr},
    {.param = PARAM_defWarning,
     .isValue = false, .rValue = 0, .wValue = 0,
     .set = setWarning, .get = nullptr},
    {.param = PARAM_prmError,
     .isValue = false, .rValue = 0, .wValue = 0,
     .set = setError, .get = nullptr},
    {.param = PARAM_prmWarning,
     .isValue = false, .rValue = 0, .wValue = 0,
     .set = setWarning, .get = nullptr},
    {.param = PARAM_prm2Error,
     .isValue = false, .rValue = 0, .wValue = 0,
     .set = setError, .get = nullptr},
    {.param = PARAM_prm2Warning,
     .isValue = false, .rValue = 0, .wValue = 0,
     .set = setWarning, .get = nullptr},
    {.param = PARAM_prdError,
     .isValue = false, .rValue = 0, .wValue = 0,
     .set = setError, .get = nullptr},
    {.param = PARAM_prdWarning,
     .isValue = false, .rValue = 0, .wValue = 0,
     .set = setWarning, .get = nullptr},
    {.param = PARAM_glbError,
     .isValue = false, .rValue = 0, .wValue = 0,
     .set = setError, .get = nullptr},
    {.param = PARAM_glbWarning,
     .isValue = false, .rValue = 0, .wValue = 0,
     .set = setWarning, .get = nullptr},
    {.param = PARAM_defRemoteError,
     .isValue = false, .rValue = 0, .wValue = 0,
     .set = nullptr, .get = nullptr},
    {.param = PARAM_prmRemoteError,
     .isValue = false, .rValue = 0, .wValue = 0,
     .set = nullptr, .get = nullptr},
    {.param = PARAM_prdRemoteError,
     .isValue = false, .rValue = 0, .wValue = 0,
     .set = nullptr, .get = nullptr},
    {.param = PARAM_glbRemoteError,
     .isValue = false, .rValue = 0, .wValue = 0,
     .set = nullptr, .get = nullptr},
    //
    // Дата и время
    //
    {.param = PARAM_dateYear,
     .isValue = false, .rValue = 0, .wValue = 0,
     .set = nullptr, .get = nullptr},
    {.param = PARAM_dateMonth,
     .isValue = false, .rValue = 0, .wValue = 0,
     .set = nullptr, .get = nullptr},
    {.param = PARAM_dateDay,
     .isValue = false, .rValue = 0, .wValue = 0,
     .set = nullptr, .get = nullptr},
    {.param = PARAM_timeHour,
     .isValue = false, .rValue = 0, .wValue = 0,
     .set = nullptr, .get = nullptr},
    {.param = PARAM_timeMin,
     .isValue = false, .rValue = 0, .wValue = 0,
     .set = nullptr, .get = nullptr},
    {.param = PARAM_timeSec,
     .isValue = false, .rValue = 0, .wValue = 0,
     .set = nullptr, .get = nullptr},
    {.param = PARAM_timeMSec,
     .isValue = false, .rValue = 0, .wValue = 0,
     .set = nullptr, .get = nullptr},
    //
    // Общие параметры
    //
    {.param = PARAM_alarmReset,
     .isValue = false, .rValue = 0, .wValue = 0,
     .set = nullptr, .get = nullptr},
    //
    // Внутренние параметры
    //
    {.param = PARAM_extAlarm,
     .isValue = false, .rValue = 0, .wValue = 0,
     .set = nullptr, .get = nullptr},
    //
    // Параметры панели виртуальных ключей
    //
    {.param = PARAM_dirControl,
     .isValue = false, .rValue = 0, .wValue = 0,
     .set = setDirControl, .get = nullptr},
    //
    {.param = PARAM_blkComPrmAll,
     .isValue = false, .rValue = 0, .wValue = 0,
     .set = setBlkComPrmAll, .get = nullptr},
    //
    {.param = PARAM_blkComPrmDir,
     .isValue = false, .rValue = 0, .wValue = 0,
     .set = nullptr, .get = nullptr},
    //
    {.param = PARAM_blkComPrm32to01,
     .isValue = false, .rValue = 0, .wValue = 0,
     .set = setBlkComPrm, .get = getBlkComPrm},
    //
    {.param = PARAM_blkComPrm64to33,
     .isValue = false, .rValue = 0, .wValue = 0,
     .set = setBlkComPrm, .get = getBlkComPrm},
    //
    {.param = PARAM_blkComPrd32to01,
     .isValue = false, .rValue = 0, .wValue = 0,
     .set = nullptr, .get = nullptr},
    //
    {.param = PARAM_blkComPrd64to33,
     .isValue = false, .rValue = 0, .wValue = 0,
     .set = nullptr, .get = nullptr},
    //
    // Кнопки панели виртуальных ключей.
    //
    {.param = PARAM_vpBtnSAnSbSac,
     .isValue = false, .rValue = 0, .wValue = 0,
     .set = setVpBtnSAnSbSac, .get = nullptr},
    //
    {.param = PARAM_vpBtnSA32to01,
     .isValue = false, .rValue = 0, .wValue = 0,
     .set = setBtnSA, .get = nullptr},
    //
    {.param = PARAM_vpBtnSA64to33,
     .isValue = false, .rValue = 0, .wValue = 0,
     .set = setBtnSA, .get = nullptr},
};

} // namespace BVP

#endif // TPARAM_DB_H

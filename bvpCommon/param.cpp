#include "global.hpp"
#include "param.h"
#include "paramDB.h"

namespace BVP {

TParam TParam::mParam;

//
bool getBlkComPrm(param_t param, src_t src, uint32_t &value)
{
    bool ok = false;
    TParam *params = TParam::getInstance();

    UNUSED(param);

    if (src == SRC_vkey) {
        uint32_t blkall = params->getValue(PARAM_blkComPrmAll, src, ok);

        if (ok && (blkall != ON_OFF_off)) {
            value = 0xFFFFFFFF;
        }
    }

    return ok;
}

/** Считывание сигналов управления.
 *
 *  - Для SRC_pi возвращается самый приоритетный на данный момент сигнал
 *  управления ctrl_t. После считывания данный сигнал будет сброшен.
 *  - Для SRC_vkey возвращается текущее состояние сигналов управления.
 *
 *  @param[in] params Параметры.
 *  @param[in] src Источник доступа.
 *  @param[in/out] value Значение.
 *  @return true если значение можно считать, иначе false.
 */
bool getControl(param_t param, src_t src, uint32_t &value)
{
    bool ok = false;
    TParam *params = TParam::getInstance();

    Q_ASSERT(param == PARAM_control);

    switch(src) {
        case SRC_pi: {
            if (value > 0) {
                uint8_t i = 0;
                while((value & (1 << i)) == 0) {
                    i++;
                }
                params->setLocalValue(param, value & ~(1 << i));
                value = i + CTRL_MIN;
            }
            ok = true;
        } break;

        case SRC_vkey: {
            ok = true;
        } break;
        case SRC_pc: break;
        case SRC_acs: break;
        case SRC_MAX: break;
    }

    return ok;
}

//
bool setBlkComPrmAll(param_t param, src_t src, uint32_t &value)
{
    bool ok = false;
    TParam *params = TParam::getInstance();

    UNUSED(param);
    Q_ASSERT(param == PARAM_blkComPrmAll);

    switch(src) {
        case SRC_pi: {
            ok = true;
        } break;

        case SRC_int: {
            ok = true;
        } break;

        case SRC_vkey: {
            if (value > 0) {
                uint32_t dir = params->getValue(PARAM_dirControl, src, ok);
                ok = ok && (dir == DIR_CONTROL_local);
                if (ok) {
                    uint32_t v = params->getValue(PARAM_blkComPrmAll, src, ok);
                    if (ok) {
                        value = (v == ON_OFF_off) ? ON_OFF_on : ON_OFF_off;
                    }
                }
            }
        } break;

        case SRC_pc: break;
        case SRC_acs: break;
        case SRC_MAX: break;
    }

    return ok;
}

/** Установка блокированных команд приемника.
 *
 *  - Запись с SRC_pi идет без изменений.
 *  - Запись с SRC_vkey идет переключением текущих состояний блокировки для
 *  команд по маске, т.е. тех где бит равен 1. При этом состояние меняется
 *  только в случае локального управления и отсутствии общей блокировки команд
 *  приемника.
 *
 *  @param[in] param Параметр.
 *  @param[in] src Источник доступа.
 *  @param[in/out] value Значение.
 *  @return true если значение надо записать, иначе false.
 */
bool setBlkComPrm(param_t param, src_t src, uint32_t &value)
{
    bool ok = false;
    TParam *params = TParam::getInstance();

    Q_ASSERT((param == PARAM_blkComPrm32to01) || (param == PARAM_blkComPrm64to33));

    switch(src) {
        case SRC_pi: {
            ok = true;
        } break;

        case SRC_vkey: {
            if (value > 0) {
                uint32_t dir = params->getValue(PARAM_dirControl, src, ok);
                ok = ok && (dir == DIR_CONTROL_local);

                if (ok) {
                    uint32_t blkall = params->getValue(PARAM_blkComPrmAll, src, ok);
                    ok = ok && (blkall == ON_OFF_off);
                }

                if (ok) {
                    uint32_t v = params->getValue(param, src, ok);
                    if (ok) {
                        value = v ^ value;
                    }
                }
            }
        } break;

        case SRC_pc: break;
        case SRC_acs: break;
        case SRC_MAX: break;
    }

    return ok;
}

//
bool setBtnSA(param_t param, src_t src, uint32_t &value)
{
    bool ok = false;
    TParam *params = TParam::getInstance();

    Q_ASSERT((param == PARAM_vpBtnSA32to01) || (param == PARAM_vpBtnSA64to33));

    if (src == SRC_vkey) {
        uint32_t tvalue = (params->getValue(param, src, ok) ^ value) & value;

        if (tvalue > 0) {
            if (param == PARAM_vpBtnSA32to01) {
                param = PARAM_blkComPrm32to01;
            } else if (param == PARAM_blkComPrm64to33) {
                param = PARAM_blkComPrm64to33;
            } else {
                param = PARAM_MAX;
            }

            if (param != PARAM_MAX) {
                params->setValue(param, src, tvalue);
            }
        }
        ok = true;
    }

    return ok;
}

/** Установка сигналов управления.
 *
 *  - Запись с SRC_pi означает сброс бит по маске. Если это первая установка
 *  данного параметра, в него будет записан 0.
 *  - Запись с SRC_vkey значения не 0 означает нажатие на кнопку сброса индикации,
 *  т.е. сброс индикации команд, неисправностей, предупреждений и их реле.
 *  Обработка нажатия производится только в случае локального управления. Этого
 *  достаточно для форимрования сигнала сброса индикации команд. Сигнал сброса
 *  индикации неисправностей/предупреждений формируется толко при их наличии.
 *
 *  @param[in] param Параметр.
 *  @param[in] src Источник доступа.
 *  @param[in/out] value Значение.
 *  @return true если значение надо записать, иначе false.
 */
bool setControl(param_t param, src_t src, uint32_t &value)
{
    bool ok = false;
    TParam *params = TParam::getInstance();

    switch(src) {
        case SRC_pi: {
            if (params->isValueSet(param)) {
                uint32_t v = params->getValue(param, src, ok);

                Q_ASSERT(ok);
                if (ok) {
                    value = v & ~value;
                }
            } else {
                value = 0;
                ok = true;
            }
        } break;

        case SRC_vkey: {
            if (value > 0) {
                uint32_t dir = params->getValue(PARAM_dirControl, src, ok);
                ok = ok && (dir == DIR_CONTROL_local);

                if (ok) {
                    uint32_t v = params->getValue(param, src, ok);

                    Q_ASSERT(ok);
                    if (ok) {
                        // сброс индикации
                        value = v | (1 << (CTRL_resetIndication - CTRL_MIN));

                        bool tok = false;

                        // проверка наличия неисправностей для их сброса
                        v = params->getValue(PARAM_error, src, tok);
                        if (tok && (v > 0)) {
                            value = value | (1 << (CTRL_resetErrors - CTRL_MIN));
                        }

                        // проверка наличия предупреждений для их сброса
                        v = params->getValue(PARAM_warning, src, tok);
                        if (tok && (v > 0)) {
                            value = value | (1 << (CTRL_resetErrors - CTRL_MIN));
                        }
                    }
                }
            }
        } break;

        case SRC_pc: break;
        case SRC_acs: break;
        case SRC_MAX: break;
    }

    return ok;
}

//
bool setError(param_t param, src_t src, uint32_t &value)
{
    bool ok = true;
    TParam *params = TParam::getInstance();
    uint32_t v = value;

    // TODO Добавить фильтр по наличию текущих устройств (прм, прд, защ)

    if (ok && (param != PARAM_glbError)) {
        v |= params->getValue(PARAM_glbError, src, ok);
    }

    if (ok && (param != PARAM_defError)) {
        v |= params->getValue(PARAM_defError, src, ok);
    }

    if (ok && (param != PARAM_prmError)) {
        v |= params->getValue(PARAM_prmError, src, ok);
    }

    if (ok && (param != PARAM_prdError)) {
        v |= params->getValue(PARAM_prdError, src, ok);
    }

    if (ok) {
        params->setLocalValue(PARAM_error, v);
    }

    return true;
}

//
bool setDirControl(param_t param, src_t src, uint32_t &value)
{
    bool ok = false;
    TParam *params = TParam::getInstance();

    switch(src) {
        case SRC_pi: {
            ok = true;
        } break;

        case SRC_vkey: {
            if (value > 0) {
                uint32_t d = params->getValue(param, src, ok);
                if (ok) {
                    dirControl_t dir = DIR_CONTROL_local;
                    switch(static_cast<dirControl_t> (d)) {
                        case DIR_CONTROL_remote: {
                            dir = DIR_CONTROL_local;
                        } break;
                        case DIR_CONTROL_local: {
                            dir = DIR_CONTROL_remote;
                        } break;
                        case DIR_CONTROL_MAX: break;
                    }
                    value = dir;
                }
            }
        } break;

        case SRC_pc: break;
        case SRC_acs: break;
        case SRC_MAX: break;
    }

    return ok;
}

//
bool setVpBtnSAnSbSac(param_t param, src_t src, uint32_t &value)
{
    bool ok = false;
    TParam *params = TParam::getInstance();

    UNUSED(param);

    if (src == SRC_vkey) {
        param_t param = PARAM_vpBtnSAnSbSac;
        if (params->isValueSet(param)) {
            uint32_t tvalue = value ^ params->getValue(param, src, ok);

            if (tvalue > 0) {
                if (tvalue & TParam::VP_BTN_CONTROL_sac1) {
                    params->setValue(PARAM_blkComPrmAll, src,
                                     value & TParam::VP_BTN_CONTROL_sac1);
                }

                if (tvalue & TParam::VP_BTN_CONTROL_sac2) {
                    params->setValue(PARAM_dirControl, src,
                                     value & TParam::VP_BTN_CONTROL_sac2);
                }

                if (tvalue & TParam::VP_BTN_CONTROL_sb) {
                    params->setValue(PARAM_control, src,
                                     value & TParam::VP_BTN_CONTROL_sb);
                }

                if (tvalue & TParam::VP_BTN_CONTROL_san) {
                    static_assert(TParam::VP_BTN_CONTROL_san == 0x0000FF00,
                            "Wrong position buttons SAnn.x");
                    params->setValue(PARAM_blkComPrmDir, src,
                                     (value & TParam::VP_BTN_CONTROL_san) >> 8);
                }
            }
        }
        ok = true;
    }

    return ok;
}

//
bool setWarning(param_t param, src_t src, uint32_t &value)
{
    bool ok = true;
    TParam *params = TParam::getInstance();
    uint32_t v = value;

    // TODO Добавить фильтр по наличию текущих устройств (прм, прд, защ)

    if (ok && (param != PARAM_glbWarning)) {
        v |= params->getValue(PARAM_glbWarning, src, ok);
    }

    if (ok && (param != PARAM_defWarning)) {
        v|= params->getValue(PARAM_defWarning, src, ok);
    }

    if (ok && (param != PARAM_prmWarning)) {
        v |= params->getValue(PARAM_prmWarning, src, ok);
    }

    if (ok && (param != PARAM_prdWarning)) {
        v |= params->getValue(PARAM_prdWarning, src, ok);
    }

    if (ok) {
        params->setLocalValue(PARAM_warning, v);
    }

    return true;
}



//
TParam::TParam()
{
    for(uint16_t i = 0; i < PARAM_MAX; i++) {
        //    qDebug() << "params[" << i << "].param = " << params[i].param;
        Q_ASSERT(params[i].param == static_cast<param_t> (i));
    }

    // TODO Подумать над инициализацией параметров которые не надо считывать.
    setLocalValue(PARAM_control, 0);
}

//
TParam* TParam::getInstance()
{
    return &TParam::mParam;
}

//
bool TParam::isValueSet(param_t param) const
{
    Q_ASSERT(param < PARAM_MAX);

    return (param < PARAM_MAX) ? params[param].isValue : false;
}

//
bool TParam::isAccessRead(param_t param, src_t src) const
{
    // TODO Сделать проверку доступа на чтение.
    return true;
}

//
bool TParam::isAccessSet(param_t param, src_t src) const
{
    // TODO Сделать првоерку доступа на запись.
    return true;
}

//
uint32_t TParam::getValue(param_t param, src_t src, bool &ok)
{
    uint32_t value = 0;

    Q_ASSERT(param < PARAM_MAX);

    // TODO добавить проверку источника доступа!
    ok = isAccessRead(param, src) && isValueSet(param);

    if (ok) {
        value = params[param].rValue;

        if (params[param].get != nullptr) {
            ok = params[param].get(param, src, value);
        }
    }

    return value;
}

//
uint32_t TParam::getValueR(param_t param)
{
    Q_ASSERT(param < PARAM_MAX);

    return params[param].rValue;
}

//
uint32_t TParam::getValueW(param_t param)
{
    Q_ASSERT(param < PARAM_MAX);

    return params[param].wValue;
}

//
bool TParam::setValue(param_t param, src_t src, uint32_t value)
{
    bool ok = false;

    Q_ASSERT(param < PARAM_MAX);
    if (param < PARAM_MAX) {
        if (isAccessSet(param, src)) {
            ok = true;

            if (params[param].set != nullptr) {
                ok = params[param].set(param, src, value);
            }

            if (ok) {
                setLocalValue(param, value);
            }
        }

    }

    return ok;
}

//
void TParam::setLocalValue(param_t param, uint32_t value)
{
    params[param].wValue = value;

    params[param].rValue = value; // FIXME rValue должно идти из БСП!
    params[param].isValue = true;
}

} // namespace BVP

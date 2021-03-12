#include "global.hpp"
#include "param.h"
#include "paramDB.h"

namespace BVP {

TParam TParam::mParam;

/** ”становка блокированных команд приемника.
 *
 *  - «апись с SRC_pi идет без изменений.
 *  - «апись с SRC_vkey идет переключением текущих состо€ний блокировки дл€
 *  команд по маске, т.е. тех где бит равен 1. ѕри этом состо€ние мен€етс€
 *  только в случае локального управлени€ и отсутствии общей блокировки команд
 *  приемника.
 *
 *  @param[in] param ѕараметр.
 *  @param[in] src »сточник доступа.
 *  @param[in/out] value «начение.
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

//
bool setError(param_t param, src_t src, uint32_t &value)
{
    bool ok = true;
    TParam *params = TParam::getInstance();
    uint32_t v = value;

    // TODO ƒобавить фильтр по наличию текущих устройств (прм, прд, защ)

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
        params->setValue(PARAM_error, SRC_int, v);
    }

    return true;
}

//
bool setVpBtnSAnSbSac(param_t param, src_t src, uint32_t &value)
{
    bool ok = false;
    TParam *params = TParam::getInstance();

    UNUSED(param);

    if (src == SRC_vkey) {
        param_t param = PARAM_vpBtnSAnSbSac;
        if (params->isSet(param)) {
            uint32_t tvalue = value ^ params->getValue(param, src, ok);

            if (tvalue > 0) {
                if (tvalue & VP_BTN_CONTROL_san) {
                    static_assert(VP_BTN_CONTROL_san == 0x0000FF00,
                            "Wrong position buttons SAnn.x");
                    params->setValue(PARAM_blkComPrmDir, src,
                                     (value & VP_BTN_CONTROL_san) >> 8);
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

    // TODO ƒобавить фильтр по наличию текущих устройств (прм, прд, защ)

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
        params->setValue(PARAM_warning, SRC_int, v);
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

    // TODO ѕодумать над инициализацией параметров которые не надо считывать.
    setLocalValue(PARAM_control, SRC_int, 0);
}

//
TParam* TParam::getInstance()
{
    return &TParam::mParam;
}

//
bool TParam::isSet(param_t param) const
{
    Q_ASSERT(param < PARAM_MAX);

    return (param < PARAM_MAX) ? params[param].isSet : false;
}

//
bool TParam::isModified(param_t param) const
{
    Q_ASSERT(param < PARAM_MAX);

    // TODO надо где-то сделать проверку, а изменилось ли значение?!

    return params[param].isSet && params[param].isModified;
}

//
bool TParam::isAccess(param_t param, src_t src) const
{
    // TODO —делать првоерку доступа
    return true;
}

//
uint32_t TParam::getValue(param_t param, src_t src, bool &ok)
{
    uint32_t value = 0;

    Q_ASSERT(param < PARAM_MAX);

    // TODO добавить проверку источника доступа!
    ok = isAccess(param, src) && isSet(param);

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
        if (isAccess(param, src)) {
            ok = true;

            if (params[param].set != nullptr) {
                ok = params[param].set(param, src, value);
            }

            if (ok) {
                setLocalValue(param, src, value);
            }
        }

    }

    return ok;
}



//
void TParam::setLocalValue(param_t param, src_t src, uint32_t value)
{
    paramFields_t *p = &params[param];

    if (p->source & (1 << src)) {
        p->rValue = value;
        p->isSet = true;

        if (!p->isModified) {
            p->wValue = value;
        } else {
            p->isModified = (value != p->wValue);
        }
    } else {
        params[param].wValue = value;
        params[param].isModified = true;
        if (param == PARAM_blkComPrmAll) {
            qDebug() << "New Prm disable value set to" << value;
        }
    }
}

} // namespace BVP

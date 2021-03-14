#include "global.hpp"
#include "param.h"
#include "paramDB.h"

namespace BVP {

TParam TParam::mParam;

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
        params->setValue(PARAM_error, SRC_int, v);
    }

    return true;
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
        v |= params->getValue(PARAM_defWarning, src, ok);
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

    // TODO Подумать над инициализацией параметров которые не надо считывать.
    setLocalValue(PARAM_control, SRC_int, 0);
    setLocalValue(PARAM_extAlarm, SRC_int, 0);
    setLocalValue(PARAM_alarmResetBtn, SRC_int, 0);
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
    // TODO Сделать првоерку доступа
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
uint32_t TParam::getValueR(param_t param) const
{
    Q_ASSERT(param < PARAM_MAX);

    return params[param].rValue;
}

//
uint32_t TParam::getValueW(param_t param) const
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
    }
}

} // namespace BVP

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

    // TODO �������� ������ �� ������� ������� ��������� (���, ���, ���)

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

    // TODO �������� ������ �� ������� ������� ��������� (���, ���, ���)

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
        Q_ASSERT_X(params[i].param == static_cast<param_t> (i),
                   "param", QString::number(i).toStdString().c_str());
    }

    reset();
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

    // TODO ���� ���-�� ������� ��������, � ���������� �� ��������?!

    return params[param].isSet && params[param].isModified;
}

//
bool TParam::isAccess(param_t param, src_t src) const
{
    // TODO ������� �������� �������
    return true;
}

//
uint32_t TParam::getValue(param_t param, src_t src, bool &ok)
{
    uint32_t value = 0;

    Q_ASSERT(param < PARAM_MAX);

    // TODO �������� �������� ��������� �������!
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
void TParam::reset()
{
    paramFields_t *p;

    for(uint16_t i = 0; i < PARAM_MAX; i++) {
        p = &params[i];

        p->rValue = 0;
        p->isSet = false;
        p->wValue = 0;
        p->isModified = false;
    }

    // TODO �������� ��� �������������� ���������� ������� �� ���� ���������.
    setLocalValue(PARAM_control, mSrc, 0);
    setLocalValue(PARAM_extAlarm, mSrc, 0);
    setLocalValue(PARAM_alarmRstCtrl, mSrc, 0);

    // FIXME ��������� ��������� ������ ���� � �� ��� ���!!!
    uint32_t version = (static_cast<uint32_t>(versionMajor) << 8) + versionMinor;
    setLocalValue(PARAM_version, mSrc, version);
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
        p->wValue = value;
        p->isModified = true;
    }
}

} // namespace BVP

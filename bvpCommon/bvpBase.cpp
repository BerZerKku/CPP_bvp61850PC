/*
 * bvpBase.cpp
 *
 *  Created on: March 16, 2021
 *      Author: bear
 */

#include "bvpBase.hpp"
#include "clock.hpp"

#if defined(QT_CORE_LIB) && !defined(MY_TESTS)
#include "BvpApp/mainwindow.h"
#endif

extern bool getExtAlarmSignals(BVP::extAlarm_t signal);
extern void setExtAlarmSignal(BVP::extAlarm_t signal, bool value);

namespace BVP {

//
BVP::TBvpBase::TBvpBase() :
    mParam(TParam::getInstance()),
    mSerialVp(TModbusVp::REGIME_master),
    mSerialPc(TAvantPc::REGIME_slave),
    mSerialPi(TAvantPi::REGIME_master)
{
    for(uint8_t i = 0; i < sizeof(mSerial) / sizeof(mSerial[0]); i++) {
        mSerial[i] = nullptr;
    }

    mSerialVp.setNetAddress(kModbusVpAddress);
    mSerialVp.setup(9600, true, 1);
    mSerial[SRC_vkey] = &mSerialVp;

    mSerialPc.setup(19200, true, 2);
    mSerial[SRC_pc] = &mSerialPc;

    mSerialPi.setup(19200, true, 2);
    mSerial[SRC_pi] = &mSerialPi;

    for(uint8_t i = 0; i < sizeof(mSerial) / sizeof(mSerial[0]); i++) {
        TSerialProtocol *p = mSerial[i];

        if (p != nullptr) {
            p->setID(src_t(i));
            p->setBuffer(mSerialBuf[i], kSerialBufferSize);
        }
    }
}

//
void TBvpBase::init()
{
    for(uint8_t i = 0; i < sizeof(mSerial) / sizeof(mSerial[0]); i++) {
        TSerialProtocol *p = mSerial[i];

        if (p != nullptr) {
            p->setEnable(true);
        }
    }
}

//
void TBvpBase::loop()
{
    alarmLoop();
    serialLoop();
}

//
void TBvpBase::tick1ms()
{

}

//
uint16_t TBvpBase::protPop(src_t src, uint8_t *data[])
{
    uint16_t len = 0;
    data = nullptr;

    if (src < SRC_MAX) {
        TSerialProtocol *p = mSerial[src];
        if (p != nullptr) {
            len = p->pop(data);
        }
    }

    return len;
}

//
bool TBvpBase::protPush(src_t src, uint8_t byte)
{
    bool state = false;

    if (src < SRC_MAX) {
        TSerialProtocol *p = mSerial[src];
        if (p != nullptr) {
            state = p->push(byte);
        }
    }

    return state;
}

//
void TBvpBase::protSendFinished(src_t src)
{
    if (src < SRC_MAX) {
        TSerialProtocol *p = mSerial[src];
        if (p != nullptr) {
            p->sendFinished();
        }
    }
}

//
void TBvpBase::protTick()
{
    for(uint8_t i = 0; i < sizeof(mSerial) / sizeof(mSerial[0]); i++) {
        TSerialProtocol *p = mSerial[i];
        if (p != nullptr) {
            p->tick();
        }
    }
}

//
void TBvpBase::protSetTick(uint32_t tickus)
{
    for(uint8_t i = 0; i < sizeof(mSerial) / sizeof(mSerial[0]); i++) {
        TSerialProtocol *p = mSerial[i];
        if (p != nullptr) {
            p->setTimeTick(tickus);
        }
    }
}

bool TBvpBase::protIsConnection(src_t src)
{
    bool connection = false;
    if (src < SRC_MAX) {
        TSerialProtocol *p = mSerial[src];
        if (p != nullptr) {
            connection = p->isConnection();
        }
    }
    return connection;
}

//
void TBvpBase::alarmLoop()
{
    const src_t src = src_t::SRC_int;
    bool ok;
    uint32_t uval32;

    alarmResetLoop();

    // Обработка входных и установка выходных сигналов

    uval32 = mParam->getValue(param_t::PARAM_alarmResetMode, src, ok);
    if (ok) {
        mAlarm.setAlarmReset(alarmReset_t(uval32));
    }

    for(uint8_t i = 0; i < extAlarm_t::EXT_ALARM_MAX; i++) {
        bool value;
        extAlarm_t signal = static_cast<extAlarm_t> (i);

        if (signal == extAlarm_t::EXT_ALARM_disablePrm) {
            uval32 = mParam->getValue(param_t::PARAM_blkComPrmAll,
                                      src_t::SRC_int, ok);
            if (!ok) {
                uval32 = disablePrm_t::DISABLE_PRM_enable;
            }

            value = (uval32 == disablePrm_t::DISABLE_PRM_disable);
        } else {
            value = getExtAlarmSignals(signal);
        }

        mAlarm.setAlarmInputSignal(signal, value);
    }

    for(uint8_t i = 0; i < extAlarm_t::EXT_ALARM_MAX; i++) {
        extAlarm_t signal = static_cast<extAlarm_t> (i);

        bool value = mAlarm.getAlarmOutputSignal(signal);
        setExtAlarmSignal(signal, value);
    }
}

//
void TBvpBase::alarmResetLoop()
{
    static clockPoint_t last = TClock::getClockPoint();
    const src_t src = src_t::SRC_int;
    bool ok = true;
    uint32_t uval32;

    // Обработка нажатия кнопки сброса

    uval32 = mParam->getValue(param_t::PARAM_alarmRstCtrl, src, ok);
    if ((ok) && (uval32 != alarmRstCtrl_t::ALARM_RST_CTRL_no)) {
        if (uval32 == alarmRstCtrl_t::ALARM_RST_CTRL_pressed) {
            uval32 = mParam->getValue(param_t::PARAM_control, src, ok);
            if (!ok) {
                uval32 = 0;
            }

            uval32 |= (1 << ctrl_t::CTRL_resetComInd);
            if (mAlarm.isSignalForDeviceReset()) {
                qDebug() << "Reset signal";
                uval32 |= (1 << ctrl_t::CTRL_resetFault);
            }

            mParam->setValue(param_t::PARAM_control, src, uval32);

            // Сброс сигнализации и начало отсчета времени
            mAlarm.reset(true);
            last = TClock::getClockPoint();
        }
        uval32 = alarmRstCtrl_t::ALARM_RST_CTRL_no;
    }
    mParam->setValue(param_t::PARAM_alarmRstCtrl, src, uval32);

    // Отключение сброса сигнализации, если прошло заданное время.
    if (mAlarm.isReset() && (TClock::getDurationS(last) >= 2)) {
        mAlarm.reset(false);
    }
}

void TBvpBase::serialLoop()
{
    for(uint8_t i = 0; i < sizeof(mSerial) / sizeof(mSerial[0]); i++) {
        TSerialProtocol *p = mSerial[i];

        if ((p != nullptr) && p->isEnable()) {
            p->read();

            if (p->write()) {
                uint8_t *data = nullptr;
                uint16_t len = p->pop(&data);

                Q_ASSERT(data != nullptr);

                if (len > 0) {
                    sendDataToSerial(src_t(i), len, data);
                }
            }
        }
    }
}

} // namespace BVP

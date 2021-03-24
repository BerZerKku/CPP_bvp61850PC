#include "gtest/gtest.h"


#include <cstdio>
#include <iostream>

#define TEST_FRIENDS \
    friend class TExtAlarm_Test; \
    FRIEND_TEST(TExtAlarm_Test, config); \
    FRIEND_TEST(TExtAlarm_Test, setBitValue); \
    FRIEND_TEST(TExtAlarm_Test, alarmReset); \
    FRIEND_TEST(TExtAlarm_Test, reset); \
    FRIEND_TEST(TExtAlarm_Test, alarmSignal_default); \
    FRIEND_TEST(TExtAlarm_Test, alarmSignal_manual); \
    FRIEND_TEST(TExtAlarm_Test, alarmSignal_auto);



#include "bvpCommon/extAlarm.hpp"

using namespace std;

namespace BVP {


class TExtAlarm_Test: public ::testing::Test {

public:
    const uint16_t kAlarmOutDefault = 0; //(1 << EXT_ALARM_fault);
    const alarmReset_t kAlarmResetDefault = ALARM_RESET_MAX;

    TExtAlarm *mAlarm;

    // Сброс всех сигналов
    bool resetSignals() {
        bool reset = true;

        for(uint8_t i = 0; i < EXT_ALARM_MAX; i++) {
            mAlarm->setAlarmInputSignal(extAlarm_t(i), false);
        }

        mAlarm->reset(true);
        mAlarm->reset(false);

        for(uint8_t i = 0; i < EXT_ALARM_MAX; i++) {
            if (mAlarm->getAlarmOutputSignal(extAlarm_t(i))  != false) {
                reset = false;
                break;
            }
        }

        return reset;
    }

protected:

    void SetUp() override {
        mAlarm = new BVP::TExtAlarm();
    }

    void TearDown()override {
        delete mAlarm;
    }
};

//
TEST_F(TExtAlarm_Test, constant)
{
    int i = 0;
    ASSERT_EQ(i++, EXT_ALARM_model61850 );
    ASSERT_EQ(i++, EXT_ALARM_test61850);
    ASSERT_EQ(i++, EXT_ALARM_channelFault);
    ASSERT_EQ(i++, EXT_ALARM_warning);
    ASSERT_EQ(i++, EXT_ALARM_fault);
    ASSERT_EQ(i++, EXT_ALARM_comPrd);
    ASSERT_EQ(i++, EXT_ALARM_comPrm);
    ASSERT_EQ(i++, EXT_ALARM_disablePrm);
    ASSERT_EQ(i++, EXT_ALARM_MAX);
}

//
TEST_F(TExtAlarm_Test, config)
{
    for(uint8_t i = 0; i < EXT_ALARM_MAX; i++) {
        const TExtAlarm::signal_t *s = &mAlarm->mSignal[i];

        ASSERT_EQ(extAlarm_t(i), s->signal);

        // проверка режима работы сигнала во время сброса
        TExtAlarm::resetMode_t rmode = (i == EXT_ALARM_fault) ?
                                           TExtAlarm::RESET_MODE_off :
                                           TExtAlarm::RESET_MODE_direct;
        ASSERT_EQ(rmode, s->resetMode) << "signal" << uint(i);

        alarmReset_t areset = (i == EXT_ALARM_comPrd) || (i == EXT_ALARM_comPrm) ?
                                  ALARM_RESET_manual :
                                  ALARM_RESET_MAX;
        ASSERT_EQ(areset, s->alarmReset) << "signal" << uint(i);
    }
}

TEST_F(TExtAlarm_Test, setBitValue) {
    uint16_t value = 0;
    uint16_t result = 0;

    ASSERT_EQ(sizeof(value), sizeof(mAlarm->mAlarmIn));
    ASSERT_EQ(sizeof(value), sizeof(mAlarm->mAlarmOut));
    ASSERT_LE(EXT_ALARM_MAX, sizeof(mAlarm->mAlarmIn)*CHAR_BIT);

    // Установка true
    value = 0;
    result = 0;
    for(uint8_t i = 0; i < (sizeof(value) * CHAR_BIT); i++) {
        uint16_t result = mAlarm->setBitValue(value, i, true);
        ASSERT_EQ(value | (1 << i), result) << "i = " << int(i);
        value = result;
    }

    // Установка false
    for(uint8_t i = 0; i < (sizeof(value) * CHAR_BIT); i++) {
        uint16_t result = mAlarm->setBitValue(value, i, false);
        ASSERT_EQ(value & ~(1 << i), result) << "i = " << int(i);
        value = result;
    }
}

// Проверка установки сброса сигнализации
TEST_F(TExtAlarm_Test, alarmReset)
{
    alarmReset_t value;
    ASSERT_EQ(ALARM_RESET_MAX, mAlarm->mAlarmReset);

    value = ALARM_RESET_auto;
    mAlarm->setAlarmReset(value);
    ASSERT_EQ(value, mAlarm->mAlarmReset);

    value = ALARM_RESET_manual;
    mAlarm->setAlarmReset(value);
    ASSERT_EQ(value, mAlarm->mAlarmReset);

    value = ALARM_RESET_auto;
    mAlarm->setAlarmReset(value);
    ASSERT_EQ(value, mAlarm->mAlarmReset);

    value = ALARM_RESET_MAX;
    mAlarm->setAlarmReset(value);
    ASSERT_EQ(ALARM_RESET_MAX, mAlarm->mAlarmReset);

    value = ALARM_RESET_auto;
    mAlarm->setAlarmReset(value);
    ASSERT_EQ(value, mAlarm->mAlarmReset);

    value = alarmReset_t(0xFF);
    mAlarm->setAlarmReset(value);
    ASSERT_EQ(ALARM_RESET_MAX, mAlarm->mAlarmReset);
}

// Проверка работы сигнализации во время сброса
TEST_F(TExtAlarm_Test, reset)
{
    //
    // Проверка устновки выходных сигналов состояниями из входных
    //

    // Все входные сигналы false
    for(uint8_t i = 0; i < EXT_ALARM_MAX; i++) {
        ASSERT_TRUE(mAlarm->setAlarmInputSignal(extAlarm_t(i), false))
                << "signal " << extAlarm_t(i);
    }

    mAlarm->reset(true);

    for(uint8_t i = 0; i < EXT_ALARM_MAX; i++) {
        ASSERT_FALSE(mAlarm->getAlarmOutputSignal(extAlarm_t(i)))
                << "signal " << extAlarm_t(i);
    }

    // Все входные сигналы true
    for(uint8_t i = 0; i < EXT_ALARM_MAX; i++) {
        ASSERT_TRUE(mAlarm->setAlarmInputSignal(extAlarm_t(i), true))
                << "signal " << extAlarm_t(i);
    }

    mAlarm->reset(true);

    for(uint8_t i = 0; i < EXT_ALARM_MAX; i++) {
        ASSERT_TRUE(mAlarm->getAlarmOutputSignal(extAlarm_t(i)))
                << "signal " << extAlarm_t(i);
    }

    //
    // Проверка установки сигналов во время сброса
    //

    // Если предыдущие проверки прошли, то можно пользоваться функцией сброса
    resetSignals();
    ASSERT_FALSE(mAlarm->isReset());
    mAlarm->reset(true);
    ASSERT_TRUE(mAlarm->isReset());

    for(uint8_t i = 0; i < EXT_ALARM_MAX; i++) {
        const TExtAlarm::signal_t *s = &mAlarm->mSignal[i];

        ASSERT_TRUE(mAlarm->setAlarmInputSignal(extAlarm_t(i), true))
                << "signal " << extAlarm_t(i);

        bool result = (s->resetMode == TExtAlarm::RESET_MODE_direct);
        ASSERT_EQ(result, mAlarm->getAlarmOutputSignal(extAlarm_t(i)))
                << "signal " << extAlarm_t(i);

        ASSERT_TRUE(mAlarm->setAlarmInputSignal(extAlarm_t(i), false))
                << "signal " << extAlarm_t(i);

        result = false;
        ASSERT_EQ(false, mAlarm->getAlarmOutputSignal(extAlarm_t(i)))
                << "signal " << extAlarm_t(i);
    }

    //
    // Проверка установленных сигналов после окончания сброса
    //

    resetSignals();

    mAlarm->reset(true);

    // Все входные сигналы в true.
    // Для сигналов с RESET_MODE_off на выходе сохраняется состояние которое
    // было при сбросе на входе. Для остальных на выход будет передан вход.
    for(uint8_t i = 0; i < EXT_ALARM_MAX; i++) {
        const TExtAlarm::signal_t *s = &mAlarm->mSignal[i];

        ASSERT_TRUE(mAlarm->setAlarmInputSignal(extAlarm_t(i), true))
                << "signal " << extAlarm_t(i);

        //
        bool result = (s->resetMode != TExtAlarm::RESET_MODE_off);
        ASSERT_EQ(result, mAlarm->getAlarmOutputSignal(extAlarm_t(i)))
                << "signal " << extAlarm_t(i);
    }

    mAlarm->reset(false);

    for(uint8_t i = 0; i < EXT_ALARM_MAX; i++) {
        ASSERT_TRUE(mAlarm->getAlarmOutputSignal(extAlarm_t(i)))
                << "signal " << extAlarm_t(i);
    }

    mAlarm->reset(true);

    // Все входные сигналы в false.
    // Для сигналов с RESET_MODE_off на выходе сохраняется состояние которое
    // было при сбросе на входе. Для остальных на выход будет передан вход.
    for(uint8_t i = 0; i < EXT_ALARM_MAX; i++) {
        const TExtAlarm::signal_t *s = &mAlarm->mSignal[i];

        ASSERT_TRUE(mAlarm->setAlarmInputSignal(extAlarm_t(i), false))
                << "signal " << extAlarm_t(i);

        bool result = !(s->resetMode == TExtAlarm::RESET_MODE_direct);
        ASSERT_EQ(result, mAlarm->getAlarmOutputSignal(extAlarm_t(i)))
                << "signal " << extAlarm_t(i);
    }

    mAlarm->reset(false);

    for(uint8_t i = 0; i < EXT_ALARM_MAX; i++) {
        ASSERT_FALSE(mAlarm->getAlarmOutputSignal(extAlarm_t(i)))
                << "signal " << extAlarm_t(i);
    }
}

// Проверка работы сигнализации при запуске
TEST_F(TExtAlarm_Test, alarmSignal_default)
{
    ASSERT_FALSE(mAlarm->isReset());

    // Начальное состояние выходных сигналов
    for(uint8_t i = 0; i < EXT_ALARM_MAX; i++) {
        const TExtAlarm::signal_t *s = &mAlarm->mSignal[i];
        ASSERT_EQ(s->valDef, mAlarm->getAlarmOutputSignal(extAlarm_t(i)))
                << "signal " << extAlarm_t(i);
    }

    // Проверка установки сигналов
    for(uint8_t i = 0; i < EXT_ALARM_MAX; i++) {
        const TExtAlarm::signal_t *s = &mAlarm->mSignal[i];

        ASSERT_TRUE(mAlarm->setAlarmInputSignal(extAlarm_t(i), true))
                << "signal " << extAlarm_t(i);

        ASSERT_EQ(true, mAlarm->getAlarmOutputSignal(extAlarm_t(i)))
                << "signal " << extAlarm_t(i);

        mAlarm->setAlarmInputSignal(extAlarm_t(i), false);
        bool result = (s->alarmReset == ALARM_RESET_manual);
        ASSERT_EQ(result, mAlarm->getAlarmOutputSignal(extAlarm_t(i)))
                << "signal " << extAlarm_t(i);
    }
}

// Проверка считывания сигналов сигнлизации
TEST_F(TExtAlarm_Test, alarmSignal_manual)
{
    // Сброс всех сигналов
    ASSERT_TRUE(resetSignals());
    ASSERT_FALSE(mAlarm->isReset());

    // Установка режима сброса сигнализации
    mAlarm->setAlarmReset(ALARM_RESET_manual);
    for(uint8_t i = 0; i < EXT_ALARM_MAX; i++) {
        ASSERT_EQ(false, mAlarm->getAlarmOutputSignal(extAlarm_t(i)))
                << "signal " << extAlarm_t(i);
    }

    // Проверка установки сигналов
    for(uint8_t i = 0; i < EXT_ALARM_MAX; i++) {
        ASSERT_TRUE(mAlarm->setAlarmInputSignal(extAlarm_t(i), true))
                << "signal " << extAlarm_t(i);

        ASSERT_EQ(true, mAlarm->getAlarmOutputSignal(extAlarm_t(i)))
                << "signal " << extAlarm_t(i);

        mAlarm->setAlarmInputSignal(extAlarm_t(i), false);
        ASSERT_EQ(true, mAlarm->getAlarmOutputSignal(extAlarm_t(i)))
                << "signal " << extAlarm_t(i);
    }
}

// Проверка считывания сигналов сигнлизации
TEST_F(TExtAlarm_Test, alarmSignal_auto)
{
    // Сброс всех сигналов
    ASSERT_TRUE(resetSignals());
    ASSERT_FALSE(mAlarm->isReset());

    // Установка режима сброса сигнализации
    mAlarm->setAlarmReset(ALARM_RESET_auto);
    for(uint8_t i = 0; i < EXT_ALARM_MAX; i++) {
        ASSERT_EQ(false, mAlarm->getAlarmOutputSignal(extAlarm_t(i)))
                << "signal " << extAlarm_t(i);
    }

    // Проверка установки сигналов
    for(uint8_t i = 0; i < EXT_ALARM_MAX; i++) {
        const TExtAlarm::signal_t *s = &mAlarm->mSignal[i];

        ASSERT_TRUE(mAlarm->setAlarmInputSignal(extAlarm_t(i), true))
                << "signal " << extAlarm_t(i);

        ASSERT_EQ(true, mAlarm->getAlarmOutputSignal(extAlarm_t(i)))
                << "signal " << extAlarm_t(i);

        mAlarm->setAlarmInputSignal(extAlarm_t(i), false);
        bool result = (s->alarmReset == ALARM_RESET_manual);
        ASSERT_EQ(result, mAlarm->getAlarmOutputSignal(extAlarm_t(i)))
                << "signal " << extAlarm_t(i);
    }

}

} // namespace BVP

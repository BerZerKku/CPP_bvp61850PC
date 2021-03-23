#include "gtest/gtest.h"


#include <cstdio>
#include <iostream>

#define TEST_FRIENDS \
    friend class TExtAlarm_Test; \
    FRIEND_TEST(TExtAlarm_Test, setOutSignal); \
    FRIEND_TEST(TExtAlarm_Test, getAlarmReset_signal); \
    FRIEND_TEST(TExtAlarm_Test, setAlarmInputSignal_auto); \
    FRIEND_TEST(TExtAlarm_Test, setAlarmInputSignal_manual);


#include "bvpCommon/extAlarm.hpp"

using namespace std;

namespace BVP {


class TExtAlarm_Test: public ::testing::Test {

public:
    const uint16_t kAlarmOutDefault = 0; //(1 << EXT_ALARM_fault);
    const alarmReset_t kAlarmResetDefault = ALARM_RESET_manual;

    TExtAlarm *mAlarm;

    bool checkSignals(uint16_t value) {
        bool check = true;

        for(extAlarm_t signal = extAlarm_t(0);
            signal < EXT_ALARM_MAX;
            signal = extAlarm_t(signal + 1)) {

            bool v = (value & (1 << signal));
            if (v != mAlarm->getAlarmOutputSignal(signal)) {
                check = false;
                EXPECT_EQ(v, mAlarm->getAlarmOutputSignal(signal))
                        << "Signal: " << signal << ", value = " << std::hex << value;
            }

        }

        return check;
    }

    bool checkAlarmResetSignals(const alarmReset_t reset) {
        bool check = true;

        for(extAlarm_t signal = extAlarm_t(0);
            signal < EXT_ALARM_MAX;
            signal = extAlarm_t(signal + 1)) {

            alarmReset_t result = reset;
            if ((signal == EXT_ALARM_comPrd) || (signal == EXT_ALARM_comPrm)) {
                result = ALARM_RESET_manual;
            }

            if (result != mAlarm->getAlarmReset(signal)) {
                EXPECT_EQ(result, mAlarm->getAlarmReset(signal))
                        << "Signal: " << signal << ", alarm = " << std::hex << reset;
            }
        }

        return check;
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

// Проверка установки текущего состояния сигналов сигнализации
TEST_F(TExtAlarm_Test, alarmOutput)
{
    uint16_t value;
    ASSERT_EQ(kAlarmOutDefault, mAlarm->getAlarmOutput());

    value = (1 << EXT_ALARM_comPrd);
    mAlarm->setAlarmOutput(value);
    ASSERT_EQ(value, mAlarm->getAlarmOutput());

    value |= (1 << EXT_ALARM_disablePrm);
    mAlarm->setAlarmOutput(value);
    ASSERT_EQ(value, mAlarm->getAlarmOutput());

    mAlarm->setAlarmOutput(value | (1 << EXT_ALARM_MAX));
    ASSERT_EQ(value, mAlarm->getAlarmOutput());

    mAlarm->setAlarmOutput(0xFFFF);
    ASSERT_EQ((1 << EXT_ALARM_MAX) - 1, mAlarm->getAlarmOutput());

    value = (1 << EXT_ALARM_comPrd);
    mAlarm->setAlarmOutput(value);
    ASSERT_EQ(value, mAlarm->getAlarmOutput());
}

// Проверка считывания сигналов сигнлизации
TEST_F(TExtAlarm_Test, getAlarmOutputSignal)
{
    uint16_t value;

    value = kAlarmOutDefault;
    ASSERT_TRUE(checkSignals(value));

    value |= uint16_t (1 << EXT_ALARM_comPrd);
    value |= uint16_t (1 << EXT_ALARM_disablePrm);
    mAlarm->setAlarmOutput(value);
    ASSERT_TRUE(checkSignals(value));

    mAlarm->setAlarmOutput(0xFFFF);
    value = mAlarm->getAlarmOutput();
    ASSERT_TRUE(checkSignals(value));

    ASSERT_FALSE(mAlarm->getAlarmOutputSignal(EXT_ALARM_MAX));
    ASSERT_FALSE(mAlarm->getAlarmOutputSignal(extAlarm_t(0xFFFF)));
}


// Проверка установки сброса сигнализации
TEST_F(TExtAlarm_Test, alarmReset)
{
    alarmReset_t value;
    ASSERT_EQ(kAlarmResetDefault, mAlarm->getAlarmReset());

    value = ALARM_RESET_auto;
    mAlarm->setAlarmReset(value);
    ASSERT_EQ(value, mAlarm->getAlarmReset());

    value = ALARM_RESET_manual;
    mAlarm->setAlarmReset(value);
    ASSERT_EQ(value, mAlarm->getAlarmReset());

    value = ALARM_RESET_auto;
    mAlarm->setAlarmReset(value);
    ASSERT_EQ(value, mAlarm->getAlarmReset());

    value = ALARM_RESET_MAX;
    mAlarm->setAlarmReset(value);
    ASSERT_EQ(kAlarmResetDefault, mAlarm->getAlarmReset());

    value = ALARM_RESET_auto;
    mAlarm->setAlarmReset(value);
    ASSERT_EQ(value, mAlarm->getAlarmReset());

    value = alarmReset_t(0xFF);
    mAlarm->setAlarmReset(value);
    ASSERT_EQ(kAlarmResetDefault, mAlarm->getAlarmReset());
}

// Проверка установки сброса сигнализации для отдельных сигналов
TEST_F(TExtAlarm_Test, alarmReset_signal)
{
    for(alarmReset_t reset = alarmReset_t(0); reset < ALARM_RESET_MAX;
        reset = alarmReset_t(reset + 1)) {

        mAlarm->setAlarmReset(reset);
        ASSERT_TRUE(checkAlarmResetSignals(reset));
    }
}

// Проверка установки сигнала
TEST_F(TExtAlarm_Test, setOutSignal)
{
    uint16_t value = 0;
    uint16_t result;
    extAlarm_t signal = EXT_ALARM_model61850;

    // Проверка добавления сигнала со значением false
    result = 0;
    mAlarm->setAlarmOutput(value);
    ASSERT_EQ(result, mAlarm->setOutSignal(signal, false, ALARM_RESET_auto));
    mAlarm->setAlarmOutput(value);
    ASSERT_EQ(result, mAlarm->setOutSignal(signal, false, ALARM_RESET_manual));

    // Проверка добавления сигнала со значением true
    result = 1 << EXT_ALARM_model61850;
    mAlarm->setAlarmOutput(value);
    ASSERT_EQ(result, mAlarm->setOutSignal(signal, true, ALARM_RESET_auto));
    mAlarm->setAlarmOutput(value);
    ASSERT_EQ(result, mAlarm->setOutSignal(signal, true, ALARM_RESET_manual));

    value = (1 << EXT_ALARM_test61850);

    // Проверка добавления сигнала со значением false при наличии другого сигнала
    result = value;
    mAlarm->setAlarmOutput(value);
    ASSERT_EQ(result, mAlarm->setOutSignal(signal, false, ALARM_RESET_auto));
    mAlarm->setAlarmOutput(value);
    ASSERT_EQ(result, mAlarm->setOutSignal(signal, false, ALARM_RESET_manual));

    // Проверка добавления сигнала со значением true при наличии другого сигнала
    result |= (1 << signal);
    mAlarm->setAlarmOutput(value);
    ASSERT_EQ(result, mAlarm->setOutSignal(signal, true, ALARM_RESET_auto));
    mAlarm->setAlarmOutput(value);
    ASSERT_EQ(result, mAlarm->setOutSignal(signal, true, ALARM_RESET_manual));

    // Проверка сброса сигнала
    signal = EXT_ALARM_channelFault;
    result = value;
    value |= (1 << signal);
    mAlarm->setAlarmOutput(value);
    ASSERT_EQ(value, mAlarm->setOutSignal(signal, false, ALARM_RESET_manual));
    mAlarm->setAlarmOutput(value);
    ASSERT_EQ(result, mAlarm->setOutSignal(signal, false, ALARM_RESET_auto));
}

// Установка сигналов при автоматическом сбросе
TEST_F(TExtAlarm_Test, setAlarmInputSignal_auto)
{    
    uint16_t result;

    mAlarm->setAlarmOutput(0);
    ASSERT_EQ(int(0), mAlarm->getAlarmOutput());

    // Автоматический сброс
    mAlarm->setAlarmReset(ALARM_RESET_auto);

    // сброс сигнала при его отсутствии
    for(extAlarm_t s = extAlarm_t(0); s < EXT_ALARM_MAX; s = extAlarm_t(s + 1)) {
        ASSERT_TRUE(mAlarm->setAlarmInputSignal(s, false));
        ASSERT_TRUE(checkSignals(0));
    }

    result = 0;
    // установка сигнала
    for(extAlarm_t s = extAlarm_t(0); s < EXT_ALARM_MAX; s = extAlarm_t(s + 1)) {
        ASSERT_TRUE(mAlarm->setAlarmInputSignal(s, true));
        result |= (1 << s);
        ASSERT_TRUE(checkSignals(result));
    }

    // сброс сигнала при его наличии
    for(extAlarm_t s = extAlarm_t(0); s < EXT_ALARM_MAX; s = extAlarm_t(s + 1)) {
        ASSERT_TRUE(mAlarm->setAlarmInputSignal(s, false));
        if (mAlarm->getAlarmReset(s) != ALARM_RESET_manual) {
            result &= ~(1 << s);
        }
        ASSERT_TRUE(checkSignals(result));
    }
}

// Установка сигналов при ручном сбросе
TEST_F(TExtAlarm_Test, setAlarmInputSignal_manual) {
    uint16_t result;

    mAlarm->setAlarmOutput(0);
    ASSERT_EQ(int(0), mAlarm->getAlarmOutput());

    mAlarm->setAlarmReset(ALARM_RESET_manual);

    // сброс сигнала при его отсутствии
    for(extAlarm_t s = extAlarm_t(0); s < EXT_ALARM_MAX; s = extAlarm_t(s + 1)) {
        ASSERT_TRUE(mAlarm->setAlarmInputSignal(s, false));
        ASSERT_TRUE(checkSignals(0));
    }

    result = 0;
    // установка сигнала
    for(extAlarm_t s = extAlarm_t(0); s < EXT_ALARM_MAX; s = extAlarm_t(s + 1)) {
        ASSERT_TRUE(mAlarm->setAlarmInputSignal(s, true));
        result |= (1 << s);
        ASSERT_TRUE(checkSignals(result));
    }

    // сброс сигнала при его наличии
    for(extAlarm_t s = extAlarm_t(0); s < EXT_ALARM_MAX; s = extAlarm_t(s + 1)) {
        ASSERT_TRUE(mAlarm->setAlarmInputSignal(s, false));
        ASSERT_TRUE(checkSignals(result));
    }
}

} // namespace BVP

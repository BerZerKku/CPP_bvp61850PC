#include "gtest/gtest.h"


#include <cstdio>
#include <iostream>

#define TEST_FRIENDS \
    FRIEND_TEST(TExtAlarm_Test, setSignal); \
    FRIEND_TEST(TExtAlarm_Test, getExtAlarm); \
    FRIEND_TEST(TExtAlarm_Test, getDisablePrm);

#include "bvpCommon/extAlarm.hpp"

using namespace std;

namespace BVP {


class TExtAlarm_Test: public ::testing::Test {

   public:
    TExtAlarm *alarm;
    TParam *param = TParam::getInstance();

   protected:

    void SetUp() override {
        alarm = new BVP::TExtAlarm();
    }

    void TearDown()override {
        delete alarm;
    }
};


TEST_F(TExtAlarm_Test, setSignal) {
    uint16_t value;
    uint16_t result;
    extAlarmOut_t signal = EXT_ALARM_OUT_model61850;

    value = 0;
    result = alarm->setSignal(value, false, signal, ALARM_RESET_auto);
    ASSERT_EQ(int{0}, result);
    result = alarm->setSignal(value, false, signal, ALARM_RESET_manual);
    ASSERT_EQ(int{0}, result);
    result = alarm->setSignal(value, true, signal, ALARM_RESET_auto);
    ASSERT_EQ(EXT_ALARM_OUT_model61850, result);
    result = alarm->setSignal(value, true, signal, ALARM_RESET_manual);
    ASSERT_EQ(EXT_ALARM_OUT_model61850, result);

    value = 1;
    result = alarm->setSignal(value, false, signal, ALARM_RESET_auto);
    ASSERT_EQ(int{0}, result);
    result = alarm->setSignal(value, false, signal, ALARM_RESET_manual);
    ASSERT_EQ(EXT_ALARM_OUT_model61850, result);
    result = alarm->setSignal(value, true, signal, ALARM_RESET_auto);
    ASSERT_EQ(EXT_ALARM_OUT_model61850, result);
    result = alarm->setSignal(value, true, signal, ALARM_RESET_manual);
    ASSERT_EQ(EXT_ALARM_OUT_model61850, result);
}

TEST_F(TExtAlarm_Test, getExtAlarm) {
    bool ok;
    ASSERT_EQ(EXT_ALARM_OUT_fault, alarm->getExtAlarm());

    ok = param->setValue(PARAM_extAlarm, SRC_int, EXT_ALARM_OUT_channelFault);
    ASSERT_TRUE(ok);
    ASSERT_EQ(EXT_ALARM_OUT_channelFault, alarm->getExtAlarm());

    ok = param->setValue(PARAM_extAlarm, SRC_int, 0xFFFF);
    ASSERT_TRUE(ok);
    ASSERT_EQ(0xFFFF, alarm->getExtAlarm());
}

TEST_F(TExtAlarm_Test, getDisablePrm) {
    bool ok;
    ASSERT_EQ(ON_OFF_off, alarm->getDisablePrm());

    ok = param->setValue(PARAM_blkComPrmAll, SRC_int, ON_OFF_on);
    ASSERT_TRUE(ok);
    ASSERT_EQ(ON_OFF_on, alarm->getDisablePrm());

    ok = param->setValue(PARAM_blkComPrmAll, SRC_int, 0xFFFF);
    ASSERT_TRUE(ok);
    ASSERT_EQ(ON_OFF_off, alarm->getExtAlarm());
}

} // namespace BVP

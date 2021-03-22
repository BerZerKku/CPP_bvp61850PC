#include "gtest/gtest.h"


#include <cstdio>
#include <iostream>
#include <limits>

#define TEST_FRIENDS \
//    friend class TClock_Test;

#include "bvpCommon/clock.hpp"

using namespace std;

namespace BVP {

class TClock_Test: public ::testing::Test {

public:

protected:

    void SetUp() override { }

    void TearDown() override { }
};

//
TEST_F(TClock_Test, clock)
{
    uint16_t ticktime;
    clockPoint_t point = 0;
    // Начальное состояние.
    ASSERT_EQ(point, TClock::getClockPoint());

    ticktime = 1;
    TClock::setTickInMs(ticktime);
    while(point < 100) {
        TClock::tick();
        point += ticktime;

        ASSERT_EQ(point, TClock::getClockPoint());
    }

    ticktime = 100;
    TClock::setTickInMs(ticktime);
    while(point < 1000) {
        TClock::tick();
        point += ticktime;

        ASSERT_EQ(point, TClock::getClockPoint());
    }

    ticktime = 1000;
    TClock::setTickInMs(ticktime);
    while(point < 10000) {
        TClock::tick();
        point += ticktime;

        ASSERT_EQ(point, TClock::getClockPoint());
    }

    ticktime = 0xFFFF;
    TClock::setTickInMs(ticktime);
    while(point <= (0xFFFFFFFF - ticktime)) {
        TClock::tick();
        point += ticktime;

        ASSERT_EQ(point, TClock::getClockPoint());
    }

    ticktime = 1;
    TClock::setTickInMs(ticktime);
    while(point > 0) {
        TClock::tick();
        point += ticktime;

        ASSERT_EQ(point, TClock::getClockPoint());
    }

    // Проверка перехода через 0

    ASSERT_EQ(int(0), TClock::getClockPoint());
}

//
TEST_F(TClock_Test, getDuration)
{
    clockPoint_t ctime = 0;
    clockPoint_t ltime = 0;

    //
    // Установка начального времени
    //

    uint16_t tick = 30000;
    TClock::reset();
    ASSERT_EQ(ctime, TClock::getClockPoint());

    TClock::setTickInMs(tick);
    TClock::tick();
    ctime += tick;
    ASSERT_EQ(ctime, TClock::getClockPoint());

    //
    // Проверка времени с значением нового отсчета больше предыдущего
    //

    ltime = 0;
    ASSERT_EQ(ctime - ltime, TClock::getDurationMs(ltime));
    ASSERT_EQ((ctime - ltime)/1000, TClock::getDurationS(ltime));

    ltime = 1111;
    ASSERT_EQ(ctime - ltime, TClock::getDurationMs(ltime));
    ASSERT_EQ((ctime - ltime)/1000, TClock::getDurationS(ltime));

    ltime = tick - 1;
    ASSERT_EQ(ctime - ltime, TClock::getDurationMs(ltime));
    ASSERT_EQ((ctime - ltime)/1000, TClock::getDurationS(ltime));

    ltime = tick;
    ASSERT_EQ(ctime - ltime, TClock::getDurationMs(ltime));
    ASSERT_EQ(((ctime - ltime))/1000, TClock::getDurationS(ltime));

    //
    // Проверка времени с значением нового отсчета меньше предыдущего
    //

    ltime = clockPoint_t(-tick);
    ASSERT_EQ(UINT32_MAX - tick + 1, ltime);
    ASSERT_EQ(2*tick, TClock::getDurationMs(ltime));
    ASSERT_EQ((2*tick)/1000, TClock::getDurationS(ltime));
}

} // namespace BVP

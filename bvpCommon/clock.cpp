/*
 * clock.cpp
 *
 *  Created on: March 22, 2021
 *      Author: bear
 */

#include "clock.hpp"

namespace BVP
{

clockPoint_t TClock::mClockCounter = 0;

uint16_t TClock::mTickTimeMs = 0;


TClock::TClock()
{
    Q_STATIC_ASSERT(sizeof(clockPoint_t) >= 4);
    Q_STATIC_ASSERT(sizeof(mTickTimeMs) < sizeof(clockPoint_t));
}

}

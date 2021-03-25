/*
 * bvpCommon.hpp
 *
 *  Created on: Aug 20, 2020
 *      Author: bear
 */

#ifndef BVP_BASE_H_
#define BVP_BASE_H_

#include "debug.hpp"
#include <cstdint>
#include "extAlarm.hpp"
#include "param.h"
#include "serial/modbusVp.h"
#include "serial/avantpi.h"
#include "serial/avantpc.h"

namespace BVP {

class TBvpBase {

    const uint16_t kModbusVpAddress = 10;
    static constexpr uint16_t kSerialBufferSize = 256;

public:

    TBvpBase();
    ~TBvpBase() {}

    void init();
    void loop();

    void tick1ms();

    uint16_t protPop(src_t src, uint8_t *data[]);
    bool protPush(src_t src, uint8_t byte);
    void protTick();
    void protSetTick(uint32_t tickus);
    bool protIsConnection(src_t src);
    void protSendFinished(src_t src);

private:

    uint8_t mSerialBuf[SRC_MAX][kSerialBufferSize];

    TExtAlarm mAlarm;
    TParam * const mParam;

    TModbusVp mSerialVp;
    TAvantPc mSerialPc;
    TAvantPi mSerialPi;

    TSerialProtocol * mSerial[src_t::SRC_MAX];

    void alarmLoop();
    void alarmResetLoop();

    void serialLoop();
};

} // namespace BVP

#endif /* BVPCOMMON_HPP_ */

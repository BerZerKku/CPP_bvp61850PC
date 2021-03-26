#include "avantpc.h"

namespace BVP {


TAvantPc::TAvantPc(regime_t regime) :
    TProtocolAvant(regime),
    mComRx(COM_getTime) {

    Q_ASSERT(regime == REGIME_slave);
}

//
bool
BVP::TAvantPc::vWriteAvant() {
    bool ok = false;
    bool iscommand = false;
    uint32_t value = 0;

    if (mIsComRx == true) {
        if (mComRx == 0x01) {
            setCom(mComRx);
            value = mParam->getValue(PARAM_debug1, mSrcId, ok);
            addByte(static_cast<uint8_t> (value));
            addByte(static_cast<uint8_t> (value >> 8));
            addByte(static_cast<uint8_t> (value >> 16));
            addByte(static_cast<uint8_t> (value >> 24));
            value = mParam->getValue(PARAM_debug2, mSrcId, ok);
            addByte(static_cast<uint8_t> (value));
            addByte(static_cast<uint8_t> (value >> 8));
            addByte(static_cast<uint8_t> (value >> 16));
            addByte(static_cast<uint8_t> (value >> 24));
            // добивка до 16 байт в посылке, для удобного вывода в CuteCom
            addByte(0);
            addByte(0);
            addByte(0);
            iscommand = true;
        } else if (mComRx == 0x02) {
            setCom(mComRx);
            iscommand = true;
        }

        mIsComRx = false;
    }

    return iscommand;
}

//
bool
TAvantPc::vReadAvant() {
    mComRx = static_cast<com_t> (mBuf[POS_COM]);
    mIsComRx = true;
    uint32_t value = 0;

    if (mComRx == 0x02) {
        Q_ASSERT(mBuf[POS_DATA_LEN] == 4);
        value = mBuf[POS_DATA + 3];
        value <<= 8;
        value += mBuf[POS_DATA + 2];
        value <<= 8;
        value += mBuf[POS_DATA + 1];
        value <<= 8;
        value += mBuf[POS_DATA];
        mParam->setValue(PARAM_debug2, mSrcId, value);
    }

    transferTo(SRC_pi, mBuf, mLen);

    return mIsComRx;
}

} // namespace BVP

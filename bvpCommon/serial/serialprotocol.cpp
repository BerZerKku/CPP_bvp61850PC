#include "serialprotocol.h"

namespace BVP {

//
TSerialProtocol::TSerialProtocol(regime_t regime) :
  mRegime(regime),
  mParam(nullptr),
  mBuf(nullptr),
  mSize(0),
  mPos(0),
  mLen(0),
  mNetAddress(0xFF),
  mTimeReadStart(0),
  mTimeUs(0),
  mTimeTickUs(0),
  mTimeOneByteUs(0) {

  *(const_cast<TParam**> (&mParam)) = TParam::getInstance();
}

//
TSerialProtocol::~TSerialProtocol() {

}

//
bool
TSerialProtocol::push(uint8_t byte) {
  if (mPos == 0) {
    mTimeReadStart = 0;
  }

  return vPush(byte);
}

//
bool
TSerialProtocol::setTimeTick(uint32_t ticktimeus) {
  *(const_cast<uint32_t*> (&mTimeTickUs)) = ticktimeus;

  Q_ASSERT(mTimeTickUs > 0);

  return mTimeTickUs != 0;
}

//
bool
TSerialProtocol::setup(uint32_t baudrate, bool parity, uint8_t stopbits) {
  uint8_t nbites = 1 + 8 + stopbits + (parity ? 1 : 0);

  *(const_cast<uint32_t*> (&mTimeOneByteUs)) =
      static_cast<uint32_t> ((1000000UL / baudrate) * nbites);

  return vSetup(baudrate, parity, stopbits);
}

//
void
TSerialProtocol::tick() {
  mTimeReadStart = (mTimeReadStart < kMaxTimeFromReadFirstByte - mTimeTickUs) ?
                    mTimeReadStart + mTimeTickUs : kMaxTimeFromReadFirstByte;

  vTick();
}

//
void
TSerialProtocol::setBuffer(uint8_t buf[], uint16_t size) {
  *(const_cast<uint16_t*> (&mSize)) = size;
  *(const_cast<uint8_t**> (&mBuf)) = buf;
}

} // namespace BVP

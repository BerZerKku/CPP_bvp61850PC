#include "global.hpp"
#include "protocolAvant.h"

namespace BVP {

//
TProtocolAvant::TProtocolAvant(regime_t regime) :
  TSerialProtocol(regime),
  mState(STATE_disable),
  cntLostMessage(kMaxLostMessage) {

}

//
TProtocolAvant::~TProtocolAvant()
{

}

//
bool TProtocolAvant::setEnable(bool enable)
{
  Q_ASSERT(mSrcId != SRC_MAX);
  Q_ASSERT(mParam != nullptr);
  Q_ASSERT(mBuf != nullptr);
  Q_ASSERT(mSize != 0);

  if (enable) {
    setDefaultState();
    mLen = 0;
    mTimeUs = 0;
  } else {
    mState = STATE_disable;
    cntLostMessage = kMaxLostMessage;
  }

  return mState != STATE_disable;
}

//
bool TProtocolAvant::isEnable() const
{
  return mState != STATE_disable;
}

//
bool TProtocolAvant::read()
{
  bool isread = false;

  if (mState == STATE_procReply) {
    uint8_t crcpkg = mBuf[--mLen];
    bool ok = (crcpkg == calcCRC(&mBuf[POS_COM], mLen - POS_COM));

    if (ok) {
      ok  = vReadAvant();
      cntLostMessage = 0;
    }

    if (!ok) {
      incLostMessageCounter();
    }

    isread = ok;
    mState = STATE_idle;
  }

  return isread;
}

//
void TProtocolAvant::readError()
{
  if (mState == STATE_waitForReply) {
    mState = STATE_errorReply;
    mTimeUs = 0;
  }
}

//
bool TProtocolAvant::write()
{

  if (mState == STATE_idle) {
    bool ok = true;
    uint16_t len = 0;

    mLen = 0;
    ok = vWriteAvant();

    if (ok) {
      mBuf[POS_PMBL_55] = 0x55;
      mBuf[POS_PMBL_AA] = 0xAA;
      len = kMinLenFrame + mBuf[POS_DATA_LEN] - 1;
      uint8_t crc =  calcCRC(&mBuf[POS_COM], len - POS_COM);
      mBuf[len++] = crc;

      if (len > 0) {
        mLen = len;
        mPos = 0;
        mTimeUs = 0;
        mState = STATE_reqSend;
      }
    } else {
      setDefaultState();
    }
  }

  return mState == STATE_reqSend;
}

//
uint16_t TProtocolAvant::pop(uint8_t *data[])
{
  uint16_t len = 0;
  *data = &mBuf[mPos];

  Q_ASSERT(mState == STATE_reqSend);

  if (mState == STATE_reqSend) {
    len = mLen;
    mState = STATE_waitSendFinished;
  }

  return len;
}

//
void TProtocolAvant::sendFinished()
{
    // FIXME При работе с виртуальными портаи периодически вылетает ошибка!
//  Q_ASSERT(mState == STATE_waitSendFinished || mState == STATE_disable);

  if (mState != STATE_disable) {
    if (mState == STATE_waitSendFinished) {
      mPos = 0;
      mLen = 0;
      mTimeUs = 0;
      mState = STATE_waitForReply;
    } else {
      setDefaultState();
    }
  }
}

//
bool TProtocolAvant::vPush(uint8_t byte)
{
  uint16_t pos = 0;

  if (mState == STATE_waitForReply) {
    pos = mPos;
    mBuf[pos] = byte;

    if (pos == POS_PMBL_55) {
      pos = (byte == 0x55) ? pos + 1 : 0;
    } else if (pos == POS_PMBL_AA) {
      pos = (byte == 0xAA) ? pos + 1 : 0;
      mLen = kMinLenFrame;
    } else if (pos == POS_DATA_LEN) {
      if (mSize >= (kMinLenFrame + byte)) {
        mLen += byte;
        pos = pos + 1;
      } else {
        pos = 0;
      }
    } else {
      pos++;
      if (pos >= mLen) {
        mState = STATE_procReply;
      }
    }

    mTimeUs = 0;
    mPos = pos;
  }

  if (mState == STATE_errorReply) {
    mTimeUs = 0;
  }

  return pos != 0;
}

//
bool TProtocolAvant::setNetAddress(uint16_t address)
{
  *(const_cast<uint8_t *> (&mNetAddress)) = static_cast<uint8_t> (address);

  return mNetAddress == address;
}

//
bool TProtocolAvant::vSetup(uint32_t baudrate, bool parity, uint8_t stopbits)
{
  UNUSED(baudrate);
  UNUSED(parity);
  UNUSED(stopbits);

  return mTimeOneByteUs > 0;
}

//
void TProtocolAvant::vTick()
{
  if (mState != STATE_disable) {
    if (mTimeUs < kMaxTimeToResponseUs) {
      mTimeUs += mTimeTickUs;
    }

    if (mTimeUs >= kMaxTimeToResponseUs) {
      setDefaultState();
      incLostMessageCounter();
    }
  }
}

//
bool TProtocolAvant::isConnection() const
{
  return (mState != STATE_disable) && (cntLostMessage < kMaxLostMessage);
}

//
void TProtocolAvant::incLostMessageCounter()
{
  if (cntLostMessage < kMaxLostMessage) {
    cntLostMessage++;
  }
}

//
void TProtocolAvant::setCom(uint8_t com)
{
  mBuf[POS_COM] = com;
  mBuf[POS_DATA_LEN] = 0;
}

//
void TProtocolAvant::addByte(uint8_t byte, uint16_t nbyte)
{
  nbyte = (nbyte > 0) ? nbyte - 1 : mBuf[POS_DATA_LEN] + 1;

  Q_ASSERT(nbyte < (mSize - kMinLenFrame));

  if (mBuf[POS_DATA_LEN] < nbyte) {
    mBuf[POS_DATA_LEN] = static_cast<uint8_t> (nbyte);
  }

  mBuf[POS_DATA + nbyte - 1] = byte;
}

//
uint8_t TProtocolAvant::calcCRC(const uint8_t buf[], size_t len, uint8_t crc)
{
  for(size_t i = 0; i < len; i++) {
    crc += buf[i];
  }

  return crc;
}

//
void TProtocolAvant::setDefaultState()
{
  mState = (mRegime == REGIME_master) ? STATE_idle : STATE_waitForReply;
  mPos = 0;
  mLen = 0;
}

} // namespace BVP

#include "avantpi.h"

namespace BVP {


TAvantPi::TAvantPi(regime_t regime) :
  TProtocolAvant(regime) {
  Q_ASSERT(regime == REGIME_master);

  ringComArray.add(COM_AVANT_getError);
  ringComArray.add(COM_AVANT_getTime);

}

//
bool
BVP::TAvantPi::vWriteAvant() {
  bool ok = false;
  uint32_t value = 0;

  // Сигналы управления
  value = mParam->getValue(PARAM_control, mSrc, ok);
  ok = ok && (value > 0);
  if (ok) {
    ok = fillComControl(value);
  }

  // Если других команд нет, то команда опроса.
  if (!ok && !ringComArray.isEmpty()) {
    setCom(ringComArray.get());
    ok = true;
  }

  return ok;
}

//
bool
TAvantPi::vReadAvant() {
  bool ok = false;

  switch(static_cast<comAvant_t> (mBuf[POS_COM])) {
    case COM_AVANT_getError: {
      ok = comGetError();
    } break;
    case COM_AVANT_getTime: {
      ok = comGetTime();
    } break;
  }

  return ok;
}

//
bool
TAvantPi::fillComControl(uint32_t value) {
  bool ok = false;

  Q_ASSERT(value < CTRL_MAX);
  if (value < CTRL_MAX) {

    switch(static_cast<ctrl_t> (value)) {

      case CTRL_resetErrors: {
        setCom(COM_AVANT_setControl);
        addByte(COM_CONTROL_BYTES_selfReset);
        ok = true;
      } break;

      case CTRL_resetIndication: {
        setCom(COM_AVANT_setPrmResetInd);
        ok = true;
      } break;

      case CTRL_reset: {
        setCom(COM_AVANT_setControl);
        addByte(COM_CONTROL_BYTES_selfReset);
        ok = true;
      } break;

      case CTRL_MAX: break;

    }
  }

  return ok;
}

//
bool
TAvantPi::comGetError() {
  bool ok = false;
  uint16_t pos = POS_DATA;
  uint32_t value = 0;

  if (mBuf[POS_DATA_LEN] >= 20) {
    ok = true;
    value = mBuf[pos++];
    value = (value << 8) + mBuf[pos++];
    mParam->setValue(PARAM_defError, mSrc, value);
    value = mBuf[pos++];
    value = (value << 8) + mBuf[pos++];
    mParam->setValue(PARAM_defWarning, mSrc, value);
    value = mBuf[pos++];
    value = (value << 8) + mBuf[pos++];
    mParam->setValue(PARAM_prmError, mSrc, value);
    value = mBuf[pos++];
    value = (value << 8) + mBuf[pos++];
    mParam->setValue(PARAM_prmWarning, mSrc, value);
    value = mBuf[pos++];
    value = (value << 8) + mBuf[pos++];
    mParam->setValue(PARAM_prdError, mSrc, value);
    value = mBuf[pos++];
    value = (value << 8) + mBuf[pos++];
    mParam->setValue(PARAM_prdWarning, mSrc, value);
    value = mBuf[pos++];
    value = (value << 8) + mBuf[pos++];
    mParam->setValue(PARAM_glbError, mSrc, value);
    value = mBuf[pos++];
    value = (value << 8) + mBuf[pos++];
    mParam->setValue(PARAM_glbWarning, mSrc, value);
    value = mBuf[pos++];
    value = (value << 8) + mBuf[pos++];
    mParam->setValue(PARAM_prm2Warning, mSrc, value);
    value = mBuf[pos++];
    value = (value << 8) + mBuf[pos++];
    mParam->setValue(PARAM_prm2Error, mSrc, value);
    Q_ASSERT(pos == (POS_DATA + 20));
  }

  if (mBuf[POS_DATA_LEN] >= 28) {
    value = mBuf[pos++];
    value = (value << 8) + mBuf[pos++];
    mParam->setValue(PARAM_defRemoteError, mSrc, value);
    value = mBuf[pos++];
    value = (value << 8) + mBuf[pos++];
    mParam->setValue(PARAM_prmRemoteError, mSrc, value);
    value = mBuf[pos++];
    value = (value << 8) + mBuf[pos++];
    mParam->setValue(PARAM_prdRemoteError, mSrc, value);
    value = mBuf[pos++];
    value = (value << 8) + mBuf[pos++];
    mParam->setValue(PARAM_glbRemoteError, mSrc, value);
    Q_ASSERT(pos == (POS_DATA + 28));
  }

  return ok;
}

//
bool
TAvantPi::comGetTime() {
  bool ok = false;
  uint16_t pos = POS_DATA;
  uint32_t value = 0;

  if (mBuf[POS_DATA_LEN] >= 6) {
    mParam->setValue(PARAM_dateYear, mSrc, bcd2int(mBuf[pos++]));
    mParam->setValue(PARAM_dateMonth, mSrc, bcd2int(mBuf[pos++]));
    mParam->setValue(PARAM_dateDay, mSrc, bcd2int(mBuf[pos++]));
    mParam->setValue(PARAM_timeHour, mSrc, bcd2int(mBuf[pos++]));
    mParam->setValue(PARAM_timeMin, mSrc, bcd2int(mBuf[pos++]));
    mParam->setValue(PARAM_timeSec, mSrc, bcd2int(mBuf[pos++]));
    Q_ASSERT(pos == (POS_DATA + 6));
  }

  if (mBuf[POS_DATA_LEN] >= 8) {
    value = *(reinterpret_cast<uint16_t*> (&mBuf[pos]));
    pos += sizeof(uint16_t);
    mParam->setValue(PARAM_timeMSec, mSrc, value);
    Q_ASSERT(pos == (POS_DATA + 8));
  }

  if (mBuf[POS_DATA_LEN] >= 21) {
    // TODO Добавить обработку записи журнала и запрос для него!
    Q_ASSERT(pos == (POS_DATA + 21));
  }

  return ok;
}

} // namespace BVP

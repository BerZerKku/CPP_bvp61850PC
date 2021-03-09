#include "avantpc.h"

namespace BVP {


TAvantPc::TAvantPc(regime_t regime) :
  TProtocolAvant(regime),
  comRx(COM_AVANT_getTime) {

  Q_ASSERT(regime == REGIME_slave);
}

//
bool
BVP::TAvantPc::vWriteAvant() {
  bool ok = false;
  uint32_t value = 0;

  if (isComRx == true) {    
    if (comRx == COM_AVANT_getError) {
      setCom(comRx);

      value = mParam->getValue(PARAM_defError, mSrc, ok);
      addByte(value >> 8);
      addByte(value);

      if (ok) {
        value = mParam->getValue(PARAM_defWarning, mSrc, ok);
        addByte(value >> 8);
        addByte(value);
      }

      if (ok) {
        value = mParam->getValue(PARAM_prmError, mSrc, ok);
        addByte(value >> 8);
        addByte(value);
      }

      if (ok) {
        value = mParam->getValue(PARAM_prmWarning, mSrc, ok);
        addByte(value >> 8);
        addByte(value);
      }

      if (ok) {
        value = mParam->getValue(PARAM_prdError, mSrc, ok);
        addByte(value >> 8);
        addByte(value);
      }

      if (ok) {
        value = mParam->getValue(PARAM_prdWarning, mSrc, ok);
        addByte(value >> 8);
        addByte(value);
      }

      if (ok) {
        value = mParam->getValue(PARAM_glbError, mSrc, ok);
        addByte(value >> 8);
        addByte(value);
      }

      if (ok) {
        value = mParam->getValue(PARAM_glbWarning, mSrc, ok);
        addByte(value >> 8);
        addByte(value);
      }

      // TODO Неисправности удаленного для РЗСК
      //      bool tok;
      //      value = mParam->getValue(PARAM_defRemoteError, mSrc, tok);
      //      if (tok) {
      //        mBuf[pos++] = value >> 8;
      //        mBuf[pos++] = value;
      //        value = mParam->getValue(PARAM_prmRemoteError, mSrc, tok);
      //        mBuf[pos++] = value >> 8;
      //        mBuf[pos++] = value;
      //        value = mParam->getValue(PARAM_prdRemoteError, mSrc, tok);
      //        mBuf[pos++] = value >> 8;
      //        mBuf[pos++] = value;
      //        value = mParam->getValue(PARAM_glbRemoteError, mSrc, tok);
      //        mBuf[pos++] = value >> 8;
      //        mBuf[pos++] = value;
      //      }



      if (ok) {
        uint8_t pos = mBuf[POS_DATA_LEN];
        Q_ASSERT((pos == 16) || (pos == 24));
      }
    }

    isComRx = false;
  }

  return ok;
}

//
bool
TAvantPc::vReadAvant() {
  comRx = static_cast<comAvant_t> (mBuf[POS_COM]);
  isComRx = true;

  return isComRx;
}

} // namespace BVP

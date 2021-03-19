#ifndef AVANT_PC_H
#define AVANT_PC_H

#include "protocolAvant.h"

namespace BVP {

class TAvantPc : public TProtocolAvant {

public:
  TAvantPc(regime_t regime);

  bool vWriteAvant() override;
  bool vReadAvant() override;

private:
  bool mIsComRx = false;
  com_t mComRx;
};

} // namespace BVP

#endif // AVANT_PC_H

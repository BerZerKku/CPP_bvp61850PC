#include "avant_rpi.h"

namespace BVP
{

TAvantRPi::TAvantRPi(regime_t regime) : TProtocolAvant(regime)
{
    Q_ASSERT(regime == REGIME_slave);
}


//
bool BVP::TAvantRPi::vWriteAvant()
{
    bool ok = false;

//    if (mIsComRx)
//    {
//        mIsComRx = false;
        ok       = true;
//    }

    return ok;
}

//
bool TAvantRPi::vReadAvant()
{
    bool ok = true;

    mIsComRx = true;

    return ok;
}

}  // namespace BVP

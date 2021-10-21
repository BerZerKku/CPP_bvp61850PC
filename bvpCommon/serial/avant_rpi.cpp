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

    qDebug() << __PRETTY_FUNCTION__;

    if (mIsComRx)
    {
        mIsComRx = false;
        ok       = true;
    }

    return ok;
}

//
bool TAvantRPi::vReadAvant()
{
    bool ok = true;

    mIsComRx = true;
    qDebug() << __PRETTY_FUNCTION__;

    return ok;
}

}  // namespace BVP

#include "avantpi.h"

namespace BVP {


TAvantPi::TAvantPi(regime_t regime) :
    TProtocolAvant(regime)
{
    Q_ASSERT(regime == REGIME_master);

    ringComArray.add(COM_AVANT_getMisc);
    ringComArray.add(COM_AVANT_getPrmDisable);
    ringComArray.add(COM_AVANT_getPrmBlock);
    ringComArray.add(COM_AVANT_getPrdBlock);
    //    ringComArray.add(COM_AVANT_getTime);

    Q_ASSERT(!ringComArray.isEmpty());
}

//
bool TAvantPi::writeComControl()
{
    const uint32_t mask = (uint32_t(1) << CTRL_MAX) - 1;
    bool ok = false;
    bool iscommand = false;

    // Сигналы управления
    uint32_t value = mParam->getValue(PARAM_control, mSrc, ok) ;
    if (ok) {
        if ((value & mask) > 0) {
            qDebug() << "value " << value;
            for(uint8_t i = 0; (i < CTRL_MAX); i++) {
                uint32_t current = uint32_t(1) << i;
                if (value & current) {
                    switch(static_cast<ctrl_t> (i)) {
                        case CTRL_resetIndication: {
                            setCom(COM_AVANT_setPrmResetInd);
                            iscommand = true;
                        } break;

                        case CTRL_resetErrors:  // DOWN
                        case CTRL_reset: {
                            setCom(COM_AVANT_setControl);
                            addByte(COM_CONTROL_BYTES_selfReset);
                            iscommand = true;
                        } break;

                        case CTRL_MAX: break;
                    }

                    if (iscommand) {
                        // Удаление обработанного сигнала управления
                        value &= ~current;
                        mParam->setValue(PARAM_control, mSrc, value);
                        break;
                    }
                }
            }
            qDebug() << "    , value " << value;
        }
    }

    return iscommand ;
}

//
bool TAvantPi::writeComMisc()
{
    bool ok = false;

    param_t param = PARAM_dirControl;
    if (mParam->isModified(param)) {
        setCom(COM_AVANT_getMisc | COM_AVANT_MASK_GROUP_writeParam);
        addByte(COM_MISC_BYTES_vpSac2);
        addByte(static_cast<uint8_t>(mParam->getValueW(param)));
        ok = true;
    }

    return ok;
}

//
bool TAvantPi::writeComPrmDisable()
{
    bool ok = false;

    param_t param = PARAM_blkComPrmAll;
    if (mParam->isModified(param)) {
        qDebug() << "Prm disable value set to  " << mParam->getValueW(param);
        setCom(COM_AVANT_getPrmDisable | COM_AVANT_MASK_GROUP_writeParam);
        addByte(COM_PRM_DISABLE_BYTES_prmDisable);
        addByte(static_cast<uint8_t>(mParam->getValueW(param)));
        ok = true;
    }

    return ok;
}

//
bool TAvantPi::comGetError()
{
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
bool TAvantPi::comGetMisc(comAvantMaskGroup_t group)
{
    bool ok = false;
    uint16_t nbytes = 0;

    uint16_t len = mBuf[POS_DATA_LEN];
    if (group == COM_AVANT_MASK_GROUP_read) {
        if (len >= (COM_MISC_BYTES_MAX - 1)) {
            ok = true;

            while(nbytes < len) {
                nbytes += comGetMiscGet(comMiscBytes_t(nbytes + 1),
                                        &mBuf[POS_DATA + nbytes], len - nbytes);
            }
        }
    } else if (group == COM_AVANT_MASK_GROUP_writeParam) {
        if (mBuf[POS_DATA] < COM_MISC_BYTES_MAX) {
            ok = true;
            comGetMiscGet(comMiscBytes_t(mBuf[POS_DATA]),
                          &mBuf[POS_DATA + 1], len - 1);
        }
    }

    return ok;
}

bool TAvantPi::comGetPrmDisable(comAvantMaskGroup_t group)
{
    bool ok = false;
    uint16_t nbytes = 0;

    uint16_t len = mBuf[POS_DATA_LEN];

    if (group == COM_AVANT_MASK_GROUP_read) {
        if (len >= (COM_PRM_DISABLE_BYTES_MAX - 1)) {
            ok = true;
            while(nbytes < len) {
                nbytes += comGetPrmDisableGet(comPrmDisableBytes_t(nbytes + 1),
                                        &mBuf[POS_DATA + nbytes], len - nbytes);
            }
        }
    } else if (group == COM_AVANT_MASK_GROUP_writeParam) {
        if (mBuf[POS_DATA] < COM_PRM_DISABLE_BYTES_MAX) {
            ok = true;
            comGetPrmDisableGet(comPrmDisableBytes_t(mBuf[POS_DATA]),
                                &mBuf[POS_DATA + 1], len - 1);
        }
    }

    return ok;
}

uint16_t TAvantPi::comGetMiscGet(TAvantPi::comMiscBytes_t pos,
                                const uint8_t *buf, uint16_t len)
{
    uint16_t numbytes = len;

    switch(pos) {
        case COM_MISC_BYTES_netAdr: {
            numbytes = 1;
        } break;
        case COM_MISC_BYTES_protocol: {
            numbytes = 1;
        } break;
        case COM_MISC_BYTES_baudrate: {
            numbytes = 1;
        } break;
        case COM_MISC_BYTES_dataBits: {
            numbytes = 1;
        } break;
        case COM_MISC_BYTES_parity: {
            numbytes = 1;
        } break;
        case COM_MISC_BYTES_stopBits: {
            numbytes = 1;
        } break;
        case COM_MISC_BYTES_password: {
            numbytes = 2;
        } break;
        case COM_MISC_BYTES_vpSac2: {
            numbytes = 1;
            mParam->setValue(PARAM_dirControl, mSrc, uint32_t(buf[0]));
        } break;
        case COM_MISC_BYTES_MAX: break;
    }

    return numbytes;
}

uint16_t TAvantPi::comGetPrmDisableGet(TAvantPi::comPrmDisableBytes_t pos, const uint8_t *buf, uint16_t len)
{
    uint16_t numbytes = len;

    switch(pos) {
        case COM_PRM_DISABLE_BYTES_prmDisable: {
            numbytes = 1;
            mParam->setValue(PARAM_blkComPrmAll, mSrc, uint32_t(buf[0]));
        } break;

        case COM_PRM_DISABLE_BYTES_MAX: break;
    }

    return numbytes;
}

//
bool TAvantPi::comGetTime()
{
    bool ok = false;
    uint16_t pos = POS_DATA;
    uint32_t value = 0;

    if (mBuf[POS_DATA_LEN] >= 6) {
        ok = true;
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

//
bool BVP::TAvantPi::vWriteAvant()
{
    static uint8_t start = 0xFF;
    bool ok = false;

    while(!ok) {
        start++;
        switch(start) {
            case 1: {
                ok = writeComControl();
            } break;
            case 2: {
                ok = writeComMisc();
            } break;
            case 3: {
                ok = writeComPrmDisable();
            } break;
            default: {
                if (!ringComArray.isEmpty()) {
                    setCom(ringComArray.get());
                    ok = true;
                    start = 0;
                } else {
                    start = 0xFF;
                }
            }
        }

        // заглушка на случай если команда не может быть сформирована
        if (start == 0xFF) {
            start = 0;
            break;
        }
    }

    return ok;
}

//
bool TAvantPi::vReadAvant()
{
    bool ok = false;


    comAvant_t com = static_cast<comAvant_t> (mBuf[POS_COM]);
    comAvantMaskGroup_t group = comAvantMaskGroup_t(mBuf[POS_COM] &
                                                    COM_AVANT_MASK_GROUP_mask);

    switch(static_cast<comAvant_t> (com & ~COM_AVANT_MASK_GROUP_mask)) {
        case COM_AVANT_getError: {
            ok = comGetError();
        } break;
        case COM_AVANT_getTime: {
            ok = comGetTime();
        } break;
        case COM_AVANT_getMisc: {
            ok = comGetMisc(group);
        } break;
        case COM_AVANT_getPrmDisable: {
            ok = comGetPrmDisable(group);
        } break;
    }

    return ok;
}

} // namespace BVP

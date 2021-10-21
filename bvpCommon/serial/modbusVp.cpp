#include "modbusVp.h"
#include "global.hpp"


namespace BVP
{

/// Таблица CRC для полинома 0x8005 (LSB first)
static const uint16_t crc_ibm_table[256] = {
    0x0000, 0xc0c1, 0xc181, 0x0140, 0xc301, 0x03c0, 0x0280, 0xc241, 0xc601, 0x06c0, 0x0780, 0xc741,
    0x0500, 0xc5c1, 0xc481, 0x0440, 0xcc01, 0x0cc0, 0x0d80, 0xcd41, 0x0f00, 0xcfc1, 0xce81, 0x0e40,
    0x0a00, 0xcac1, 0xcb81, 0x0b40, 0xc901, 0x09c0, 0x0880, 0xc841, 0xd801, 0x18c0, 0x1980, 0xd941,
    0x1b00, 0xdbc1, 0xda81, 0x1a40, 0x1e00, 0xdec1, 0xdf81, 0x1f40, 0xdd01, 0x1dc0, 0x1c80, 0xdc41,
    0x1400, 0xd4c1, 0xd581, 0x1540, 0xd701, 0x17c0, 0x1680, 0xd641, 0xd201, 0x12c0, 0x1380, 0xd341,
    0x1100, 0xd1c1, 0xd081, 0x1040, 0xf001, 0x30c0, 0x3180, 0xf141, 0x3300, 0xf3c1, 0xf281, 0x3240,
    0x3600, 0xf6c1, 0xf781, 0x3740, 0xf501, 0x35c0, 0x3480, 0xf441, 0x3c00, 0xfcc1, 0xfd81, 0x3d40,
    0xff01, 0x3fc0, 0x3e80, 0xfe41, 0xfa01, 0x3ac0, 0x3b80, 0xfb41, 0x3900, 0xf9c1, 0xf881, 0x3840,
    0x2800, 0xe8c1, 0xe981, 0x2940, 0xeb01, 0x2bc0, 0x2a80, 0xea41, 0xee01, 0x2ec0, 0x2f80, 0xef41,
    0x2d00, 0xedc1, 0xec81, 0x2c40, 0xe401, 0x24c0, 0x2580, 0xe541, 0x2700, 0xe7c1, 0xe681, 0x2640,
    0x2200, 0xe2c1, 0xe381, 0x2340, 0xe101, 0x21c0, 0x2080, 0xe041, 0xa001, 0x60c0, 0x6180, 0xa141,
    0x6300, 0xa3c1, 0xa281, 0x6240, 0x6600, 0xa6c1, 0xa781, 0x6740, 0xa501, 0x65c0, 0x6480, 0xa441,
    0x6c00, 0xacc1, 0xad81, 0x6d40, 0xaf01, 0x6fc0, 0x6e80, 0xae41, 0xaa01, 0x6ac0, 0x6b80, 0xab41,
    0x6900, 0xa9c1, 0xa881, 0x6840, 0x7800, 0xb8c1, 0xb981, 0x7940, 0xbb01, 0x7bc0, 0x7a80, 0xba41,
    0xbe01, 0x7ec0, 0x7f80, 0xbf41, 0x7d00, 0xbdc1, 0xbc81, 0x7c40, 0xb401, 0x74c0, 0x7580, 0xb541,
    0x7700, 0xb7c1, 0xb681, 0x7640, 0x7200, 0xb2c1, 0xb381, 0x7340, 0xb101, 0x71c0, 0x7080, 0xb041,
    0x5000, 0x90c1, 0x9181, 0x5140, 0x9301, 0x53c0, 0x5280, 0x9241, 0x9601, 0x56c0, 0x5780, 0x9741,
    0x5500, 0x95c1, 0x9481, 0x5440, 0x9c01, 0x5cc0, 0x5d80, 0x9d41, 0x5f00, 0x9fc1, 0x9e81, 0x5e40,
    0x5a00, 0x9ac1, 0x9b81, 0x5b40, 0x9901, 0x59c0, 0x5880, 0x9841, 0x8801, 0x48c0, 0x4980, 0x8941,
    0x4b00, 0x8bc1, 0x8a81, 0x4a40, 0x4e00, 0x8ec1, 0x8f81, 0x4f40, 0x8d01, 0x4dc0, 0x4c80, 0x8c41,
    0x4400, 0x84c1, 0x8581, 0x4540, 0x8701, 0x47c0, 0x4680, 0x8641, 0x8201, 0x42c0, 0x4380, 0x8341,
    0x4100, 0x81c1, 0x8081, 0x4040,
};


//
TModbusVp::TModbusVp(regime_t regime) :
    TSerialProtocol(regime),
    mSrc(SRC_MAX),
    mState(STATE_disable),
    mTimeToCompleteUs(0),
    mTimeToErrorUs(0),
    cntLostMessage(kMaxLostMessage)
{
    Q_ASSERT(regime == REGIME_master);
}

//
TModbusVp::~TModbusVp()
{
    setNetAddress(255);
}

//
bool TModbusVp::setEnable(bool enable)
{
    Q_ASSERT(mSrc != SRC_MAX);
    Q_ASSERT(mParam != nullptr);
    Q_ASSERT(mBuf != nullptr);
    Q_ASSERT(mSize != 0);

    if (enable)
    {
        mState  = STATE_idle;
        mLen    = 0;
        mTimeUs = 0;
    }
    else
    {
        mState         = STATE_disable;
        cntLostMessage = kMaxLostMessage;
    }

    return mState != STATE_disable;
}

//
bool TModbusVp::isEnable() const
{
    return mState != STATE_disable;
}

//
bool TModbusVp::read()
{
    bool isread = false;

    if (mState == STATE_procReply)
    {
        bool     ok       = true;
        uint16_t len      = mLen;
        uint16_t position = 0;

        //    QString message;
        //    for(uint8_t i = 0; i < len; i++) {
        //      message += QString("%1 ").arg(mBuf[i], 2, 16, QLatin1Char('0'));
        //    }
        //    qDebug() << message;

        position += checkReadMsg(&mBuf[position], len, ok);

        if (ok)
        {
            switch (static_cast<com_t>(mBuf[position++]))
            {
            case COM_readHoldingRegs:
                {
                    position +=
                        getReadReg(&mBuf[position], len, REG_READ_MIN, REG_READ_MAX - 1, ok);
                }
                break;
            case COM_writeMultRegs:
                {
                    // TODO добавить проверку сообщения.
                }
                break;
            case COM_readWriteRegs:
                {
                    position +=
                        getReadReg(&mBuf[position], len, REG_READ_MIN, REG_READ_MAX - 1, ok);
                }
                break;
            }
            cntLostMessage = 0;
        }

        if (!ok)
        {
            incLostMessageCounter();
        }

        isread = ok;
        mState = STATE_idle;
    }

    return isread;
}

//
void TModbusVp::readError()
{
    if (mState == STATE_procReply)
    {
        mState  = STATE_errorReply;
        mTimeUs = 0;
    }
}

//
bool TModbusVp::write()
{
    if (mState == STATE_idle)
    {
        bool     ok  = true;
        uint16_t len = 0;
        mBuf[len++]  = mNetAddress;

        com_t com   = COM_readWriteRegs;
        mBuf[len++] = com;
        switch (com)
        {
        case COM_readHoldingRegs:
            {
                len += addReadRegMsg(&mBuf[len], REG_READ_MIN, REG_READ_MAX - 1, ok);
            }
            break;
        case COM_writeMultRegs:
            {
                len += addReadRegMsg(&mBuf[len], REG_READ_MIN, REG_READ_MAX - 1, ok);
            }
            break;
        case COM_readWriteRegs:
            {
                len += addReadRegMsg(&mBuf[len], REG_READ_MIN, REG_READ_MAX - 1, ok);
                len += addWriteRegMsg(&mBuf[len], REG_WRITE_MIN, REG_WRITE_MAX - 1, ok);
            }
            break;
        }

        Q_ASSERT(ok);

        if (ok)
        {
            uint16_t crc = calcCRC(mBuf, len);
            mBuf[len++]  = static_cast<uint8_t>(crc);
            mBuf[len++]  = static_cast<uint8_t>(crc >> 8);

            Q_ASSERT(len < kMaxSizeFrameRtu);

            if (len > 0)
            {
                mLen    = len;
                mTimeUs = 0;
                mState  = STATE_reqSend;
            }
        }
    }

    return mState == STATE_reqSend;
}

//
uint16_t TModbusVp::pop(uint8_t *data[])
{
    uint16_t len = 0;
    *data        = &mBuf[0];

    Q_ASSERT(mState == STATE_reqSend);

    if (mState == STATE_reqSend)
    {
        len    = mLen;
        mState = STATE_waitSendFinished;
    }

    return len;
}

//
void TModbusVp::sendFinished()
{
    // FIXME При работе с виртуальными портаи периодически вылетает ошибка!
    //    Q_ASSERT(mState == STATE_waitSendFinished || mState == STATE_disable);

    if (mState == STATE_waitSendFinished)
    {
        mLen    = 0;
        mPos    = 0;
        mTimeUs = 0;
        mState  = STATE_waitForReply;
    }
    else
    {
        mState = STATE_idle;
    }
}

//
bool TModbusVp::vPush(uint8_t byte)
{
    uint16_t pos = 0;

    if (mState == STATE_waitForReply)
    {
        pos = mPos;

        if ((pos == 0) || ((pos < mSize) && (mTimeUs < mTimeToErrorUs)))
        {
            mBuf[pos++] = byte;
            mTimeUs     = 0;
        }
        else
        {
            mState = STATE_errorReply;
            pos    = 0;
        }

        mPos = pos;
    }

    if (mState == STATE_errorReply)
    {
        mTimeUs = 0;
    }

    return pos != 0;
}

//
bool TModbusVp::setNetAddress(uint16_t address)
{
    if ((address > 0) && (address <= 247))
    {
        *(const_cast<uint8_t *>(&mNetAddress)) = static_cast<uint8_t>(address);
    }

    return mNetAddress == address;
}

//
bool TModbusVp::vSetup(uint32_t baudrate, bool parity, uint8_t stopbits)
{
    UNUSED(baudrate);
    UNUSED(parity);
    UNUSED(stopbits);

    // TODO Проверить времена в железе. На ПК от них толка нет.
    *(const_cast<uint32_t *>(&mTimeToCompleteUs)) = (mTimeOneByteUs * 7) / 2;  // 3.5 char
    *(const_cast<uint32_t *>(&mTimeToErrorUs))    = (mTimeOneByteUs * 3) / 2;  // 1.5 char

#if defined(QT_CORE_LIB)
    // На ПК таймауты не выдерживаются. Просто так не задать меньше 1 мс.
    *(const_cast<uint32_t *>(&mTimeToCompleteUs)) = mTimeToCompleteUs * 10;
    *(const_cast<uint32_t *>(&mTimeToErrorUs))    = mTimeToErrorUs * 10;
#endif

    return mTimeOneByteUs > 0;
}

//
void TModbusVp::setID(uint32_t id)
{
    Q_ASSERT(id < SRC_MAX);

    *(const_cast<src_t *>(&mSrc)) = static_cast<src_t>(id);
}

//
uint32_t TModbusVp::getID() const
{
    return static_cast<uint32_t>(mSrc);
}


//
void TModbusVp::vTick()
{
    if (mState == STATE_disable)
    {
        return;
    }

    if (mTimeUs < kMaxTimeToResponseUs)
    {
        mTimeUs += mTimeTickUs;
    }

    if (mTimeUs >= kMaxTimeToResponseUs)
    {
        mState = STATE_idle;
        incLostMessageCounter();
    }

    if (mState == STATE_waitForReply)
    {
        if ((mPos != 0) && (mTimeUs > mTimeToCompleteUs))
        {
            mLen   = mPos;
            mState = STATE_procReply;
        }
    }

    if (mState == STATE_errorReply)
    {
        if (mTimeUs >= mTimeToCompleteUs)
        {
            mState = STATE_idle;
            incLostMessageCounter();
        }
    }
}

//
bool TModbusVp::isConnection() const
{
    return (mState != STATE_disable) && (cntLostMessage < kMaxLostMessage);
}

//
uint16_t TModbusVp::getSwitchLed(param_t hi, param_t low, switchOff_t sw) const
{
    uint16_t result = 0;

    result = getSwitchLed(hi, sw);
    result = static_cast<uint16_t>(result << 8) + getSwitchLed(low, sw);

    return result;
}

//
void TModbusVp::incLostMessageCounter()
{
    if (cntLostMessage < kMaxLostMessage)
    {
        cntLostMessage++;
    }
}

//
uint16_t TModbusVp::addReadRegMsg(uint8_t buf[], uint16_t min, uint16_t max, bool &ok)
{
    uint16_t nbytes = 0;

    Q_ASSERT(min >= REG_READ_MIN);
    Q_ASSERT(max < REG_READ_MAX);
    Q_ASSERT(ok);

    if (ok)
    {
        Q_ASSERT(min > 0);
        uint16_t startaddress = min - 1;
        buf[nbytes++]         = static_cast<uint8_t>(startaddress >> 8);
        buf[nbytes++]         = static_cast<uint8_t>(startaddress);

        Q_ASSERT(max >= min);
        uint8_t quantity = static_cast<uint8_t>(max - min + 1);
        buf[nbytes++]    = static_cast<uint8_t>(quantity >> 8);
        buf[nbytes++]    = static_cast<uint8_t>(quantity);
    }

    Q_ASSERT(ok);
    return nbytes;
}

//
uint16_t TModbusVp::addWriteRegMsg(uint8_t buf[], uint16_t min, uint16_t max, bool &ok)
{
    uint16_t nbytes = 0;

    Q_ASSERT(min >= REG_WRITE_MIN);
    Q_ASSERT(max < REG_WRITE_MAX);
    Q_ASSERT(ok);

    if (ok)
    {
        if ((min < REG_WRITE_MIN) || (max >= REG_WRITE_MAX))
        {
            ok = false;
        }
        else
        {
            uint16_t startaddress = min - 1;
            buf[nbytes++]         = static_cast<uint8_t>(startaddress >> 8);
            buf[nbytes++]         = static_cast<uint8_t>(startaddress);

            uint8_t quantity = static_cast<uint8_t>(max - min + 1);
            buf[nbytes++]    = static_cast<uint8_t>(quantity >> 8);
            buf[nbytes++]    = static_cast<uint8_t>(quantity);
            buf[nbytes++]    = static_cast<uint8_t>(quantity * 2);

            for (uint8_t i = 0; i < quantity; i++)
            {
                uint16_t value = getWriteRegMsgData(min + i, ok);
                Q_ASSERT(ok);
                buf[nbytes++] = static_cast<uint8_t>(value >> 8);
                buf[nbytes++] = static_cast<uint8_t>(value);
            }
        }
    }

    Q_ASSERT(ok);
    return nbytes;
}

//
uint16_t TModbusVp::checkReadMsg(const uint8_t buf[], uint16_t &len, bool &ok)
{
    uint16_t nbytes = 0;

    if (ok)
    {
        if (buf[nbytes++] != mNetAddress)
        {
            ok = false;
        }

        uint16_t crcpkg = static_cast<uint16_t>(buf[--len] << 8);
        crcpkg += buf[--len];

        if (crcpkg != calcCRC(buf, len))
        {
            ok = false;
        }
    }

    return nbytes;
}

//
uint16_t TModbusVp::getWriteRegMsgData(uint16_t number, bool &ok) const
{
    uint16_t value  = 0;
    uint16_t tvalue = 0;

    Q_ASSERT(number >= REG_WRITE_MIN);
    Q_ASSERT(number < REG_WRITE_MAX);
    Q_ASSERT(ok);

    if (ok)
    {
        if ((number < REG_WRITE_MIN) || (number >= REG_WRITE_MAX))
        {
            ok = false;
        }
    }

    if (ok)
    {
        param_t param = PARAM_MAX;

        Q_ASSERT((number >= REG_WRITE_MIN) && (number < REG_WRITE_MAX));

        switch (static_cast<regWrite_t>(number))
        {
        case REG_WRITE_enSanSbSac:
            {
                param  = PARAM_blkComPrmAll;
                tvalue = mParam->getValue(param, mSrc, ok) == DISABLE_PRM_disable
                             ? 0
                             : (uint16_t(1) << VP_BTN_CONTROL_sac1);
                if (ok)
                {
                    value |= tvalue;
                }
                param  = PARAM_dirControl;
                tvalue = mParam->getValue(param, mSrc, ok) == DIR_CONTROL_remote
                             ? 0
                             : (uint16_t(1) << VP_BTN_CONTROL_sac2);
                if (ok)
                {
                    value |= tvalue;
                }
                param = PARAM_blkComPrmDir;
                tvalue |= (~mParam->getValue(param, mSrc, ok)) << 8;
                if (ok)
                {
                    value |= tvalue;
                }
            }
            break;

        case REG_WRITE_enLed16to01:
            {
                // FIXME Для Казань MPLSTP сделана блокировка команд передатчика
                value = getSwitchLed(PARAM_comPrdBlk16to09, PARAM_comPrdBlk08to01, ON_OFF_off);
                // value = getSwitchLed(PARAM_comPrmBlk16to09,
                //                     PARAM_comPrmBlk08to01, ON_OFF_off);
            }
            break;

        case REG_WRITE_dsLed16to01:
            {
                // FIXME Для Казань MPLSTP сделана блокировка команд передатчика
                value = getSwitchLed(PARAM_comPrdBlk16to09, PARAM_comPrdBlk08to01, ON_OFF_on);
                // value = getSwitchLed(PARAM_comPrmBlk16to09,
                //                      PARAM_comPrmBlk08to01, ON_OFF_on);
            }
            break;

        case REG_WRITE_enLed32to17:
            {   // DOWN}
                //                value = getSwitchLed(PARAM_comPrmBlk32to25,
                //                                     PARAM_comPrmBlk24to17, ON_OFF_off);
                // FIXME Для Казань MPLSTP сделана блокировка команд передатчика
                value = getSwitchLed(PARAM_comPrmBlk16to09, PARAM_comPrmBlk08to01, ON_OFF_off);
            }
            break;

        case REG_WRITE_dsLed32to17:
            {
                //                value = getSwitchLed(PARAM_comPrmBlk32to25,
                //                                     PARAM_comPrmBlk24to17, ON_OFF_on);
                // FIXME Для Казань MPLSTP сделана блокировка команд передатчика
                value = getSwitchLed(PARAM_comPrmBlk16to09, PARAM_comPrmBlk08to01, ON_OFF_on);
            }
            break;

        case REG_WRITE_enLed48to33:  // DOWN
        case REG_WRITE_dsLed48to33:
            {
                param  = PARAM_blkComPrm64to33;
                tvalue = static_cast<uint16_t>(mParam->getValue(param, mSrc, ok));
                if (ok)
                {
                    value = tvalue;
                    if (number == REG_WRITE_enLed48to33)
                    {
                        value = ~value;
                    }
                }
            }
            break;

        case REG_WRITE_enLed64to49:
        case REG_WRITE_dsLed64to49:
            {
                param  = PARAM_blkComPrm64to33;
                tvalue = static_cast<uint16_t>(~mParam->getValue(param, mSrc, ok) >> 16);
                if (ok)
                {
                    value = tvalue;
                    if (number == REG_WRITE_enLed64to49)
                    {
                        value = ~value;
                    }
                }
            }
            break;

        case REG_WRITE_dsSanSbSac:
            {
                param  = PARAM_blkComPrmAll;
                tvalue = mParam->getValue(param, mSrc, ok) == DISABLE_PRM_disable
                             ? (uint16_t(1) << VP_BTN_CONTROL_sac1)
                             : 0;
                if (ok)
                {
                    value |= tvalue;
                }
                param = PARAM_dirControl;
                tvalue |= mParam->getValue(param, mSrc, ok) == DIR_CONTROL_remote
                              ? (uint16_t(1) << VP_BTN_CONTROL_sac2)
                              : 0;
                if (ok)
                {
                    value |= tvalue;
                }
                param = PARAM_blkComPrmDir;
                value |= (mParam->getValue(param, mSrc, ok)) << 8;
                if (ok)
                {
                    value |= tvalue;
                }
            }
            break;

        case REG_WRITE_MAX: break;
        }

        ok = true;
    }

    Q_ASSERT(ok);
    return value;
}

//
uint16_t
TModbusVp::getReadReg(const uint8_t buf[], uint16_t &len, uint16_t min, uint16_t max, bool &ok)
{
    uint16_t nbytes   = 0;
    uint8_t  quantity = 0;

    if (ok)
    {
        quantity = static_cast<uint8_t>(max - min + 1);
        if ((buf[nbytes++] != 2 * quantity) || (len < 2 * quantity))
        {
            ok = false;
        }
    }

    if (ok)
    {
        for (uint8_t i = 0; i < quantity; i++)
        {
            nbytes += getReadRegMsgData(&buf[nbytes], min++, ok);
            if (!ok)
            {
                break;
            }
        }
    }

    len -= nbytes;
    return nbytes;
}

//
uint16_t TModbusVp::getReadRegMsgData(const uint8_t buf[], uint16_t number, bool &ok)
{
    uint16_t nbytes = 0;

    if (ok)
    {
        if ((number < REG_READ_MIN) || (number >= REG_READ_MAX))
        {
            ok = false;
        }
    }

    if (ok)
    {
        uint32_t val32 = 0;
        uint16_t value = static_cast<uint16_t>(buf[nbytes++]);
        value          = static_cast<uint16_t>((value << 8) + buf[nbytes++]);

        param_t param = PARAM_MAX;
        switch (static_cast<regRead_t>(number))
        {
        case REG_READ_sanSbSac:
            {
                param = PARAM_vpBtnSAnSbSac;
                val32 = static_cast<uint32_t>(value);
                hdlrButtonSbSacSan(static_cast<uint32_t>(value));
                hdlrButtonSa(PARAM_blkComPrmDir, val32, PARAM_vpBtnSAnSbSac, 2);
            }
            break;
        case REG_READ_sa16to01:
            {
                param = PARAM_vpBtnSA32to01;
                val32 = static_cast<uint32_t>(value);
                val32 += (mParam->getValue(param, mSrc, ok) & 0xFFFF0000);
                // FIXME Для Казань MPLSTP сделана блокировка команд передатчика
                // hdlrButtonSa(PARAM_comPrmBlk08to01, val32, PARAM_vpBtnSA32to01, 1);
                // hdlrButtonSa(PARAM_comPrmBlk16to09, val32, PARAM_vpBtnSA32to01, 2);
                hdlrButtonSa(PARAM_comPrdBlk08to01, val32, PARAM_vpBtnSA32to01, 1);
                hdlrButtonSa(PARAM_comPrdBlk16to09, val32, PARAM_vpBtnSA32to01, 2);
            }
            break;
        case REG_READ_sa32to17:
            {
                param = PARAM_vpBtnSA32to01;
                val32 = (static_cast<uint32_t>(value)) << 16;
                val32 += (mParam->getValue(param, mSrc, ok) & 0x0000FFFF);
                // FIXME Для Казань MPLSTP сделана блокировка команд передатчика
                // hdlrButtonSa(PARAM_comPrmBlk24to17, val32, PARAM_vpBtnSA32to01, 3);
                // hdlrButtonSa(PARAM_comPrmBlk32to25, val32, PARAM_vpBtnSA32to01, 4);
                hdlrButtonSa(PARAM_comPrmBlk08to01, val32, PARAM_vpBtnSA32to01, 3);
                hdlrButtonSa(PARAM_comPrmBlk16to09, val32, PARAM_vpBtnSA32to01, 4);
            }
            break;
        case REG_READ_sa48to33:
            {
                param = PARAM_vpBtnSA64to33;
                val32 = static_cast<uint32_t>(value);
                val32 += (mParam->getValue(param, mSrc, ok) & 0xFFFF0000);
            }
            break;
        case REG_READ_sa64to49:
            {
                param = PARAM_vpBtnSA64to33;
                val32 = (static_cast<uint32_t>(value)) << 16;
                val32 += (mParam->getValue(param, mSrc, ok) & 0x0000FFFF);
            }
            break;
        case REG_READ_version:
            {
                param = PARAM_versionVp;
                val32 = static_cast<uint32_t>(value);
            }
            break;
        case REG_READ_MAX: break;
        }

        if (param != PARAM_MAX)
        {
            mParam->setValue(param, mSrc, val32);
        }

        ok = true;
    }

    return nbytes;
}

//
uint16_t TModbusVp::calcCRC(const uint8_t buf[], size_t len, uint16_t crc)
{
    for (size_t i = 0; i < len; i++)
    {
        uint8_t lut = (crc ^ buf[i]) & 0xFF;
        crc         = (crc >> 8) ^ crc_ibm_table[lut];
    }

    return crc;
}

//
void TModbusVp::hdlrButtonSbSacSan(uint32_t value)
{
    bool           ok   = true;
    const uint32_t mask = (uint32_t(1) << VP_BTN_CONTROL_MAX) - 1;
    uint32_t       last = mParam->getValue(PARAM_vpBtnSAnSbSac, mSrc, ok);

    if (!ok)
    {
        last = 0x00000000;
    }

    if (value > 0)
    {
        uint32_t pressed = ((value ^ last) & value) & mask;
        if (pressed)
        {
            for (uint8_t i = 0; i < VP_BTN_CONTROL_MAX; i++)
            {
                if (pressed & (uint32_t(1) << i))
                {
                    hdlrButtonSbSacSan(static_cast<vpBtnControl_t>(i));
                }
            }
        }
    }
}

void TModbusVp::hdlrButtonSbSacSan(vpBtnControl_t btn)
{
    bool     ok     = true;
    param_t  param  = PARAM_dirControl;
    uint32_t value  = mParam->getValue(param, mSrc, ok);
    bool     change = ok && (value == DIR_CONTROL_local);

    switch (btn)
    {
    case VP_BTN_CONTROL_sb:
        {
            param = PARAM_alarmRstCtrl;
            mParam->setValue(param, mSrc, ALARM_RST_CTRL_pressed);
        }
        break;
    case VP_BTN_CONTROL_sac1:
        {
            if (change)
            {
                param      = PARAM_blkComPrmAll;
                uint32_t v = mParam->getValue(param, mSrc, ok);
                v          = ok ? (v + 1) % DISABLE_PRM_MAX : DISABLE_PRM_enable;
                mParam->setValue(param, mSrc, v);
            }
        }
        break;
    case VP_BTN_CONTROL_sac2:
        {
            param      = PARAM_dirControl;
            uint32_t v = mParam->getValue(param, mSrc, ok);
            v          = ok ? (v + 1) % DIR_CONTROL_MAX : DIR_CONTROL_local;
            mParam->setValue(param, mSrc, v);
        }
        break;
    case VP_BTN_CONTROL_san1:
        {
            // TODO
        }
        break;
    case VP_BTN_CONTROL_san2:
        {
            // TODO
        }
        break;
    case VP_BTN_CONTROL_san3:
        {
            // TODO
        }
        break;
    case VP_BTN_CONTROL_san4:
        {
            // TODO
        }
        break;
    case VP_BTN_CONTROL_san5:
        {
            // TODO
        }
        break;
    case VP_BTN_CONTROL_san6:
        {
            // TODO
        }
        break;
    case VP_BTN_CONTROL_san7:
        {
            // TODO
        }
        break;
    case VP_BTN_CONTROL_san8:
        {
            // TODO
        }
        break;
    case VP_BTN_CONTROL_MAX:
        {
            // TODO
        }
        break;
    }
}

//
void TModbusVp::hdlrButtonSa(param_t param, uint32_t cvalue, param_t lparam, uint8_t num)
{
    bool           ok    = true;
    const uint32_t mask  = 0x00FF;
    uint32_t       value = mParam->getValue(PARAM_dirControl, mSrc, ok);

    // Обработка переключателя вдетеся только при локальном управлении
    bool change = ok && (value == DIR_CONTROL_local);

    // Переключатели приемника обрабатываются, если нет общей блокировки
    if (change && isParamComPrmBlk(param))
    {
        change = !isBlockPrm();
    }

    if (change)
    {
        // NOTE Еще не сохраненное предыдущее значение потеряется
        uint32_t lvalue = mParam->getValue(lparam, mSrc, ok);
        if (ok)
        {
            Q_ASSERT((num >= 1) && (num <= 4));
            if ((num >= 1) && (num <= 4))
            {
                uint32_t value = mParam->getValue(param, mSrc, ok) & mask;

                if (ok)
                {
                    // определение нового нажатия на переключатель
                    uint32_t pressed = ((cvalue ^ lvalue) & cvalue);
                    pressed          = (pressed >> 8 * (num - 1)) & mask;
                    if (pressed)
                    {
                        mParam->setValue(param, mSrc, value ^ pressed);
                    }
                }
            }
        }
    }
}

//
uint8_t TModbusVp::getSwitchLed(param_t param, switchOff_t sw) const
{
    bool    ok     = false;
    uint8_t result = 0x00;

    // ограничение на список параметров, чтобы небыло доступа к остальным
    if ((param >= PARAM_comPrmBlk08to01) && (param <= PARAM_blkComPrd64to33))
    {
        if (isParamComPrmBlk(param) && isBlockPrm())
        {
            result = 0xFF;
            ok     = true;
        }
        else
        {
            result = static_cast<uint8_t>(mParam->getValue(param, mSrc, ok));
        }
    }

    return !ok ? 0x00 : ((sw == ON_OFF_off) ? ~result : result);
}

//
bool TModbusVp::isBlockPrm() const
{
    bool     ok    = true;
    uint32_t value = mParam->getValue(PARAM_blkComPrmAll, mSrc, ok);

    return ok && (value == DISABLE_PRM_disable);
}

//
bool TModbusVp::isParamComPrmBlk(param_t param) const
{
    return (param >= PARAM_comPrmBlk08to01) && (param <= PARAM_blkComPrm64to33);
}

}  // namespace BVP

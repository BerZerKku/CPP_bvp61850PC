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
    mTimeOneByteUs(0)
{
    // FIXME Сброс всех источников происходит в каждом созданной объекте!
    for(uint8_t i = 0; i < src_t::SRC_MAX; i++) {
        transferReset(src_t(i));
    }

    *(const_cast<TParam**> (&mParam)) = TParam::getInstance();
}

//
TSerialProtocol::~TSerialProtocol()
{

}

//
bool TSerialProtocol::push(uint8_t byte)
{
    if (mPos == 0) {
        mTimeReadStart = 0;
    }

    return vPush(byte);
}

//
bool TSerialProtocol::setTimeTick(uint32_t ticktimeus)
{
    *(const_cast<uint32_t*> (&mTimeTickUs)) = ticktimeus;

    Q_ASSERT(mTimeTickUs > 0);

    return mTimeTickUs != 0;
}

//
bool TSerialProtocol::setSrcId(src_t id)
{
    Q_ASSERT(id < SRC_MAX);

    if (id >= SRC_MAX) {
        QCRITICAL("Wrong id value " << id);
        id = SRC_MAX;
    }

    *(const_cast<src_t*> (&mSrcId)) = id;

    return (id < SRC_MAX);
}

//
src_t TSerialProtocol::getSrcId() const
{
    return mSrcId;
}

//
bool TSerialProtocol::setup(uint32_t baudrate, bool parity, uint8_t stopbits)
{
    uint8_t nbites = 1 + 8 + stopbits + (parity ? 1 : 0);

    Q_ASSERT(baudrate > 0);
    Q_ASSERT((stopbits == 1) || (stopbits == 2));

    if (baudrate > 0) {
        *(const_cast<uint32_t*> (&mTimeOneByteUs)) =
                static_cast<uint32_t> ((1000000UL / baudrate) * nbites);
    }

    return vSetup(baudrate, parity, stopbits);
}

//
void TSerialProtocol::tick()
{
    mTimeReadStart = (mTimeReadStart < kMaxTimeFromReadFirstByte - mTimeTickUs) ?
                         mTimeReadStart + mTimeTickUs : kMaxTimeFromReadFirstByte;

    vTick();
}

//
void TSerialProtocol::setBuffer(uint8_t buf[], uint16_t size)
{
    *(const_cast<uint16_t*> (&mSize)) = size;
    *(const_cast<uint8_t**> (&mBuf)) = buf;
}

//
void TSerialProtocol::transferReset()
{
    // FIXME Получается что mId всегда должен быть источником сообщений!
    transferReset(src_t(mSrcId));
}

//
void TSerialProtocol::transferReset(src_t src)
{
    // TODO Добавить ID типов протоколов, чтобы не передавать туда, куда не надо!

    if (src < src_t::SRC_MAX) {
        transfer_t *t = &mTransfer[mSrcId];

        t->srcData = src_t::SRC_MAX;
        t->req = false;
        t->resp = false;
        t->dataSize = 0;
        t->data = nullptr;
    } else {
        QCRITICAL("Wrong 'src' value " << src);
    }
}

} // namespace BVP

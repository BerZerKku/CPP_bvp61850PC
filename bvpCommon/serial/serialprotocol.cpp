#include "serialprotocol.h"

namespace BVP {

TSerialProtocol::transfer_t TSerialProtocol::mTransfer[src_t::SRC_MAX];

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

    qDebug() << "TSerialProtocol::regime " << regime <<
                "mRegime" << mRegime;

    // FIXME Параметр почему-то не присваивается через список инициализации в конструкторе
    *(const_cast<regime_t*>(&mRegime)) = regime;
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
bool TSerialProtocol::transferTo(src_t dst, const uint8_t *data, uint16_t size)
{
    bool ok = false;

    if (dst < src_t::SRC_MAX) {
        transfer_t *t = &mTransfer[dst];

        t->req = true;
        t->resp = false;
        t->srcData = mSrcId;
        t->data = data;
        t->dataSize = size;
    } else {
        QCRITICAL("Wrong 'dst' value " << dst);
    }

    return ok;
}

bool TSerialProtocol::transferFrom(src_t dst, const uint8_t *data, uint16_t size)
{
    bool ok = false;

    if (dst < src_t::SRC_MAX) {
        transfer_t *t = &mTransfer[dst];

        t->req = false;
        t->resp = true;
        t->srcData = mSrcId;
        t->data = data;
        t->dataSize = size;

        t = &mTransfer[mSrcId];
        t->resp = false;
    } else {
        QCRITICAL("Wrong 'dst' value " << dst);
    }

    return ok;
}

//
bool TSerialProtocol::isTransferReq()
{
    bool req = false;
    const src_t src = mSrcId;

    if (src < src_t::SRC_MAX) {
        transfer_t *t = &mTransfer[src];

        if (t->req) {
            if ((t->dataSize == 0) || (t->data == nullptr)) {
                t->req = false;
            }
            req = t->req;
        }
    } else {
        QCRITICAL("Wrong 'src' value " << src);
    }

    return req;
}

bool TSerialProtocol::isTransferResp()
{
    bool resp = false;
    const src_t src = mSrcId;

    if (src < src_t::SRC_MAX) {
        transfer_t *t = &mTransfer[src];

        resp = t->resp;
    } else {
        QCRITICAL("Wrong 'src' value " << src);
    }

    return resp;
}

//
uint16_t TSerialProtocol::copyTransferDataReq()
{
    uint16_t cnt = 0;
    const src_t src = mSrcId;

    if (src < src_t::SRC_MAX) {
        transfer_t *t = &mTransfer[src];

        if (mSize >= t->dataSize) {
            for(uint16_t i = 0; i < t->dataSize; i++) {
                mBuf[i] = t->data[i];
            }
            cnt = t->dataSize;
        } else {
            QCRITICAL("Buffer size is too small" << src);
        }

        t->req = false;
        t->re
        t->dataSize = 0;
        t->data = nullptr;
    } else {
        QCRITICAL("Wrong 'src' value " << src);
    }

    return cnt;
}

//
void TSerialProtocol::transferReset(src_t src)
{
    // TODO Добавить ID типов протоколов, чтобы не передавать туда, куда не надо!

    if (src < src_t::SRC_MAX) {
        transfer_t *t = &mTransfer[src];

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

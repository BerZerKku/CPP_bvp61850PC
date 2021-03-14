#include "gtest/gtest.h"


#include <cstdio>
#include <iostream>

#define TEST_FRIENDS \
    friend class TModbusVp_Test; \
    FRIEND_TEST(TModbusVp_Test, hdlrButtonSa); \


#include "bvpCommon/serial/modbusVp.h"

using namespace std;

namespace BVP {


class TModbusVp_Test: public ::testing::Test {

public:
    TModbusVp *mModbus;
    TParam *mParam;

    TModbusVp_Test() {
        mParam = TParam::getInstance();
    }

    //
    bool setValue(param_t param, src_t src, uint32_t value) {
        bool check = true;

        bool ok = mParam->setValue(param, src, value);
        check &= ok;
        EXPECT_TRUE(ok);

        uint32_t result = mParam->getValue(param, src, ok);
        check &= ok;
        EXPECT_TRUE(ok);
        if (result != value) {
            EXPECT_EQ(value, result);
            check = false;
        }

        if (!mParam->isSet(param)) {
            EXPECT_TRUE(true);
            check = false;
        }

        return check;
    }

    bool checkValue(param_t param, src_t src, uint32_t value) {
        bool ok = true;

        uint32_t result = mParam->getValue(param, src, ok);
        if (!ok) {
            EXPECT_TRUE(ok);
        }

        if (result != value) {
            EXPECT_EQ(value, result);
            ok = false;
        }

        if (!mParam->isSet(param)) {
            EXPECT_TRUE(true);
            ok = false;
        }

        return ok;
    }

    //
    bool checkValueW(param_t param, uint32_t value) {
        EXPECT_EQ(value,  mParam->getValueW(param));

        return value == mParam->getValueW(param);
    }

protected:

    void SetUp() override {
        mModbus = new BVP::TModbusVp(TSerialProtocol::REGIME_master);
    }

    void TearDown()override {
        delete mModbus;
    }
};

//
TEST_F(TModbusVp_Test, hdlrButtonSa)
{
    ASSERT_TRUE(setValue(PARAM_comPrmBlk08to01, SRC_pi, 0));
    ASSERT_TRUE(setValue(PARAM_comPrmBlk16to09, SRC_pi, 0));
    ASSERT_TRUE(setValue(PARAM_comPrmBlk24to17, SRC_pi, 0));
    ASSERT_TRUE(setValue(PARAM_comPrmBlk32to25, SRC_pi, 0));
    ASSERT_TRUE(setValue(PARAM_vpBtnSA32to01, SRC_vkey, 0));

    mModbus->hdlrButtonSa(PARAM_comPrmBlk08to01, 0x00, PARAM_vpBtnSA32to01, 1);

    ASSERT_TRUE(checkValue(PARAM_comPrmBlk08to01, SRC_pi, 0));
    ASSERT_TRUE(checkValue(PARAM_comPrmBlk16to09, SRC_pi, 0));
    ASSERT_TRUE(checkValue(PARAM_comPrmBlk24to17, SRC_pi, 0));
    ASSERT_TRUE(checkValue(PARAM_comPrmBlk32to25, SRC_pi, 0));
    ASSERT_TRUE(checkValue(PARAM_vpBtnSA32to01, SRC_vkey, 0));

    mModbus->hdlrButtonSa(PARAM_comPrmBlk08to01, 0x01, PARAM_vpBtnSA32to01, 1);

    ASSERT_TRUE(checkValueW(PARAM_comPrmBlk08to01, 0x01));
    ASSERT_TRUE(checkValueW(PARAM_comPrmBlk16to09, 0));
    ASSERT_TRUE(checkValueW(PARAM_comPrmBlk24to17, 0));
    ASSERT_TRUE(checkValueW(PARAM_comPrmBlk32to25, 0));
    ASSERT_TRUE(checkValue(PARAM_vpBtnSA32to01, SRC_vkey, 0));

    mModbus->hdlrButtonSa(PARAM_comPrmBlk08to01, 0x02, PARAM_vpBtnSA32to01, 1);

    ASSERT_TRUE(checkValueW(PARAM_comPrmBlk08to01, 0x3));
    ASSERT_TRUE(checkValueW(PARAM_comPrmBlk16to09, 0));
    ASSERT_TRUE(checkValueW(PARAM_comPrmBlk24to17, 0));
    ASSERT_TRUE(checkValueW(PARAM_comPrmBlk32to25, 0));
    ASSERT_TRUE(checkValue(PARAM_vpBtnSA32to01, SRC_vkey, 0));

    mModbus->hdlrButtonSa(PARAM_comPrmBlk16to09, 0x1300, PARAM_vpBtnSA32to01, 2);

    ASSERT_TRUE(checkValueW(PARAM_comPrmBlk08to01, 0x03));
    ASSERT_TRUE(checkValueW(PARAM_comPrmBlk16to09, 0x13));
    ASSERT_TRUE(checkValueW(PARAM_comPrmBlk24to17, 0));
    ASSERT_TRUE(checkValueW(PARAM_comPrmBlk32to25, 0));
    ASSERT_TRUE(checkValue(PARAM_vpBtnSA32to01, SRC_vkey, 0));

    mModbus->hdlrButtonSa(PARAM_comPrmBlk32to25, 0x88000000, PARAM_vpBtnSA32to01, 4);

    ASSERT_TRUE(checkValueW(PARAM_comPrmBlk08to01, 0x03));
    ASSERT_TRUE(checkValueW(PARAM_comPrmBlk16to09, 0x13));
    ASSERT_TRUE(checkValueW(PARAM_comPrmBlk24to17, 0));
    ASSERT_TRUE(checkValueW(PARAM_comPrmBlk32to25, 0x88));
    ASSERT_TRUE(checkValue(PARAM_vpBtnSA32to01, SRC_vkey, 0));

    // Отсутвие изменения при не нажатых переключателях
    mModbus->hdlrButtonSa(PARAM_comPrmBlk32to25, 0x00000000, PARAM_vpBtnSA32to01, 4);

    ASSERT_TRUE(checkValueW(PARAM_comPrmBlk08to01, 0x03));
    ASSERT_TRUE(checkValueW(PARAM_comPrmBlk16to09, 0x13));
    ASSERT_TRUE(checkValueW(PARAM_comPrmBlk24to17, 0));
    ASSERT_TRUE(checkValueW(PARAM_comPrmBlk32to25, 0x88));
    ASSERT_TRUE(checkValue(PARAM_vpBtnSA32to01, SRC_vkey, 0));

    // Отсутствие изменения при установленных битах в ненужной позиции
    mModbus->hdlrButtonSa(PARAM_comPrmBlk24to17, 0x77000000, PARAM_vpBtnSA32to01, 3);

    ASSERT_TRUE(checkValueW(PARAM_comPrmBlk08to01, 0x03));
    ASSERT_TRUE(checkValueW(PARAM_comPrmBlk16to09, 0x13));
    ASSERT_TRUE(checkValueW(PARAM_comPrmBlk24to17, 0));
    ASSERT_TRUE(checkValueW(PARAM_comPrmBlk32to25, 0x88));
    ASSERT_TRUE(checkValue(PARAM_vpBtnSA32to01, SRC_vkey, 0));

    // Отсутствие изменения при удержании переключателя
    ASSERT_TRUE(setValue(PARAM_vpBtnSA32to01, SRC_vkey, 0xFFFFFFFF));
    mModbus->hdlrButtonSa(PARAM_comPrmBlk32to25, 0xFF000000, PARAM_vpBtnSA32to01, 4);

    ASSERT_TRUE(checkValueW(PARAM_comPrmBlk08to01, 0x03));
    ASSERT_TRUE(checkValueW(PARAM_comPrmBlk16to09, 0x13));
    ASSERT_TRUE(checkValueW(PARAM_comPrmBlk24to17, 0));
    ASSERT_TRUE(checkValueW(PARAM_comPrmBlk32to25, 0x88));
    ASSERT_TRUE(checkValue(PARAM_vpBtnSA32to01, SRC_vkey, 0xFFFFFFFF));
}

} // namespace BVP

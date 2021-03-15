#include "gtest/gtest.h"


#include <cstdio>
#include <iostream>

#define TEST_FRIENDS \
    FRIEND_TEST(TModbusVp_Test, init); \
    FRIEND_TEST(TModbusVp_Test, netAddress); \
    FRIEND_TEST(TModbusVp_Test, setup); \
    FRIEND_TEST(TModbusVp_Test, isBlockPrm); \
    FRIEND_TEST(TModbusVp_Test, isParamComPrmBlk); \
    FRIEND_TEST(TModbusVp_Test, hdlrButtonSa); \
    FRIEND_TEST(TModbusVp_Test, getSwitchLed_One); \
    FRIEND_TEST(TModbusVp_Test, getSwitchLed_Two);

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

protected:

    uint8_t mBuf[256];

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

    void SetUp() override {
        mModbus = new BVP::TModbusVp(TSerialProtocol::REGIME_master);

        mModbus->setBuffer(mBuf, sizeof(mBuf) / sizeof(mBuf[0]));
        mModbus->setID(SRC_vkey);
    }

    void TearDown()override {
        delete mModbus;
    }
};

//
TEST_F(TModbusVp_Test, init)
{
    ASSERT_EQ(mBuf, mModbus->mBuf);
    ASSERT_EQ(sizeof(mBuf) / sizeof(mBuf[0]), mModbus->mSize);
}

//
TEST_F(TModbusVp_Test, id) {
    ASSERT_EQ(SRC_vkey, mModbus->getID());

    for(uint16_t i = 0; i <= (SRC_MAX + 1); i++) {
        mModbus->setID(i);
        ASSERT_EQ(i, mModbus->getID());
    }
}

//
TEST_F(TModbusVp_Test, enable)
{
    ASSERT_FALSE(mModbus->isEnable());

    ASSERT_TRUE(mModbus->setEnable(true));
    ASSERT_TRUE(mModbus->isEnable());

    ASSERT_FALSE(mModbus->setEnable(false));
    ASSERT_FALSE(mModbus->isEnable());
}

//
TEST_F(TModbusVp_Test, netAddress)
{
    uint16_t last = 255;
    ASSERT_EQ(last, mModbus->mNetAddress);

    for(uint16_t address = 0; address <= 1000; address++) {
        bool check = (address > 0) && (address <= 247);
        ASSERT_EQ(check, mModbus->setNetAddress(address))
                << " wrong address is " << address;

        if (check) {
            ASSERT_EQ(address, mModbus->mNetAddress)
                    << " wrong address is " << address;
            last = address;
        } else {
            ASSERT_EQ(last, mModbus->mNetAddress)
                    << " wrong address is " << address;
        }
    }
}

//
TEST_F(TModbusVp_Test, setup)
{
    ASSERT_FALSE(mModbus->setup(0, true, 1));

    // FIXME Сейчас на ПК значения в 10 раз больше, как проверить для железа?!
    // mModbus->mTimeToCompleteUs
    // mModbus->mTimeToErrorUs

    ASSERT_TRUE(mModbus->setup(1000000, false, 1));
    ASSERT_EQ(10, mModbus->mTimeOneByteUs);
    ASSERT_EQ(350, mModbus->mTimeToCompleteUs);
    ASSERT_EQ(150, mModbus->mTimeToErrorUs);

    ASSERT_TRUE(mModbus->setup(9600, true, 2));
    ASSERT_EQ(1248, mModbus->mTimeOneByteUs);
    ASSERT_EQ(43680, mModbus->mTimeToCompleteUs);
    ASSERT_EQ(18720, mModbus->mTimeToErrorUs);

}

//
TEST_F(TModbusVp_Test, isBlockPrm)
{
    // Чтобы добавить/убрать тесты в случае измнения диапазона значений
    ASSERT_EQ(2, DISABLE_PRM_MAX);

    for(uint16_t i = 0; i <= (DISABLE_PRM_MAX + 1); i++) {
        ASSERT_TRUE(setValue(PARAM_blkComPrmAll, SRC_pi, i));
        ASSERT_EQ(i == DISABLE_PRM_disable, mModbus->isBlockPrm())
                << " wrong value is " << i;
    }
}

//
TEST_F(TModbusVp_Test, isParamComPrmBlk)
{
    QVector<param_t> list = {
        PARAM_comPrmBlk08to01,
        PARAM_comPrmBlk16to09,
        PARAM_comPrmBlk24to17,
        PARAM_comPrmBlk32to25,
        PARAM_blkComPrm64to33
    };

    for(uint16_t i = 0; i <= (PARAM_MAX + 1); i++) {
        param_t param = static_cast<param_t> (i);

        ASSERT_EQ(mModbus->isParamComPrmBlk(param), list.contains(param))
                << " wrong param is " << param;
    }
}

//
TEST_F(TModbusVp_Test, hdlrButtonSa)
{
    // Инициализация.
    ASSERT_TRUE(setValue(PARAM_comPrmBlk08to01, SRC_pi, 0));
    ASSERT_TRUE(setValue(PARAM_comPrmBlk16to09, SRC_pi, 0));
    ASSERT_TRUE(setValue(PARAM_comPrmBlk24to17, SRC_pi, 0));
    ASSERT_TRUE(setValue(PARAM_comPrmBlk32to25, SRC_pi, 0));
    ASSERT_TRUE(setValue(PARAM_vpBtnSA32to01, SRC_vkey, 0));
    ASSERT_TRUE(setValue(PARAM_dirControl, SRC_pi, DIR_CONTROL_local));
    ASSERT_TRUE(setValue(PARAM_blkComPrmAll, SRC_pi, DISABLE_PRM_enable));

    // Отсутствие реакции на не нажатые переключатели
    mModbus->hdlrButtonSa(PARAM_comPrmBlk08to01, 0x00, PARAM_vpBtnSA32to01, 1);

    ASSERT_TRUE(checkValue(PARAM_comPrmBlk08to01, SRC_pi, 0));
    ASSERT_TRUE(checkValue(PARAM_comPrmBlk16to09, SRC_pi, 0));
    ASSERT_TRUE(checkValue(PARAM_comPrmBlk24to17, SRC_pi, 0));
    ASSERT_TRUE(checkValue(PARAM_comPrmBlk32to25, SRC_pi, 0));
    ASSERT_TRUE(checkValue(PARAM_vpBtnSA32to01, SRC_vkey, 0));

    // Переключение в первых восьми переключателях
    mModbus->hdlrButtonSa(PARAM_comPrmBlk08to01, 0x01, PARAM_vpBtnSA32to01, 1);

    ASSERT_TRUE(checkValueW(PARAM_comPrmBlk08to01, 0x01));
    ASSERT_TRUE(checkValueW(PARAM_comPrmBlk16to09, 0));
    ASSERT_TRUE(checkValueW(PARAM_comPrmBlk24to17, 0));
    ASSERT_TRUE(checkValueW(PARAM_comPrmBlk32to25, 0));
    ASSERT_TRUE(checkValue(PARAM_vpBtnSA32to01, SRC_vkey, 0));

    // Переключение в первых восьми переключателях
    mModbus->hdlrButtonSa(PARAM_comPrmBlk08to01, 0x02, PARAM_vpBtnSA32to01, 1);

    ASSERT_TRUE(checkValueW(PARAM_comPrmBlk08to01, 0x2));
    ASSERT_TRUE(checkValueW(PARAM_comPrmBlk16to09, 0));
    ASSERT_TRUE(checkValueW(PARAM_comPrmBlk24to17, 0));
    ASSERT_TRUE(checkValueW(PARAM_comPrmBlk32to25, 0));
    ASSERT_TRUE(checkValue(PARAM_vpBtnSA32to01, SRC_vkey, 0));

    // Переключение во вторых восьми переключателях
    mModbus->hdlrButtonSa(PARAM_comPrmBlk16to09, 0x1300, PARAM_vpBtnSA32to01, 2);

    ASSERT_TRUE(checkValueW(PARAM_comPrmBlk08to01, 0x02));
    ASSERT_TRUE(checkValueW(PARAM_comPrmBlk16to09, 0x13));
    ASSERT_TRUE(checkValueW(PARAM_comPrmBlk24to17, 0));
    ASSERT_TRUE(checkValueW(PARAM_comPrmBlk32to25, 0));
    ASSERT_TRUE(checkValue(PARAM_vpBtnSA32to01, SRC_vkey, 0));

    // Переключение в последних восьми переключателях
    mModbus->hdlrButtonSa(PARAM_comPrmBlk32to25, 0x88000000, PARAM_vpBtnSA32to01, 4);

    ASSERT_TRUE(checkValueW(PARAM_comPrmBlk08to01, 0x02));
    ASSERT_TRUE(checkValueW(PARAM_comPrmBlk16to09, 0x13));
    ASSERT_TRUE(checkValueW(PARAM_comPrmBlk24to17, 0));
    ASSERT_TRUE(checkValueW(PARAM_comPrmBlk32to25, 0x88));
    ASSERT_TRUE(checkValue(PARAM_vpBtnSA32to01, SRC_vkey, 0));

    // Отсутвие изменения при не нажатых переключателях
    mModbus->hdlrButtonSa(PARAM_comPrmBlk32to25, 0x00000000, PARAM_vpBtnSA32to01, 4);

    ASSERT_TRUE(checkValueW(PARAM_comPrmBlk08to01, 0x02));
    ASSERT_TRUE(checkValueW(PARAM_comPrmBlk16to09, 0x13));
    ASSERT_TRUE(checkValueW(PARAM_comPrmBlk24to17, 0));
    ASSERT_TRUE(checkValueW(PARAM_comPrmBlk32to25, 0x88));
    ASSERT_TRUE(checkValue(PARAM_vpBtnSA32to01, SRC_vkey, 0));

    // Отсутствие изменения при установленных битах в ненужной позиции
    mModbus->hdlrButtonSa(PARAM_comPrmBlk24to17, 0x77000000, PARAM_vpBtnSA32to01, 3);

    ASSERT_TRUE(checkValueW(PARAM_comPrmBlk08to01, 0x02));
    ASSERT_TRUE(checkValueW(PARAM_comPrmBlk16to09, 0x13));
    ASSERT_TRUE(checkValueW(PARAM_comPrmBlk24to17, 0));
    ASSERT_TRUE(checkValueW(PARAM_comPrmBlk32to25, 0x88));
    ASSERT_TRUE(checkValue(PARAM_vpBtnSA32to01, SRC_vkey, 0));

    // Отсутствие изменения при удержании переключателя
    ASSERT_TRUE(setValue(PARAM_vpBtnSA32to01, SRC_vkey, 0xFFFFFFFF));
    mModbus->hdlrButtonSa(PARAM_comPrmBlk32to25, 0xFF000000, PARAM_vpBtnSA32to01, 4);

    ASSERT_TRUE(checkValueW(PARAM_comPrmBlk08to01, 0x02));
    ASSERT_TRUE(checkValueW(PARAM_comPrmBlk16to09, 0x13));
    ASSERT_TRUE(checkValueW(PARAM_comPrmBlk24to17, 0));
    ASSERT_TRUE(checkValueW(PARAM_comPrmBlk32to25, 0x88));
    ASSERT_TRUE(checkValue(PARAM_vpBtnSA32to01, SRC_vkey, 0xFFFFFFFF));

    // При корректных значениях все работает
    ASSERT_TRUE(setValue(PARAM_vpBtnSA32to01, SRC_vkey, 0x00));
    mModbus->hdlrButtonSa(PARAM_comPrmBlk08to01, 0xF1, PARAM_vpBtnSA32to01, 1);

    ASSERT_TRUE(checkValueW(PARAM_comPrmBlk08to01, 0xF1));
    ASSERT_TRUE(checkValueW(PARAM_comPrmBlk16to09, 0x13));
    ASSERT_TRUE(checkValueW(PARAM_comPrmBlk24to17, 0));
    ASSERT_TRUE(checkValueW(PARAM_comPrmBlk32to25, 0x88));
    ASSERT_TRUE(checkValue(PARAM_vpBtnSA32to01, SRC_vkey, 0));

    // Отсутствие изменения при удаленном управлении
    ASSERT_TRUE(setValue(PARAM_dirControl, SRC_pi, DIR_CONTROL_remote));
    mModbus->hdlrButtonSa(PARAM_comPrmBlk08to01, 0x08, PARAM_vpBtnSA32to01, 1);

    ASSERT_TRUE(checkValueW(PARAM_comPrmBlk08to01, 0xF1));
    ASSERT_TRUE(checkValueW(PARAM_comPrmBlk16to09, 0x13));
    ASSERT_TRUE(checkValueW(PARAM_comPrmBlk24to17, 0));
    ASSERT_TRUE(checkValueW(PARAM_comPrmBlk32to25, 0x88));
    ASSERT_TRUE(checkValue(PARAM_vpBtnSA32to01, SRC_vkey, 0));

    // Отсутствие изменения при не корректном значении управления
    ASSERT_TRUE(setValue(PARAM_dirControl, SRC_pi, DIR_CONTROL_MAX));
    mModbus->hdlrButtonSa(PARAM_comPrmBlk08to01, 0x08, PARAM_vpBtnSA32to01, 1);

    ASSERT_TRUE(checkValueW(PARAM_comPrmBlk08to01, 0xF1));
    ASSERT_TRUE(checkValueW(PARAM_comPrmBlk16to09, 0x13));
    ASSERT_TRUE(checkValueW(PARAM_comPrmBlk24to17, 0));
    ASSERT_TRUE(checkValueW(PARAM_comPrmBlk32to25, 0x88));
    ASSERT_TRUE(checkValue(PARAM_vpBtnSA32to01, SRC_vkey, 0));

    // При корректных значениях все работает
    ASSERT_TRUE(setValue(PARAM_dirControl, SRC_pi, DIR_CONTROL_local));
    mModbus->hdlrButtonSa(PARAM_comPrmBlk08to01, 0x08, PARAM_vpBtnSA32to01, 1);

    ASSERT_TRUE(checkValueW(PARAM_comPrmBlk08to01, 0x08));
    ASSERT_TRUE(checkValueW(PARAM_comPrmBlk16to09, 0x13));
    ASSERT_TRUE(checkValueW(PARAM_comPrmBlk24to17, 0));
    ASSERT_TRUE(checkValueW(PARAM_comPrmBlk32to25, 0x88));
    ASSERT_TRUE(checkValue(PARAM_vpBtnSA32to01, SRC_vkey, 0));

    // Отсутствие изменения при блокировке приемника SAC1
    ASSERT_TRUE(setValue(PARAM_blkComPrmAll, SRC_pi, DISABLE_PRM_disable));
    mModbus->hdlrButtonSa(PARAM_comPrmBlk08to01, 0x01, PARAM_vpBtnSA32to01, 1);

    ASSERT_TRUE(checkValueW(PARAM_comPrmBlk08to01, 0x08));
    ASSERT_TRUE(checkValueW(PARAM_comPrmBlk16to09, 0x13));
    ASSERT_TRUE(checkValueW(PARAM_comPrmBlk24to17, 0));
    ASSERT_TRUE(checkValueW(PARAM_comPrmBlk32to25, 0x88));
    ASSERT_TRUE(checkValue(PARAM_vpBtnSA32to01, SRC_vkey, 0));

    // Наличие изменения команд передатчика при блокировке приемника SAC1
    ASSERT_TRUE(setValue(PARAM_comPrdBlk08to01, SRC_pi, 0));
    mModbus->hdlrButtonSa(PARAM_comPrdBlk08to01, 0x01, PARAM_vpBtnSA32to01, 1);

    ASSERT_TRUE(checkValueW(PARAM_comPrdBlk08to01, 0x01));
    ASSERT_TRUE(checkValue(PARAM_vpBtnSA32to01, SRC_vkey, 0));

    // Изменение при не корректном значении приемника SAC1
    ASSERT_TRUE(setValue(PARAM_blkComPrmAll, SRC_pi, DISABLE_PRM_MAX));
    mModbus->hdlrButtonSa(PARAM_comPrmBlk08to01, 0x01, PARAM_vpBtnSA32to01, 1);

    ASSERT_TRUE(checkValueW(PARAM_comPrmBlk08to01, 0x01));
    ASSERT_TRUE(checkValueW(PARAM_comPrmBlk16to09, 0x13));
    ASSERT_TRUE(checkValueW(PARAM_comPrmBlk24to17, 0));
    ASSERT_TRUE(checkValueW(PARAM_comPrmBlk32to25, 0x88));
    ASSERT_TRUE(checkValue(PARAM_vpBtnSA32to01, SRC_vkey, 0));
}

//
TEST_F(TModbusVp_Test, getSwitchLed_One)
{
    // Сброс параметров
    mParam->reset();

    // Светодиоды не установленных параметров не горят
    uint8_t result8 = 0x00;
    for(uint16_t i = PARAM_comPrmBlk08to01; i <= PARAM_comPrdBlk24to17; i++) {
        param_t param = static_cast<param_t> (i);
        ASSERT_EQ(result8, mModbus->getSwitchLed(param, ON_OFF_off));
        ASSERT_EQ(result8, mModbus->getSwitchLed(param_t(i), ON_OFF_on));
    }

    //
    // Проверка светодиодов при некорректном состоянии блокировки приемника SAC1
    //

    for(uint16_t i = PARAM_comPrmBlk08to01; i <= PARAM_comPrdBlk24to17; i++) {
        param_t param = static_cast<param_t> (i);
        ASSERT_TRUE(setValue(param, SRC_pi, i));
        ASSERT_TRUE(checkValue(param, SRC_vkey, i));

        uint8_t valueEn = ~i & 0xFF;
        ASSERT_EQ(valueEn , mModbus->getSwitchLed(param, ON_OFF_off))
                << " params is " << param;

        uint8_t valueDs = i & 0xFF;
        ASSERT_EQ(valueDs, mModbus->getSwitchLed(param, ON_OFF_on))
                << " params is " << param;
    }

    //
    // Проверка светодиодов при блокировке приемника SAC1
    //
    ASSERT_TRUE(setValue(PARAM_blkComPrmAll, SRC_pi, DISABLE_PRM_disable));

    for(uint16_t i = PARAM_comPrmBlk08to01; i <= PARAM_comPrdBlk24to17; i++) {
        param_t param = static_cast<param_t> (i);

        uint8_t valueEn = mModbus->isParamComPrmBlk(param) ? 0x00 : ~i & 0xFF;
        ASSERT_EQ(valueEn, mModbus->getSwitchLed(param, ON_OFF_off))
                << " params is " << param;

        uint8_t valueDs = mModbus->isParamComPrmBlk(param) ? 0xFF : i & 0xFF;
        ASSERT_EQ(valueDs, mModbus->getSwitchLed(param, ON_OFF_on))
                << " params is " << param;
    }

    //
    // Проверка светодиодов без блокировки приемника SAC1
    //
    ASSERT_TRUE(setValue(PARAM_blkComPrmAll, SRC_pi, DISABLE_PRM_enable));

    for(uint16_t i = PARAM_comPrmBlk08to01; i <= PARAM_comPrdBlk24to17; i++) {
        param_t param = static_cast<param_t> (i);

        uint8_t valueEn = ~i & 0xFF;
        ASSERT_EQ(valueEn , mModbus->getSwitchLed(param, ON_OFF_off))
                << " params is " << param;

        uint8_t valueDs = i & 0xFF;
        ASSERT_EQ(valueDs, mModbus->getSwitchLed(param, ON_OFF_on))
                << " params is " << param;
    }
}

//
TEST_F(TModbusVp_Test, getSwitchLed_Two)
{
    // Сброс параметров
    mParam->reset();

    // Светодиоды не установленных параметров не горят
    uint16_t result16 = 0x0000;
    for(uint16_t i = PARAM_comPrmBlk08to01; i <= PARAM_comPrdBlk24to17; i++) {
        param_t p1 = static_cast<param_t> (i);

        for(uint16_t j = i; j <= PARAM_comPrdBlk24to17; j++) {
            param_t p2 = static_cast<param_t> (j);

            ASSERT_EQ(result16, mModbus->getSwitchLed(p2, p1, ON_OFF_off));
            ASSERT_EQ(result16, mModbus->getSwitchLed(p2, p1, ON_OFF_on));
        }
    }

    // установка параметров
    for(uint16_t i = PARAM_comPrmBlk08to01; i <= PARAM_comPrdBlk24to17; i++) {
        param_t param = static_cast<param_t> (i);
        ASSERT_TRUE(setValue(param, SRC_pi, i));
        ASSERT_TRUE(checkValue(param, SRC_vkey, i));
    }

    //
    // Проверка светодиодов при некорректном состоянии блокировки приемника SAC1
    //

    // Две группы светодиодов
    for(uint16_t i = PARAM_comPrmBlk08to01; i <= PARAM_comPrdBlk24to17; i++) {
        param_t p1 = static_cast<param_t> (i);

        for(uint16_t j = i; j <= PARAM_comPrdBlk24to17; j++) {
            param_t p2 = static_cast<param_t> (j);


            uint16_t valueDs = static_cast<uint16_t> (j << 8) + (i & 0xFF);
            ASSERT_EQ(valueDs, mModbus->getSwitchLed(p2, p1, ON_OFF_on))
                    << " param 1 is " << p1 << ", param 2 is " << p2;

            uint16_t valueEn = ~valueDs;
            ASSERT_EQ(valueEn , mModbus->getSwitchLed(p2, p1, ON_OFF_off))
                    << " param 1 is " << p1 << ", param 2 is " << p2;
        }
    }

    //
    // Проверка светодиодов при блокировке приемника SAC1
    //
    ASSERT_TRUE(setValue(PARAM_blkComPrmAll, SRC_pi, DISABLE_PRM_disable));

    // Две группы светодиодов
    for(uint16_t i = PARAM_comPrmBlk08to01; i <= PARAM_comPrdBlk24to17; i++) {
        param_t p1 = static_cast<param_t> (i);

        for(uint16_t j = i; j <= PARAM_comPrdBlk24to17; j++) {
            param_t p2 = static_cast<param_t> (j);

            uint16_t valueDs = mModbus->isParamComPrmBlk(p2) ? 0xFF : (j & 0xFF);
            valueDs = static_cast<uint16_t> (valueDs << 8);
            valueDs += mModbus->isParamComPrmBlk(p1) ? 0xFF : (i & 0xFF);
            ASSERT_EQ(valueDs, mModbus->getSwitchLed(p2, p1, ON_OFF_on))
                    << " param 1 is " << p1 << ", param 2 is " << p2;

            uint16_t valueEn = ~valueDs;
            ASSERT_EQ(valueEn , mModbus->getSwitchLed(p2, p1, ON_OFF_off))
                    << " param 1 is " << p1 << ", param 2 is " << p2;
        }
    }

    //
    // Проверка светодиодов без блокировки приемника SAC1
    //
    ASSERT_TRUE(setValue(PARAM_blkComPrmAll, SRC_pi, DISABLE_PRM_enable));

    // Две группы светодиодов
    for(uint16_t i = PARAM_comPrmBlk08to01; i <= PARAM_comPrdBlk24to17; i++) {
        param_t p1 = static_cast<param_t> (i);

        for(uint16_t j = i; j <= PARAM_comPrdBlk24to17; j++) {
            param_t p2 = static_cast<param_t> (j);

            uint16_t valueDs = static_cast<uint16_t> (j << 8) + (i & 0xFF);
            ASSERT_EQ(valueDs, mModbus->getSwitchLed(p2, p1, ON_OFF_on))
                    << " param 1 is " << p1 << ", param 2 is " << p2;

            uint16_t valueEn = ~valueDs;
            ASSERT_EQ(valueEn , mModbus->getSwitchLed(p2, p1, ON_OFF_off))
                    << " param 1 is " << p1 << ", param 2 is " << p2;
        }
    }
}

} // namespace BVP

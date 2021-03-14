#ifndef AVANT_PI_H
#define AVANT_PI_H

#include "ringArray.hpp"
#include "protocolAvant.h"

namespace BVP {

class TAvantPi : public TProtocolAvant {

    /// Значение байта данных для команды COM_AVANT_control "Управление"
    enum comControlBytes_t {
        COM_CONTROL_BYTES_selfReset = 1 //<
    };

    /// Значение байта данных для команды COM_AVANT_getMisc
    enum comMiscBytes_t {
        COM_MISC_BYTES_netAdr = 1,
        COM_MISC_BYTES_protocol,
        COM_MISC_BYTES_baudrate,
        COM_MISC_BYTES_dataBits,
        COM_MISC_BYTES_parity,
        COM_MISC_BYTES_stopBits,
        COM_MISC_BYTES_password,
        COM_MISC_BYTES_vpSac2 = COM_MISC_BYTES_password + 2,
        //
        COM_MISC_BYTES_MAX
    };

    /// Значение байта данных для команды COM_AVANT_getPrdBlock
    enum comPrdBlockBytes_t {
        COM_PRD_BLOCK_BYTES_com08to01 = 1,
        COM_PRD_BLOCK_BYTES_com16to09,
        COM_PRD_BLOCK_BYTES_com24to17,
        COM_PRD_BLOCK_BYTES_com32to25,
        //
        COM_PRD_BLOCK_BYTES_MAX
    };

    /// Значение байта данных для команды COM_AVANT_getPrmBlock
    enum comPrmBlockBytes_t {
        COM_PRM_BLOCK_BYTES_com08to01 = 1,
        COM_PRM_BLOCK_BYTES_com16to09,
        COM_PRM_BLOCK_BYTES_com24to17,
        COM_PRM_BLOCK_BYTES_com32to25,
        //
        COM_PRM_BLOCK_BYTES_MAX
    };

    /// Значение байта данных для команды COM_AVANT_getPrmDisable
    enum comPrmDisableBytes_t {
        COM_PRM_DISABLE_BYTES_prmDisable = 1,
        //
        COM_PRM_DISABLE_BYTES_MAX
    };

public:
    TAvantPi(regime_t regime);

private:
    TRingArray<comAvant_t, 4, COM_AVANT_getError> ringComArray;

    /**
     * @brief Формирование команды управления.
     * @return true если команды была сформирована, иначе false.
     */
    bool writeComControl();

    /**
     * @brief Формирование команды записи "Другие параметры"
     * @return
     */
    bool writeComMisc();

    /**
     * @brief Формирование команды записи "Вывод ПРМ (SAC1)"
     * @return
     */
    bool writeComPrmDisable();

    /**
     * @brief Формирование команды записи "Блокированные команды ПРД"
     * @return
     */
    bool writeComPrdBlock();

    /**
     * @brief Формирование команды записи "Блокированные команды ПРМ"
     * @return
     */
    bool writeComPrmBlock();

    /**
     * @brief Обработчик команды чтения неисправностей и предупреждений
     * Команда проверяется на минимальный размер данных.
     * @return true если в команде нет ошибок, иначе false.
     */
    bool comGetError();

    /**
     * @brief Обработчик команды "Другие параметры"
     * @param[in] group Группа команды.
     * @return true если в команде нет ошибок, инчаче false
     */
    bool comGetMisc(comAvantMaskGroup_t group);

    /**
     * @brief comGetMiscGet
     * @param[in] pos Номер параметра (номер байта в буфере).
     * @param[in] buf Указатель на буфер (первый не обработанный байт).
     * @param[in] len Количество не обработанных данных в буфере.
     * @return
     */
    uint16_t comGetMisc(comMiscBytes_t pos, const uint8_t *buf, uint16_t len);

    /**
     * @brief Обработчик команды "Вывод ПРМ (SAC1)"
     * @param[in] group Группа команды.
     * @return true если в команде нет ошибок, инчаче false
     */
    bool comGetPrmDisable(comAvantMaskGroup_t group);

    /**
     * @brief comGetPrmDisable
     * @param[in] pos Номер параметра (номер байта в буфере).
     * @param[in] buf Указатель на буфер (первый не обработанный байт).
     * @param[in] len Количество не обработанных данных в буфере.
     * @return
     */
    uint16_t comGetPrmDisable(comPrmDisableBytes_t pos, const uint8_t *buf,
                              uint16_t len);

    /**
     * @brief Обработчик команды "Блокированные команды ПРД"
     * @param[in] group Группа команды.
     * @return true если в команде нет ошибок, инчаче false
     */
    bool comGetPrdBlock(comAvantMaskGroup_t group);

    /**
     * @brief comGetPrdBlock
     * @param[in] pos Номер параметра (номер байта в буфере).
     * @param[in] buf Указатель на буфер (первый не обработанный байт).
     * @param[in] len Количество не обработанных данных в буфере.
     * @return
     */
    uint16_t comGetPrdBlock(comPrdBlockBytes_t pos, const uint8_t *buf,
                            uint16_t len);

    /**
     * @brief Обработчик команды "Блокированные команды ПРМ"
     * @param[in] group Группа команды.
     * @return true если в команде нет ошибок, инчаче false
     */
    bool comGetPrmBlock(comAvantMaskGroup_t group);

    /**
     * @brief comGetPrmBlock
     * @param[in] pos Номер параметра (номер байта в буфере).
     * @param[in] buf Указатель на буфер (первый не обработанный байт).
     * @param[in] len Количество не обработанных данных в буфере.
     * @return
     */
    uint16_t comGetPrmBlock(comPrmBlockBytes_t pos, const uint8_t *buf,
                              uint16_t len);

    /**
     * @brief Обработчик команды чтения даты и времени.
     * Команда проверяется на минимальный размер данных.
     * @return true если в команде нет ошибок, иначе false.
     */
    bool comGetTime();

    bool vWriteAvant() override;

    bool vReadAvant() override;
};

} // namespace BVP

#endif // AVANT_PI_H

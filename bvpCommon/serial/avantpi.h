#ifndef AVANT_PI_H
#define AVANT_PI_H

#include "ringArray.hpp"
#include "protocolAvant.h"

namespace BVP {

class TAvantPi : public TProtocolAvant {

    /// Структура данных команды.
    struct comData_t {
        param_t param;      ///< Параметр.
        uint8_t position;   ///< Позиция.
    };

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
        COM_MISC_BYTES_vpSam,
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

    /// Значение байта данных для команды COM_AVANT_getPrdKeep
    enum comPrdKeepBytes_t {
        COM_PRD_KEEP_BYTES_prdKeep = 1,     // Удержание реле команд ПРД, uint8_t
        COM_PRD_KEEP_BYTES_compatible,      // Совместимость, uint8_t
        COM_PRD_KEEP_BYTES_tm,              // Телемеханика, uint8_t
        COM_PRD_KEEP_BYTES_warnThdD,        // Уровень срабатывания предупредительной сигнализации по D, uint8_t
        COM_PRD_KEEP_BYTES_alarmThdD,       // Уровень срабатывания аварийной сигнализации по D, uint8_t
        COM_PRD_KEEP_BYTES_tempControl,     // Контроль температуры, uint8_t
        COM_PRD_KEEP_BYTES_tempThdHi,       // Верхнее значение температуры, uint8_t
        COM_PRD_KEEP_BYTES_tempThdLow,      // Нижнее значение температуры, uint8_t
        COM_PRD_KEEP_BYTES_tmSpeed,         // Скорость ТМ, uint8_t
        COM_PRD_KEEP_BYTES_ringTimeWait,    // Время ожидания команд (кольцо), uint8_t
        COM_PRD_KEEP_BYTES_ringTr08to01,    // Транзитные команды 8-1 (кольцо), uint8_t, bits
        COM_PRD_KEEP_BYTES_ringTr16to09,    // Транзитные команды 16-9 (кольцо), uint8_t, bits
        COM_PRD_KEEP_BYTES_ringTr24to17,
        COM_PRD_KEEP_BYTES_ringTr32to31,
        COM_PRD_KEEP_BYTES_ringTr40to33,
        COM_PRD_KEEP_BYTES_ringTr48to41,
        COM_PRD_KEEP_BYTES_ringTr56to49,
        COM_PRD_KEEP_BYTES_ringTr64to57,
        COM_PRD_KEEP_BYTES_ringTr72to65,
        COM_PRD_KEEP_BYTES_ringTr80to73,
        COM_PRD_KEEP_BYTES_ringTr88to81,
        COM_PRD_KEEP_BYTES_ringTr96to89,    // Транзитные команды 96-89 (кольцо), uint8_t, bits
        COM_PRD_KEEP_BYTES_alarmRstMode,    // Режим сброса сигнализации, uint8_t
        //
        COM_PRD_KEEP_BYTES_MAX
    };
    Q_STATIC_ASSERT(COM_PRD_KEEP_BYTES_alarmRstMode == 23);


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
    TRingArray<com_t, 5, COM_getError> ringComArray;

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
    bool comGetMisc(group_t group);

    /**
     * @brief comGetMiscGet
     * @param[in] pos Номер параметра (номер байта в буфере).
     * @param[in] buf Указатель на буфер (первый не обработанный байт).
     * @param[in] len Количество не обработанных данных в буфере.
     * @return
     */
    uint16_t comGetMisc(comMiscBytes_t pos, const uint8_t *buf, uint16_t len);

    /**
     * @brief Обработчик команды "Удержание реле команд ПРД"
     * @param[in] group Группа команды.
     * @return true если в команде нет ошибок, инчаче false
     */
    bool comGetPrdKeep(group_t group);

    /**
     * @brief comGetPrdKeep
     * @param[in] pos Номер параметра (номер байта в буфере).
     * @param[in] buf Указатель на буфер (первый не обработанный байт).
     * @param[in] len Количество не обработанных данных в буфере.
     * @return
     */
    uint16_t comGetPrdKeep(comPrdKeepBytes_t pos, const uint8_t *buf,
                              uint16_t len);

    /**
     * @brief Обработчик команды "Вывод ПРМ (SAC1)"
     * @param[in] group Группа команды.
     * @return true если в команде нет ошибок, инчаче false
     */
    bool comGetPrmDisable(group_t group);

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
    bool comGetPrdBlock(group_t group);

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
    bool comGetPrmBlock(group_t group);

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
     * @brief Обработчик команды "Сброс индикации приемника"
     * @param[in] group Группа команды.
     * @return true если в команде нет ошибок, инчаче false
     */
    bool comResetInd(group_t group);

    /**
     * @brief Обработчик команды чтения даты и времени.
     * Команда проверяется на минимальный размер данных.
     * @return true если в команде нет ошибок, иначе false.
     */
    bool comGetTime();

    /**
     * @brief Формирование новой команды для передачи.
     * @return true если есть команда для передачи, иначе false.
     */
    bool vWriteAvant() override;

    /**
     * @brief Обработка принятой команды.
     * @return true если комада обработана, иначе false.
     */
    bool vReadAvant() override;
};

} // namespace BVP

#endif // AVANT_PI_H

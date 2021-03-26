#ifndef MODBUS_VP_H
#define MODBUS_VP_H

#include <cstdint>
#include <cstddef>
#include "serialprotocol.h"

namespace BVP {

class TModbusVp : public TSerialProtocol {

    enum regRead_t {
        REG_READ_MIN = 1,
        //
        REG_READ_sanSbSac = REG_READ_MIN,
        REG_READ_sa16to01,
        REG_READ_sa32to17,
        REG_READ_sa48to33,
        REG_READ_sa64to49,
        REG_READ_version = 10,
        //
        REG_READ_MAX
    };

    enum regWrite_t {
        REG_WRITE_MIN = 0x0B,
        //
        REG_WRITE_enSanSbSac = REG_WRITE_MIN,
        REG_WRITE_enLed16to01,
        REG_WRITE_enLed32to17,
        REG_WRITE_enLed48to33,
        REG_WRITE_enLed64to49,
        REG_WRITE_dsSanSbSac,
        REG_WRITE_dsLed16to01,
        REG_WRITE_dsLed32to17,
        REG_WRITE_dsLed48to33,
        REG_WRITE_dsLed64to49,
        //
        REG_WRITE_MAX
    };

    /// Список команд протокола.
    enum com_t {
        COM_readHoldingRegs = 0x03, /// Команда чтения регистров.
        COM_writeMultRegs   = 0x10, /// Команда записи регистров.
        COM_readWriteRegs   = 0x17  /// Команда чтения/записи регистров.
    };


    /// Максимальное время для получения ответа (в данной реализации).
    const uint32_t kMaxTimeToResponseUs = 200000UL;
    /// Максимальное количество сообщений без ответа до потери связи.
    const uint8_t kMaxLostMessage = 5;
    /// Максимальная длина RTU кадра
    const uint16_t kMaxSizeFrameRtu = 256;

   public:
    TModbusVp(regime_t regime);
    ~TModbusVp() override;

    bool setEnable(bool enable) override;
    bool isEnable() const override;
    bool read() override;
    void readError() override;
    bool write() override;
    uint16_t pop(uint8_t *data[]) override;
    void sendFinished() override;
    bool vPush(uint8_t byte) override;
    bool setNetAddress(uint16_t address) override;
    bool vSetup(uint32_t baudrate, bool parity, uint8_t stopbits) override;
    void vTick() override;
    bool isConnection() const override;

    /**
     * @brief Возвращает текущее состояние светодиодов для двух групп переключателей.
     * Если бит установлен в 0, то светодиод потушен.
     * Если значение параметра еще не установлено то светодиоды этого
     * параметра потушены.
     * @param[in] hi Параметр со значением для страших восьми бит.
     * @param[in] low Параметр со значением для младших восьми бит.
     * @param[in] sw Выбор необходимых светодиодов.
     * @return Состояние светодиодов.
     */
    uint16_t getSwitchLed(param_t hi, param_t low, switchOff_t sw) const;

   private:
    /// Текущее состояние.
    state_t mState;
    /// Количество тиков для определения принятого сообщения.
    const uint32_t mTimeToCompleteUs;
    /// Количество тиков для определения ошибки принятия сообщения.
    const uint32_t mTimeToErrorUs;
    /// Количество сообщений без ответа.
    uint8_t cntLostMessage;

    /// Увеличивает счетчик ошибок без ответа.
    void incLostMessageCounter();

    /** Добавляет в сообщение запрос для считывания регистров.
     *
     *  Адрес регистра в сообщении на единицу меньше номера.
     *  Если аргумент ok при вызове имеет значение false, то сразу на выход.
     *
     *  // TODO Заменить ok на код ошибки. Для возомжности формирования ответов с ошибкой.
     *
     *  @param[out] buf Начальная позиция в буфере сообщения.
     *  @param min Номер младшего регистра.
     *  @param max Номер старшего регистра.
     *  @param[in/out] ok true если номера регистров найдены, иначе false.
     *  @return Количество заполненных байт в сообщении.
     */
    uint16_t addReadRegMsg(uint8_t buf[], uint16_t min, uint16_t max, bool &ok);

    /** Добавляет в сообщение данные для записи регистров.
     *
     *  Адрес регистра в сообщении на единицу меньше номера.
     *  Если аргумент ok при вызове имеет значение false, то сразу на выход.
     *
     *  // TODO Заменить ok на код ошибки. Для возомжности формирования ответов с ошибкой.
     *
     *  @param[out] buf Начальная позиция в буфере сообщения.
     *  @param[in] min Номер младшего регистра.
     *  @param[in] max Номер старшего регистра.
     *  @param[in/out] ok true если номера регистров найдены, иначе false.
     *  @return Количество заполненных байт в сообщении.
     */
    uint16_t addWriteRegMsg(uint8_t buf[], uint16_t min, uint16_t max, bool &ok);

    /** Проверяет принятое сообщение.
     *
     *  После проверки CRC количество необработанных байт в сообщении
     *  уменьшается на 2.
     *
     *  Если аргумент ok при вызове имеет значение false, то сразу на выход.
     *
     *  @param[in] buf Начальная позиция в буфере сообщения.
     *  @param[in/out] Количество необработанных байт в сообщении.
     *  @param[in/out] ok true если проверка пройдена, иначе false.
     *  @return Количество обработанных байт в сообещнии.
     */
    uint16_t checkReadMsg(const uint8_t buf[], uint16_t &len, bool &ok);

    /** Возвращает значение для указанного номера регистра.
     *
     *  Если аргумент ok при вызове имеет значение false, то сразу на выход.
     *  // TODO Заменить ok на код ошибки. Для возомжности формирования ответов с ошибкой.
     *
     *  @param[in] number Номер регистра.
     *  @param[in/out] ok true если номер регистра найден, иначе false.
     *  @return Значенеи регистра.
     */
    uint16_t getWriteRegMsgData(uint16_t number, bool &ok) const;

    /** Извлекает значения регистров из сообщения.
     *
     *  Если аргумент ok при вызове имеет значение false, то сразу на выход.
     * // TODO Заменить ok на код ошибки. Для возомжности формирования ответов с ошибкой.
     *
     *  @param[in] buf Начальная позиция в буфере сообщения.
     *  @param[in/out] Количество необработанных байт в сообщении.
     *  @param[in] min Номер младшего регистра.
     *  @param[in] max Номер старшего регистра.
     *  @param[in/out] ok true если номера регистров найдены, иначе false.
     *  @return Количество обработанных байт в сообщении.
     */
    uint16_t getReadReg(const uint8_t buf[], uint16_t &len, uint16_t min,
                        uint16_t max, bool &ok);

    /** Извлекает значение для указанного номера регистра.
     *
     *  Если аргумент ok при вызове имеет значение false, то сразу на выход.
     *  // TODO Заменить ok на код ошибки. Для возомжности формирования ответов с ошибкой.
     *
     *  @param[in] buf Начальная позиция в буфере сообщения.
     *  @param[in] number Номер регистра.
     *  @param[in/out] ok true если данные для регистра обработаны, иначе false.
     *  @return Количество обработанных байт в сообщении.
     */
    uint16_t getReadRegMsgData(const uint8_t buf[], uint16_t number, bool &ok);

    /** Вычисляет CRC для массива данных.
     *
     *  @param[in] buffer Массив данных.
     *  @param[in] len Количество данных в массиве.
     *  @param[in] crc Текущее значение CRC.
     *  @return Значение CRC.
     */
    uint16_t calcCRC(const uint8_t buf[], size_t len, uint16_t crc=0xFFFF);

    /**
     * @brief Обработка нажатия на переключатели SB, SAC и Snn
     * @param value Состояние переключателей считанное с панели.
     */
    void hdlrButtonSbSacSan(uint32_t value);

    /**
     * @brief Обработка нажатия на переключатели SB, SAC и Snn
     * @param[in] btn Нажатый переключатель.
     */
    void hdlrButtonSbSacSan(vpBtnControl_t btn);

    /**
     * @brief Обработка нажатия на переключатели блокировки команд SA
     * Еще не сохраненное предыдущее изменение будет потеряно.
     * Смена значения переключателя осуществляется только в случае если это
     * разрешено переключателем SAC2.
     * Смена значений переключателей блокировки команд приемника не производится
     * если переключатель SAC1 блокирует приемник.
     * @param[in] param Параметр блокировки.
     * @param[in] cvalue Состояние переключателей считанное с панели.
     * @param[in] lparam Параметр с предыдущим состоянием переключателей.
     * @param[in] num Номер байта в предыдущем состоянии переключателей [1..4].
     */
    void hdlrButtonSa(param_t param, uint32_t cvalue, param_t lparam, uint8_t num);

    /**
     * @brief Возвращает текущее состояние светодиодов для переключателя.
     * Если бит установлен в 0, то светодиод потушен.
     * Если значение параметра еще не установлено все светодиоды потушены.
     * @param[in] hi Параметр.
     * @param[in] sw Задает необходимое положение светодиодов.
     * @return Состояние светодиодов.
     */
    uint8_t getSwitchLed(param_t param, switchOff_t sw) const;

    /**
     * @brief Проверяет блокировку выходов приемника (SAC1).
     * @return true если выходы заблокированы, иначе false.
     */
    bool isBlockPrm() const;

    /**
     * @brief Проверяет является ли параметр блокировкой команд приемника.
     * @return true если это параметр блокировки команд приемника, иначе false.
     */
    bool isParamComPrmBlk(param_t param) const;

#ifdef TEST_FRIENDS
    TEST_FRIENDS;
#endif
};

} // namespace BVP

#endif // MODBUS_VP_H

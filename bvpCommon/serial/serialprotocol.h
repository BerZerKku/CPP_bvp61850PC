#ifndef TSERIALPROTOCOL_H
#define TSERIALPROTOCOL_H

#include <cstdint>
#include "param.h"

namespace BVP {

class TSerialProtocol {

    /// Максимальное время с момента принятия первого байта данных, мкс.
    static const uint32_t kMaxTimeFromReadFirstByte = 10000000UL;

public:
    /// Режим работы протокола
    enum regime_t {
        REGIME_slave = 0, ///< Ведомый.
        REGIME_master     ///< Ведущий.
    };

    TSerialProtocol(regime_t regime);
    virtual ~TSerialProtocol();

    /**
     * @brief Запуск или останов работы протокола.
     * NOTE Настройки протокола должны быть сделаны до запуска.
     * @param[in] enable true если протокол надо запустить, иначе false.
     * @return true если протокол запущен, иначе false.
     */
    virtual bool setEnable(bool enable) = 0;

    /// Проверяет запущен протокол или нет.
    virtual bool isEnable() const = 0;

    /**
     * @brief Обрабатывает принятые сообщения.
     * @return true если было принято и обработано сообщение, иначе false.
     */
    virtual bool read() = 0;

    /// Обрабатывает ошибку приема данных.
    virtual void readError() = 0;

    /**
     * @brief Подготавливает сообщение для передачи.
     * @return true если есть сообщение для передачи, иначе false.
     */
    virtual bool write() = 0;

    /**
     * @brief Извлекает данные для передачи.
     * @param[out] data Указатель на начало данных.
     * @return Количество байт для передачи.
     */
    virtual uint16_t pop(uint8_t *data[]) = 0;

    /// Сообщает об окончании передачи данных.
    virtual void sendFinished() = 0;

    /**
     *  @brief Добавляет принятый байт данных.
     *  @param[in] byte Байт данных.
     *  @return true если байт обработан, иначе false.
     */
    bool push(uint8_t byte);

    /**
     * @brief Устанавливает адрес устройства в локальной сети.
     * @param[in] address Адрес устройства в локальной сети.
     * @return true если адрес установлен, иначе false.
     */
    virtual bool setNetAddress(uint16_t address) = 0;

    /**
     * @brief Устанавливает период вызова функции tick.
     * @param[in] ticktime Период вызова функции tick, мкс.
     * @return true если период корректный, иначе false.
     */
    bool setTimeTick(uint32_t ticktime);

    /**
     * @brief Устанавливает для протокола Id источника данных.
     * В случае ошибочного Id будет установлен SRC_MAX.
     * @param[in] id ID источника данных.
     * @return true если Id установлен.
     */
    bool setSrcId(src_t id);

    /**
     * @brief Возвращает Id источника данных для протокола.
     * @return ID источника данных.
     */
    src_t getSrcId() const;

    /**
     * @brief Устанавливает настройки последовательного порта.
     * @param[in] baudrate Скорость работы порта, бит/с.
     * @param[in] parity true если включен контроль четности, иначе false.
     * @param[in] stopbits Количество стоп-бит, 1 или 2.
     * @return  true если настройки корректные, иначе false.
     */
    bool setup(uint32_t baudrate, bool parity, uint8_t stopbits);

    /// Вызывается с периодом указанным в setupTick.
    void tick();

    /**
     * @brief Устанавливает буфер и его размер.
     * @param[in] buf
     * @param size
     */
    void setBuffer(uint8_t buf[], uint16_t size);

    /**
     * @brief Проверяет наличие связи.
     * @return true если связь есть, иначе false.
     */
    virtual bool isConnection() const = 0;

    /**
     * @brief Возвращает время прошедшее с момента приема первого байта сообщения, мкс.
     * @return Время.
     */
    uint32_t getTimeFromReadStart() const {
        return mTimeReadStart;
    }

protected:

    /// Список состояний протокола.
    enum state_t {
        STATE_disable = 0,      ///< Отключен.
        STATE_idle,             ///< В ожидании.
        STATE_reqSend,          ///< Есть данные для передачи.
        STATE_waitSendFinished, ///< Ожидание оконачания передачи.
        STATE_waitForReply,     ///< Ожидание ответа.
        STATE_procReply,        ///< Обработка ответа.
        STATE_errorReply,       ///< Ошибка в ответе.
        //
        STATE_MAX
    };

    /// Передача сообщений между протоколами.
    struct transfer_t {
        src_t srcData;          ///< Источник сообщения для ответа.
        bool req = false;       ///< Флаг необходимости передать сообщение.
        bool resp = false;      ///< Флаг необходимости передать ответ.
        uint16_t dataSize;      ///< Размер сообщения.
        const uint8_t * data;   ///< Сообщение.
    };

    static transfer_t mTransfer[src_t::SRC_MAX];
//    transfer_t mTransfer[src_t::SRC_MAX];

    const uint32_t mPrtId = uint32_t(-1);   ///< ID протокола.
    const src_t mSrcId = SRC_MAX;           ///< ID источника данных для протокола.

    const regime_t mRegime;             ///< Режим работы протокола.
    TParam * const mParam;              ///< Параметры.
    uint8_t * const mBuf;               ///< Буфер данных.
    const uint16_t mSize;               ///< Размер буфера данных.
    uint16_t mPos;                      ///< Текущая позиция в буфере.
    uint16_t mLen;                      ///< Количество байт данных по протоколу.
    const uint8_t mNetAddress;          ///< Адрес опрашиваемого устройства.
    uint32_t mTimeReadStart;            ///< Время прошедшее с момента приема первого байта.
    uint32_t mTimeUs;                   ///< Счетчик времени.
    const uint32_t mTimeTickUs;         ///< Период вызова функции tick.
    const uint32_t mTimeOneByteUs;      ///< Время передачи/приема одного байта данных.


    /**
     * @brief Передает байт данных на обработку в протокол.
     * Для правильного определения времени прошедшего с момента приема первого
     * байта данных, необходимо текущее количество байт данных хранить в mPos.
     * @param[in] byte Байт данных.
     * @return true если байт обработан, иначе false.
     */
    virtual bool vPush(uint8_t byte) = 0;

    /**
     * @brief Вызывается с периодом указанным в setupTick.
     * Обработка времени для протокола.
     */
    virtual void vTick() = 0;

    /**
     * @brief Устанавливает настройки последовательного порта.
     * @param[in] baudrate Скорость работы порта, бит/с.
     * @param[in] parity true если включен контроль четности, иначе false.
     * @param[in] stopbits Количество стоп-бит, 1 или 2.
     * @return  true если настройки корректные, иначе false.
     */
    virtual bool vSetup(uint32_t baudrate, bool parity, uint8_t stopbits) = 0;

    /**
     * @brief Сброс настроек передачи сообщений между протоколами.
     */
    void transferReset();

    /**
     * @brief Передать сообщение в другой порт ?!
     * FIXME А все таки должны быть протоколы или порты?
     * @param[in] dst Пункт назначения.
     * @param[in] data Данные.
     * @param[in] size Количество данных.
     * @return true если сообщение будет передано, иначе false.
     */
    bool transferTo(src_t dst, const uint8_t *data, uint16_t size);

    /**
     * @brief Передать ответ в другой порт ?!
     * FIXME А все таки должны быть протоколы или порты?
     * @param[in] dst Пункт назначения.
     * @param[in] data Данные.
     * @param[in] size Количество данных.
     * @return true если сообщение будет передано, иначе false.
     */
    bool transferFrom(src_t dst, const uint8_t *data, uint16_t size);

    /**
     * @brief Проверяет необходимость передачи данных из другого порта.
     * @return true если есть данные для передачи, инчае false.
     */
    bool isTransferReq();

    /**
     * @brief Проверяет необходимость передать ответ в другой порт.
     * @return true если необходимо передать ответ, инчае false.
     */
    bool isTransferResp();

    /**
     * @brief Копирует данные для передачи в буфер.
     * @return количество данных для передачи.
     */
    uint16_t copyTransferDataReq();

private:

    /**
     * @brief Сброс настроек передачи сообщений между протоколами.
     * @param[in] src Источник данных.
     */
    void transferReset(src_t src);
};

} // namespace BVP

#endif // TSERIALPROTOCOL_H

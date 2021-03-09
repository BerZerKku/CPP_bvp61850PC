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

    /** Запуск или останов работы протокола.
     *
     *  !!! Настройки протокола должны быть сделаны до запуска.
     *
     *  @param[in] enable true если протокол надо запустить, иначе false.
     *  @return true если протокол запущен, иначе false.
     */
    virtual bool setEnable(bool enable) = 0;

    /// Проверяет запущен протокол или нет.
    virtual bool isEnable() const = 0;

    /** Обрабатывает принятые сообщения.
     *
     *  @return true если было принято и обработано сообщение, иначе false.
     */
    virtual bool read() = 0;

    /// Обрабатывает ошибку приема данных.
    virtual void readError() = 0;

    /** Подготавливает сообщение для передачи.
     *
     *  @return true если есть сообщение для передачи, иначе false.
     */
    virtual bool write() = 0;

    /** Извлекает данные для передачи.
     *
     *  @param[out] data Указатель на начало данных.
     *  @return Количество байт для передачи.
     */
    virtual uint16_t pop(uint8_t *data[]) = 0;

    /// Сообщает об окончании передачи данных.
    virtual void sendFinished() = 0;

    /** Добавляет принятый байт данных.
     *
     *  @param[in] byte Байт данных.
     *  @return true если байт обработан, иначе false.
     */
    bool push(uint8_t byte);

    /** Устанавливает адрес устройства в локальной сети.
     *
     *  @param[in] address Адрес устройства в локальной сети.
     *  @return true если адрес установлен, иначе false.
     */
    virtual bool setNetAddress(uint16_t address) = 0;

    /** Устанавливает период вызова функции tick.
     *
     *  @param[in] ticktime Период вызова функции tick, мкс.
     *  @return true если период корректный, иначе false.
     */
    bool setTimeTick(uint32_t ticktime);

    /** Устанавливает ID протокола.
     *
     *  @param[in] id ID протокола.
     */
    virtual void setID(uint32_t id) = 0;

    /** Устанавливает настройки последовательного порта.
     *
     *  @param[in] baudrate Скорость работы порта, бит/с.
     *  @param[in] parity true если включен контроль четности, иначе false.
     *  @param[in] stopbits Количество стоп-бит, 1 или 2.
     *  @return  true если настройки корректные, иначе false.
     */
    bool setup(uint32_t baudrate, bool parity, uint8_t stopbits);

    /// Вызывается с периодом указанным в setupTick.
    void tick();

    /** Устанавливает буфер и его размер.
     *
     *  @param[in] buf
     *  @param size
     */
    void setBuffer(uint8_t buf[], uint16_t size);

    /** Проверяет наличие связи.
     *
     *  @return true если связь есть, иначе false.
     */
    virtual bool isConnection() const = 0;

    /** Возвращает время прошедшее с момента приема первого байта сообщения, мкс.
     *
     *  @return Время.
     */
    uint32_t getTimeFromReadStart() const {
        return mTimeReadStart;
    }

protected:

    /// Список состояний протокола.
    enum state_t {
        STATE_disable = 0,
        STATE_idle,
        STATE_reqSend,
        STATE_waitSendFinished,
        STATE_waitForReply,
        STATE_procReply,
        STATE_errorReply,
        //
        STATE_MAX
    };

    regime_t mRegime;       ///< Режим работы протокола.
    TParam * const mParam;  ///< Параметры.
    uint8_t * const mBuf;   ///< Буфер данных.
    const uint16_t mSize;   ///< Размер буфера данных.
    uint16_t mPos;          ///< Текущая позиция в буфере.
    uint16_t mLen;          ///< Количество байт данных по протоколу.
    const uint8_t mNetAddress;  ///< Адрес опрашиваемого устройства.
    uint32_t mTimeReadStart;    ///< Время прошедшее с момента приема первого байта.
    uint32_t mTimeUs;           ///< Счетчик времени.
    const uint32_t mTimeTickUs; ///< Период вызова функции tick.
    const uint32_t mTimeOneByteUs;  ///< Время передачи/приема одного байта данных.

    /** Добавляет принятый байт данных.
     *
     *  Прием байта для протокола.
     *
     *  Для правильного определения времени прошедшего с момента приема первого
     *  байта данных, необходимо текущее количество байт данных хранить в mPos.
     *
     *  @param[in] byte Байт данных.
     *  @return true если байт обработан, иначе false.
     */
    virtual bool vPush(uint8_t byte) = 0;

    /** Вызывается с периодом указанным в setupTick.
     *
     *  Обработка времени для протокола.
     */
    virtual void vTick() = 0;

    /** Устанавливает настройки последовательного порта.
     *
     *  Настройка для протокола.
     *
     *  @param[in] baudrate Скорость работы порта, бит/с.
     *  @param[in] parity true если включен контроль четности, иначе false.
     *  @param[in] stopbits Количество стоп-бит, 1 или 2.
     *  @return  true если настройки корректные, иначе false.
     */
    virtual bool vSetup(uint32_t baudrate, bool parity, uint8_t stopbits) = 0;
};

} // namespace BVP

#endif // TSERIALPROTOCOL_H

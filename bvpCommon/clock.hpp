#ifndef CLOCK_HPP
#define CLOCK_HPP

#include "debug.hpp"
#include <cstdint>

namespace BVP
{

/// Тип переменной для отсчетов времени.
typedef uint32_t clockPoint_t;

/** Класс времени.
 *
 *  Все функции и методы статичные.
 */
class TClock
{
    static const clockPoint_t kMax = clockPoint_t(-1);

    /// Конструктор.
    TClock();

public:
    /// Конструктор копирования запрещен.
    TClock(TClock &other) = delete;
    /// Конструктор присваивания запрещен.
    void operator=(const TClock &) = delete;

    /**
     * @brief Устанавливает время одного тика в мс.
     * @param[in] tick Время в мс.
     */
    static void setTickInMs(uint16_t tick)
    {
        Q_ASSERT(tick > 0);
        mTickTimeMs = tick;
    }

    /**
     * @brief Один тик.
     * Функция должны вызываться с пероиодом заданным в setTickInMs.
     */
    static void tick()
    {
        Q_ASSERT(mTickTimeMs > 0);
        mClockCounter += mTickTimeMs;
    }

    /**
     * @brief Возращает текущий отсчет времени.
     * @return Отсчет времени.
     */
    static clockPoint_t getClockPoint() { return mClockCounter; }

    /**
     * @brief Возвращает время прошедшее с указанного отсчета  в мс.
     * @param[in] last Начальный отсчет.
     * @return Прошедшее время в мс.
     */
    static uint16_t getDurationMs(clockPoint_t last)
    {
        clockPoint_t result = mClockCounter;

        if (result > last)
        {
            result -= last;
        }
        else
        {
            result += (kMax - last) + 1;
        }

        Q_ASSERT(result <= UINT16_MAX);

        return static_cast<uint16_t>(result);
    }

    /**
     * @brief Возвращает время прошедшее с указанного отсчета  в секундах.
     * @param[in] last Начальный отсчет.
     * @return Прошедшее время в мс.
     */
    static uint16_t getDurationS(clockPoint_t last)
    {
        clockPoint_t result = mClockCounter;

        if (result > last)
        {
            result -= last;
        }
        else
        {
            result += (kMax - last) + 1;
        }

        Q_ASSERT(result <= UINT16_MAX);

        return static_cast<uint16_t>(result / 1000);
    }

    /// Сброс счетчика времени.
    static void reset() { mClockCounter = 0; }

private:
    /// Счетчик прошедшего времени.
    static clockPoint_t mClockCounter;

    /// Время одного тика в мс.
    static uint16_t mTickTimeMs;
};

}
#endif  // CLOCK_HPP

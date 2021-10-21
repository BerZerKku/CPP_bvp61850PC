#ifndef CLOCK_HPP
#define CLOCK_HPP

#include "debug.hpp"
#include <cstdint>

namespace BVP
{

/// ��� ���������� ��� �������� �������.
typedef uint32_t clockPoint_t;

/** ����� �������.
 *
 *  ��� ������� � ������ ���������.
 */
class TClock
{
    static const clockPoint_t kMax = clockPoint_t(-1);

    /// �����������.
    TClock();

public:
    /// ����������� ����������� ��������.
    TClock(TClock &other) = delete;
    /// ����������� ������������ ��������.
    void operator=(const TClock &) = delete;

    /**
     * @brief ������������� ����� ������ ���� � ��.
     * @param[in] tick ����� � ��.
     */
    static void setTickInMs(uint16_t tick)
    {
        Q_ASSERT(tick > 0);
        mTickTimeMs = tick;
    }

    /**
     * @brief ���� ���.
     * ������� ������ ���������� � ��������� �������� � setTickInMs.
     */
    static void tick()
    {
        Q_ASSERT(mTickTimeMs > 0);
        mClockCounter += mTickTimeMs;
    }

    /**
     * @brief ��������� ������� ������ �������.
     * @return ������ �������.
     */
    static clockPoint_t getClockPoint() { return mClockCounter; }

    /**
     * @brief ���������� ����� ��������� � ���������� �������  � ��.
     * @param[in] last ��������� ������.
     * @return ��������� ����� � ��.
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
     * @brief ���������� ����� ��������� � ���������� �������  � ��������.
     * @param[in] last ��������� ������.
     * @return ��������� ����� � ��.
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

    /// ����� �������� �������.
    static void reset() { mClockCounter = 0; }

private:
    /// ������� ���������� �������.
    static clockPoint_t mClockCounter;

    /// ����� ������ ���� � ��.
    static uint16_t mTickTimeMs;
};

}
#endif  // CLOCK_HPP

/*
 * bvpCommon.cpp
 *
 *  Created on: Aug 20, 2020
 *      Author: bear
 */

#include "bvpCommon.hpp"

#include <cassert>

namespace BVP
{

const uint8_t BvpPkg::c_sop = 0xAB;  // Значение байта "Начало пакета".

// Конструктор.
BvpPkg::BvpPkg(mode_t mode) : mode(mode)
{
    static_assert(c_sop == 0xAB, "Error value of c_sop constant\n");
    static_assert(DATA_LEN == 32, "Wrong value of DATA_LEN constant\n");
    static_assert(sizeof(pkgTx.sop) == 1, "Wrong size of c_sop constant\n");
    static_assert(sizeof(pkg_t::sequence) == 2, "Wrong size of pkg_t::sequence\n");
    static_assert(sizeof(pkg_t::data) == DATA_LEN, "Wrong size of pkg_t::data\n");
    static_assert(sizeof(pkg_t::checksum) == 1, "Wrong size of pkg_t::checksum\n");
    static_assert(sizeof(pkg_t) == 36, "Wrong size of pkg_t");

    pkgRx.sequence = pkgTx.sequence - 1;
}

// Подготовка пакета для передачи.
bool BvpPkg::addDataToPkg(uint8_t data[], uint16_t len)
{
    uint16_t i     = 0;
    bool     state = (len <= DATA_LEN);

    if (len <= DATA_LEN)
    {
        pkgTx.sop = c_sop;

        pkgTx.sequence = sequenceTxGet();

        while (i < len)
        {
            pkgTx.data[i] = data[i];
            i++;
        }

        while (i < DATA_LEN)
        {
            pkgTx.data[i] = 0;
            i++;
        }

        pkgTx.checksum = getChecksum(pkgTx);
    }

    return state;
}

// Вычисляет 8-битную контрольную сумму для массива 8-битных данных.
uint8_t BvpPkg::calcChecksumAvant(const uint8_t buf[], uint16_t len) const
{
    uint8_t checksum = 0;

    while (len > 0)
    {
        checksum += (*buf++);
        len--;
    }

    return checksum;
}


// Вычисляет 8-битную контрольную сумму для массива 8-битных данных.
uint8_t BvpPkg::calcChecksumCompl0(const uint8_t buf[], uint16_t len) const
{
    uint8_t checksum = 0;

    while (len > 0)
    {
        checksum += (*buf++);
        len--;
    }
    checksum = 0x00 - checksum;

    return checksum;
}


// Вычисляет 8-битную контрольную сумму для массива 8-битных данных.
uint8_t BvpPkg::calcChecksumHabr(const uint8_t buf[], uint16_t len) const
{
    uint16_t checksum = 0xABBA;

    while (len > 0)
    {
        checksum += (*buf++) * 211;
        checksum = checksum ^ (checksum >> 8);
        len--;
    }

    return (checksum & 0x00FF);
}


// Проверяет принятые данные.
bool BvpPkg::checkRx() const
{
    bool check = false;

    check = (pkgRx.sop == c_sop);

    if (check)
    {
        check = sequenceRxCheck(pkgRx.sequence);
    }

    if (check)
    {
        check = isChecksum(pkgRx);
    }

    return check;
}


// Вычисляет контрольную сумму для пакета.
uint8_t BvpPkg::getChecksum(const pkg_t &pkg) const
{
    uint16_t len  = sizeof(pkg_t) - sizeof(pkg.checksum);
    uint8_t *data = (uint8_t *) &pkg;

    return calcChecksumHabr(data, len);
}

//
bool BvpPkg::getDataFromPkg(uint8_t data[], uint16_t &len) const
{
    uint16_t i     = 0;
    bool     state = false;

    if (checkRx() && (len < DATA_LEN))
    {
        while ((i < len) && (i < DATA_LEN))
        {
            data[i] = pkgRx.data[i];
            i++;
        }
        state = true;
    }

    len = i;
    return state;
}

// Возвращает указатель на начало пакета для приема и его размер.
uint8_t *BvpPkg::getRxPkg(uint16_t &size) const
{
    size = sizeof(pkgRx);

    return (uint8_t *) &pkgRx;
}

// Возвращает указатель на начало пакета для передачи и его размер.
uint8_t *BvpPkg::getTxPkg(uint16_t &size) const
{
    size = sizeof(pkgTx);

    return (uint8_t *) &pkgTx;
}

// Проверяет контрольную сумму пакета.
bool BvpPkg::isChecksum(const pkg_t &pkg) const
{
    return getChecksum(pkg) == pkg.checksum;
}

// Увеличивает счетчик последовательности.
uint16_t BvpPkg::sequenceModify(uint16_t sequence) const
{
    sequence = sequence + 1;
    if (sequence == 0)
    {
        sequence = 1;
    }

    return sequence;
}

// Проверяет номер последовательности для принятого пакета.
bool BvpPkg::sequenceRxCheck(uint16_t sequence) const
{
    bool check = false;

    switch (mode)
    {
    case MODE_slave: check = true; break;

    case MODE_master: check = (sequence == pkgTx.sequence); break;
    }

    return check;
}

// Возвращает номер последовательности для передаваемого пакета.
uint16_t BvpPkg::sequenceTxGet() const
{
    uint16_t sequence = 0;

    switch (mode)
    {
    case MODE_slave: sequence = pkgRx.sequence; break;

    case MODE_master: sequence = sequenceModify(pkgTx.sequence); break;
    }

    return sequence;
}

} /* namespace BVP */

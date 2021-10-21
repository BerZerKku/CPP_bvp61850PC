#ifndef RINGARRAY_HPP
#define RINGARRAY_HPP

#include "debug.hpp"

//
template<typename T, int size, T defvalue>
class TRingArray
{
    int cnt;
    int len;
    T   buffer[size];

public:
    TRingArray();
    void clear();
    bool add(T value);
    T    get();
    bool isEmpty() const;
    bool isFull() const;
};

//
template<typename T, int size, T defvalue>
TRingArray<T, size, defvalue>::TRingArray()
{
    clear();
}

//
template<typename T, int size, T defvalue>
void TRingArray<T, size, defvalue>::clear()
{
    len = 0;
    cnt = 0;
    for (int i = 0; i < size; i++)
    {
        buffer[i] = defvalue;
    }
}

//
template<typename T, int size, T defvalue>
bool TRingArray<T, size, defvalue>::add(T value)
{
    bool full = len < size;

    if (!isFull())
    {
        buffer[len++] = value;
    }
    else
    {
        QDEBUG("Value adding error. Array is full!");
    }

    return !full;
}

//
template<typename T, int size, T defvalue>
T TRingArray<T, size, defvalue>::get()
{
    T value;

    if (cnt >= len)
    {
        cnt = 0;
    }

    value = buffer[cnt++];

    return value;
}

//
template<typename T, int size, T defvalue>
bool TRingArray<T, size, defvalue>::isEmpty() const
{
    return len == 0;
}

//
template<typename T, int size, T defvalue>
bool TRingArray<T, size, defvalue>::isFull() const
{
    return len == size;
}

#endif  // RINGARRAY_HPP

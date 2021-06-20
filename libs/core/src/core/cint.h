#pragma once

#include "core/ints.h"

#include <cstddef>


namespace euphoria::core
{
    int
    c_sizet_to_int(size_t t);

    constexpr int
    c_float_to_int(float f)
    {
        return static_cast<int>(f);
    }


    constexpr float
    c_int_to_float(int i)
    {
        return static_cast<float>(i);
    }

    int
    c_unsigned_int_to_int(unsigned int i);

    unsigned int
    c_int_to_unsigned_int(int i);

    size_t
    c_int_to_sizet(int i);

    u8
    c_int_to_u8(unsigned int i);

    constexpr u32
    c_bit_signed_to_unsigned(s32 i)
    {
        union { s32 s; u32 u; } c = {};
        c.s = i;
        return c.u;
    }

    constexpr double
    c_float_to_double(float f)
    {
        return static_cast<double>(f);
    }

    constexpr float
    c_double_to_float(double f)
    {
        return static_cast<float>(f);
    }
}

#pragma once

#include "core/ints.h"

#include <cstddef>


namespace euphoria::core
{
    int
    c_sizet_to_int(size_t t);


    int
    c_unsigned_int_to_int(unsigned int i);


    unsigned int
    c_int_to_unsigned_int(int i);


    size_t
    c_int_to_sizet(int i);


    U8
    c_int_to_u8(unsigned int i);


    int
    c_u64_to_int(U64 u);


    // todo(Gustav): deprecate and replace with floor_to_int or ceil_to_int
    constexpr int
    c_float_to_int(float f)
    {
        return static_cast<int>(f);
    }


    constexpr float
    c_sizet_to_float(std::size_t f)
    {
        return static_cast<float>(f);
    }


    constexpr float
    c_int_to_float(int i)
    {
        return static_cast<float>(i);
    }


    constexpr float
    c_u64_to_float(U64 i)
    {
        return static_cast<float>(i);
    }


    constexpr U32
    c_bit_signed_to_unsigned(I32 i)
    {
        union { I32 s; U32 u; } c = {};
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

#pragma once

#include "core/ints.h"

#include <cstddef>


namespace euphoria::core
{
    int
    c_sizet_to_int(size_t t);

    int
    c_float_to_int(float f);

    float
    c_int_to_float(int i);

    int
    c_unsigned_int_to_int(unsigned int i);

    unsigned int
    c_int_to_unsigned_int(int i);

    size_t
    c_int_to_sizet(int i);

    u32
    c_bit_signed_to_unsigned(s32 i);
}

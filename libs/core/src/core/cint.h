#ifndef EUPHORIA_CINT_H
#define EUPHORIA_CINT_H

#include "core/ints.h"

#include <cstddef>


namespace euphoria::core
{
    int
    Csizet_to_int(size_t t);

    int
    Cfloat_to_int(float f);

    float
    Cint_to_float(int i);

    int
    Cunsigned_int_to_int(unsigned int i);

    unsigned int
    Cint_to_unsigned_int(int i);

    size_t
    Cint_to_sizet(int i);

    u32
    Cbit_signed_to_unsigned(s32 i);
}

#endif // EUPHORIA_CINT_H

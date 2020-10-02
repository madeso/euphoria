#ifndef EUPHORIA_CINT_H
#define EUPHORIA_CINT_H

#include "core/ints.h"

#include <cstddef>


namespace euphoria::core
{
    int
    Csizet_to_int(size_t t);

    int
    Cunsigned_int_to_int(unsigned int i);

    unsigned int
    Cint_to_unsigned_int(int i);

    u32
    Cbit_signed_to_unsigned(s32 i);
}  // namespace euphoria::core

#endif  // EUPHORIA_CINT_H

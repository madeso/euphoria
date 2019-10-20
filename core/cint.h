#ifndef EUPHORIA_CINT_H
#define EUPHORIA_CINT_H

#include "core/assert.h"

#include <numeric>

namespace euphoria::core
{
    int
    Csizet_to_int(size_t t);

    int
    Cunsigned_int_to_int(unsigned int i);

    unsigned int
    Cint_to_unsigned_int(int i);
}  // namespace euphoria::core

#endif  // EUPHORIA_CINT_H

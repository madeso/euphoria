#include "core/cint.h"

#include <limits>

#include "core/assert.h"


namespace euphoria::core
{
    int
    Csizet_to_int(size_t t)
    {
        ASSERTX(t <= static_cast<size_t>(std::numeric_limits<int>::max()), t);
        return static_cast<int>(t);
    }


    int
    Cfloat_to_int(float f)
    {
        return static_cast<int>(f);
    }


    float
    Cint_to_float(int i)
    {
        return static_cast<float>(i);
    }


    int
    Cunsigned_int_to_int(unsigned int i)
    {
        ASSERTX(i <= static_cast<unsigned int>(std::numeric_limits<int>::max()), i);
        return static_cast<int>(i);
    }


    unsigned int
    Cint_to_unsigned_int(int i)
    {
        ASSERTX(i >= 0, i);
        return static_cast<unsigned int>(i);
    }


    size_t
    Cint_to_sizet(int i)
    {
        ASSERTX(i >= 0, i);
        return static_cast<size_t>(i);
    }


    u32
    Cbit_signed_to_unsigned(s32 i)
    {
        union { s32 s; u32 u; } c = {};
        c.s = i;
        return c.u;
    }
}  // namespace euphoria::core


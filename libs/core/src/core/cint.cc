#include "core/cint.h"

#include <limits>

#include "core/assert.h"


namespace euphoria::core
{
    int
    c_sizet_to_int(size_t t)
    {
        ASSERTX(t <= static_cast<size_t>(std::numeric_limits<int>::max()), t);
        return static_cast<int>(t);
    }


    int
    c_unsigned_int_to_int(unsigned int i)
    {
        ASSERTX(i <= static_cast<unsigned int>(std::numeric_limits<int>::max()), i);
        return static_cast<int>(i);
    }


    unsigned int
    c_int_to_unsigned_int(int i)
    {
        ASSERTX(i >= 0, i);
        return static_cast<unsigned int>(i);
    }


    size_t
    c_int_to_sizet(int i)
    {
        ASSERTX(i >= 0, i);
        return static_cast<size_t>(i);
    }

    u8
    c_int_to_u8(unsigned int i)
    {
        ASSERTX(i >= 0, i);
        ASSERTX(i <255, i);
        return static_cast<u8>(i);
    }
}


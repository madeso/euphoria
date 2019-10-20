#include "core/cint.h"

namespace euphoria::core
{
    int
    Csizet_to_int(size_t t)
    {
        ASSERT(t <= static_cast<size_t>(std::numeric_limits<int>::max()));
        return static_cast<int>(t);
    }

    int
    Cunsigned_int_to_int(unsigned int i)
    {
        ASSERT(i <= static_cast<unsigned int>(std::numeric_limits<int>::max()));
        return static_cast<int>(i);
    }

    unsigned int
    Cint_to_unsigned_int(int i)
    {
        ASSERT(i >= 0);
        return static_cast<unsigned int>(i);
    }
}  // namespace euphoria::core

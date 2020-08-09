#include "core/range.h"

namespace euphoria::core
{
    const Range<float>&
    R01()
    {
        static const Range<float> r {0, 1};
        return r;
    }

    const Range<float>&
    R11()
    {
        static const Range<float> r {-1, 1};
        return r;
    }

}  // namespace euphoria::core

#ifndef CORE_TABLELAYOUT_H
#define CORE_TABLELAYOUT_H

#include "core/numeric.h"

#include "core/assert.h"
#include <vector>

namespace euphoria::core
{
    // positive = absolute pixel size
    // zero = invalid
    // negative = scaling size
    // if available size < min_size scaling areas will have "zero" and absolute
    // areas will be resized
    template <typename T>
    std::vector<T>
    PerformTableLayout(const std::vector<T>& pieces, T size, T zero = 0)
    {
        ASSERTX(size >= 0, size);
        ASSERT(zero <= 0);

        T min_size         = 0;
        T total_percentage = 0;
        for(T f: pieces)
        {
            if(f > 0)
                min_size += f;
            else
                total_percentage += -f;
        }
        const T size_left   = size - min_size;
        const T fixed_scale = min_size < size ? 1 : size / min_size;

        std::vector<T> ret;
        ret.reserve(pieces.size());
        for(T f: pieces)
        {
            if(f > 0)
            {
                ret.push_back(f * fixed_scale);
            }
            else
            {
                if(size_left <= 0)
                {
                    // no more room, add 0
                    ret.push_back(zero);
                }
                else
                {
                    const T p    = (-f / total_percentage);
                    const T size = p * size_left;
                    ret.push_back(size);
                }
            }
        }
        return ret;
    }

}  // namespace euphoria::core

#endif  // CORE_TABLELAYOUT_H

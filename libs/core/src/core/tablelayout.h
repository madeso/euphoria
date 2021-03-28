#ifndef CORE_TABLELAYOUT_H
#define CORE_TABLELAYOUT_H

#include "core/numeric.h"

#include "core/assert.h"
#include <vector>

namespace euphoria::core
{
    /** Calculates a table layout based on the input layout.
     * Since rows and columns are handled the same, this functions only handled 1d tables.
     *
     * Positive values in the array mean absolute pixel size
     * Zero values are invalid.
     * Negative values indicate the scaling size and the exact ration will be proportional to the rest of the scaling values.
     *
     * If available size < min_size scaling areas will have "zero" and absolute areas will be resized.
     */
    template <typename T>
    std::vector<T>
    PerformTableLayout(const std::vector<T>& pieces, T total_size, T zero = 0)
    {
        ASSERTX(total_size >= 0, total_size);
        ASSERT(zero <= 0);

        T min_size         = 0;
        T total_percentage = 0;
        for(T f: pieces)
        {
            if(f > 0)
            {
                min_size += f;
            }
            else
            {
                total_percentage += -f;
            }
        }
        const T size_left   = total_size - min_size;
        const T fixed_scale = min_size < total_size ? 1 : total_size / min_size;

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

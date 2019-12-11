#ifndef EUPHORIA_CORE_MINMAX_H
#define EUPHORIA_CORE_MINMAX_H

#include <tuple>

#include "core/assert.h"

namespace euphoria::core
{
    template
    <
        typename T,
        typename TContainer,
        typename TMinFunction,
        typename TMaxFunction
    >
    std::tuple<T, T>
    FindMinMax
    (
        const TContainer ts,
        TMinFunction min_function,
        TMaxFunction max_function
    )
    {
        ASSERT(!ts.empty());
        auto it = ts.begin();
        T min_value = *it;
        T max_value = *it;

        it += 1;

        for(; it!=ts.end(); it+=1)
        {
            min_value = min_function(min_value, *it);
            max_value = max_function(max_value, *it);
        }

        return {min_value, max_value};
    }
}

#endif  // EUPHORIA_CORE_MINMAX_H

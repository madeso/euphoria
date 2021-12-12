#pragma once


#include <tuple>

#include "core/assert.h"
#include "core/range.h"

namespace euphoria::core
{
    template
    <
        std::size_t TCount,
        typename T,
        typename TContainer,
        typename TExtract
    >
    std::pair<std::array<T, TCount>, std::array<T, TCount>>
    find_min_max_ranges
    (
        const TContainer& ts,
        TExtract extract
    )
    {
        using A = std::array<T, TCount>;

        ASSERT(!ts.empty());
        auto it = ts.begin();

        A min_value = extract(*it);
        A max_value = extract(*it);

        it += 1;

        for(; it!=ts.end(); it+=1)
        {
            const A a = extract(*it);
            for(size_t sub = 0; sub<TCount; sub += 1)
            {
                min_value[sub] = std::min(min_value[sub], a[sub]);
                max_value[sub] = std::max(max_value[sub], a[sub]);
            }
        }

        return {min_value, max_value};
    }

    template
    <
        typename T,
        typename TContainer,
        typename TExtract
    >
    Range<T>
    find_min_max_range
    (
        const TContainer& ts,
        TExtract extract
    )
    {
        ASSERT(!ts.empty());
        auto it = ts.begin();
        T min_value = extract(*it);
        T max_value = extract(*it);

        it += 1;

        for(; it!=ts.end(); it+=1)
        {
            min_value = std::min(min_value, extract(*it));
            max_value = std::max(max_value, extract(*it));
        }

        return {min_value, max_value};
    }


    template
    <
        typename T,
        typename TContainer,
        typename TMinFunction,
        typename TMaxFunction
    >
    std::tuple<T, T>
    find_min_max
    (
        const TContainer& ts,
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


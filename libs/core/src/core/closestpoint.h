#pragma once


#include <tuple>
#include <optional>

#include "assert/assert.h"

namespace euphoria::core
{
    // todo(Gustav): replace with a better search alghoritm, like k-d tree och oct/quad-tree
    template
    <
        typename TVec,
        typename TData,
        typename TDistanceFunc,
        typename TDistance
    >
    struct ClosestPointCollector
    {
        TDistanceFunc distance_func;
        std::vector<std::tuple<TVec, TData>> points;

        ClosestPointCollector(TDistanceFunc a_distance_func)
            : distance_func(a_distance_func)
        {
        }

        void
        add(const TVec& v, const TData& data)
        {
            points.emplace_back(std::make_tuple(v, data));
        }

        [[nodiscard]] TData
        find_closest(const TVec& v)
        {
            struct DistanceData { TData data; TDistance distance; };
            ASSERT(!points.empty());
            
            std::optional<DistanceData> result;
            for(const auto& [vec, data]: points)
            {
                const auto this_dist = distance_func(v, vec);
                if(result.has_value() == false || result->distance > this_dist)
                {
                    result = {data, this_dist};
                }
            }

            ASSERT(result.has_value());
            return result->data;
        }
    };
}


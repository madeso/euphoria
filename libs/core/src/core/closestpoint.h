#pragma once

#include <vector>
#include <tuple>
#include <optional>

#include "core/assert.h"

namespace euphoria::core
{
    // todo(Gustav): replace with a better search alghoritm, like k-d tree och oct/quad-tree
    template
    <
        typename Vec,
        typename Data,
        typename DistanceFunctionType,
        typename DistanceType
    >
    struct ClosestPointCollector
    {
        DistanceFunctionType distance_function;
        std::vector<std::tuple<Vec, Data>> points;

        ClosestPointCollector(DistanceFunctionType a_distance_function)
            : distance_function(a_distance_function)
        {}

        void
        add(const Vec& v, const Data& data)
        {
            points.emplace_back(std::make_tuple(v, data));
        }

        [[nodiscard]] Data
        find_closest(const Vec& v)
        {
            struct d { Data data; DistanceType distance; };
            ASSERT(!points.empty());
            
            std::optional<d> result;
            for(const auto& [vec, data]: points)
            {
                const auto this_dist = distance_function(v, vec);
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


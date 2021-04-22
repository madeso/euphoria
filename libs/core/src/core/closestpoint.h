#pragma once

#include <vector>
#include <tuple>

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
    struct closest_point
    {
        DistanceFunctionType distance_function;
        std::vector<std::tuple<Vec, Data>> points;

        closest_point(DistanceFunctionType a_distance_function)
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
            ASSERT(!points.empty());
            bool first = true;
            Data closest;
            DistanceType closest_dist;
            for(const auto& [vec, data]: points)
            {
                const auto this_dist = distance_function(v, vec);
                if(first || closest_dist > this_dist)
                {
                    closest = data;
                    closest_dist = this_dist;
                }
                first = false;
            }

            return closest;
        }
    };
}


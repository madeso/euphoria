#pragma once

#include <vector>

#include "core/assert.h"

#include "core/ints.h"
#include "core/vec2.h"
#include "core/range.h"

namespace euphoria::core
{
    /** WEL512 Random Number Generator.
    */
    struct random
    {
        // todo(Gustav): make simpler
        // possible suggestion, rename to RandomGenerator and have a global
        // Random function with overloads in all files instead of here.
        // seperate random namespace where global random functions can live?

        [[nodiscard]] static u32
        generate_time_seed();

        explicit random(u32 seed = generate_time_seed());

        u32
        get_next_integer32();

        u64
        get_next_integer64();

        float
        get_next_float01();

        float
        get_next_float_range11();

        float
        get_next_gaussian_float01();

        float
        get_next_gaussian(float mean, float std_dev);

        float
        get_next_gaussian(float mean, float std_dev, const Range<float>& r);

        bool
        get_next_bool();

        int
        get_next_sign();

        u32 index;
        u32 state[16];
    };

    // move to point class or a circle class?
    vec2f
    PointOnUnitCircle_CenterFocused(random* r);

    vec2f
    PointOnUnitCircle_Uniform(random* r);

    template <typename T>
    T
    get_next_in_range(random* rand, const Range<T>& range)
    {
        return from01(range, rand->get_next_float01());
    }

    template <typename T>
    T
    get_next_range(random* rand, T min, T max)
    {
        return static_cast<T>(min + rand->get_next_float01() * (max - min));
    }

    template <typename T>
    T
    get_next_range(random* rand, T max)
    {
        return get_next_range<T>(rand, 0, max);
    }

    template <typename T>
    const T&
    get_random_item_in_vector(random* r, const std::vector<T>& v)
    {
        const auto size = v.size();
        ASSERT(size > 0);
        if (size == 1)
        {
            return v[0];
        }
        return v[get_next_range(r, size)];
    }
}


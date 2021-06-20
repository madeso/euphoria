#pragma once

#include <vector>
#include "range/v3/view/span.hpp"

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
        // todo(Gustav): make all rng use the same simple api
        // and template the helper functions with user user facing non-templated functions can pick the rng that is needed

        [[nodiscard]] static u32
        generate_time_seed();

        explicit random(u32 seed = generate_time_seed());

        u32
        get_next_u32();

        u64
        get_next_u64();

        float
        get_next_float01();

        bool
        get_next_bool();

        int
        get_next_sign();

        u32 index;
        u32 state[16];
    };

    /*
     All helper functions should be in the style of

     ```
     xxx get_random_xxx(random* rand, ...);
     ```
    */

    // move to point class or a circle class?
    vec2f
    get_random_point_on_unit_circle_center_focused(random* r);

    vec2f
    get_random_point_on_unit_circle_uniform(random* r );

    template <typename T>
    T
    get_random_in_range(random* rand, const range<T>& range)
    {
        return from01(range, rand->get_next_float01());
    }

    template <typename T>
    T
    get_random_in_range(random* rand, T min, T max)
    {
        return static_cast<T>(min + rand->get_next_float01() * (max - min));
    }

    template <typename T>
    T
    get_random_in_range(random* rand, T max)
    {
        return get_random_in_range<T>(rand, 0, max);
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
        return v[get_random_in_range(r, size)];
    }

    template <typename T>
    const T&
    get_random_item_in_vector(random* r, const ranges::span<T>& v)
    {
        const auto size = v.size();
        ASSERT(size > 0);
        if (size == 1)
        {
            return v[0];
        }
        return v[get_random_in_range(r, size)];
    }

    float
    get_random_gaussian_float01(random* rand);

    float
    get_random_gaussian(random* rand, float mean, float std_dev);

    float
    get_random_gaussian(random* rand, float mean, float std_dev, const range<float>& r);
}


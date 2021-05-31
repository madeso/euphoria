#pragma once


#include <limits>

#include "core/ints.h"


namespace euphoria::core
{
    // https://en.wikipedia.org/wiki/Linear_congruential_generator
    template<typename I, I a, I c, I modulus>
    struct random_lcg
    {
        explicit random_lcg(I s) : state(s) {}

        I state;

        // bug: doesn't seem to return the full 0-1 range
        float
        get_next_float01()
        {
            state = (a * state + c) % modulus;
            return static_cast<float>(state)/static_cast<float>(modulus);
        }
    };


    using random_knuth_lcg = random_lcg<u64, std::numeric_limits<u64>::max() /* 2^64 */, 6364136223846793005, 1442695040888963407>;


    // https://en.wikipedia.org/wiki/Xorshift
    template<typename I, I a, I b, I c>
    struct random_xorshift
    {
        explicit random_xorshift(I s) : state(s) {}

        I state;

        I
        get_next_integer()
        {
            /* Algorithm "xor" from p. 4 of Marsaglia, "Xorshift RNGs" */
            I x = state;
            x ^= x << a;
            x ^= x >> b;
            x ^= x << c;
            state = x;
            return state;
        }

        float
        get_next_float01()
        {
            const auto r = get_next_integer();
            const auto max = static_cast<float>(std::numeric_limits<I>::max());
            return static_cast<float>(r) / max;
        }
    };


    using random_xorshift32 = random_xorshift<u32, 13, 17, 5>;
    using random_xorshift64 = random_xorshift<u64, 13, 17, 7>;
}


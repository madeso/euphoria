#pragma once


#include <limits>

#include "core/ints.h"


namespace eu::core
{
    // https://en.wikipedia.org/wiki/Linear_congruential_generator
    template<typename I, I a, I c, I modulus>
    struct RandomLcg
    {
        I state;

        explicit RandomLcg(I s)
            : state(s)
        {
        }

        // bug: doesn't seem to return the full 0-1 range
        float get_next_float01()
        {
            state = (a * state + c) % modulus;
            return static_cast<float>(state)/static_cast<float>(modulus);
        }
    };


    using RandomLnuthLcg = RandomLcg<U64, std::numeric_limits<U64>::max() /* 2^64 */, 6364136223846793005, 1442695040888963407>;


    // https://en.wikipedia.org/wiki/Xorshift
    template<typename I, I a, I b, I c>
    struct RandomXorShift
    {
        I state;

        explicit RandomXorShift(I s)
            : state(s)
        {
        }

        I get_next_integer()
        {
            /* Algorithm "xor" from p. 4 of Marsaglia, "Xorshift RNGs" */
            I x = state;
            x ^= x << a;
            x ^= x >> b;
            x ^= x << c;
            state = x;
            return state;
        }

        float get_next_float01()
        {
            const auto r = get_next_integer();
            const auto max = static_cast<float>(std::numeric_limits<I>::max());
            return static_cast<float>(r) / max;
        }
    };


    using RandomXorShift32 = RandomXorShift<U32, 13, 17, 5>;
    using RandomXorShift64 = RandomXorShift<U64, 13, 17, 7>;
}


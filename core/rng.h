#ifndef EUPHORIA_CORE_RNG_H
#define EUPHORIA_CORE_RNG_H

#include <limits>

#include "core/ints.h"


namespace euphoria::core
{
    // https://en.wikipedia.org/wiki/Linear_congruential_generator
    template<typename I, I a, I c, I modulus>
    struct Lcg
    {
        explicit Lcg(I s) : state(s) {}

        I state;

        // bug: doesn't seem to return the full 0-1 range
        float
        Next()
        {
            state = (a * state + c) % modulus;
            return static_cast<float>(state)/std::numeric_limits<I>::max();
        }
    };


    using KnuthLcg = Lcg<u64, 2^64, 6364136223846793005, 1442695040888963407>;


    // https://en.wikipedia.org/wiki/Xorshift
    template<typename I, I a, I b, I c>
    struct XorShift
    {
        explicit XorShift(I s) : state(s) {}

        I state;

        I
        Nexti()
        {
            /* Algorithm "xor" from p. 4 of Marsaglia, "Xorshift RNGs" */
            I x = state;
            x ^= x << 13;
            x ^= x >> 17;
            x ^= x << 5;
            state = x;
            return state;
        }

        float
        Next()
        {
            const auto r = Nexti();
            return static_cast<float>(r)/std::numeric_limits<I>::max();
        }
    };


    using xorshift32 = XorShift<u32, 13, 17, 5>;
    using xorshift64 = XorShift<u64, 13, 17, 7>;
}

#endif  // EUPHORIA_CORE_RNG_H

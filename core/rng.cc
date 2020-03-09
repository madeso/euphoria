#include "core/rng.h"

namespace euphoria::core
{
    wyhash64::wyhash64(uint64_t s) : state(s)
    {
    }

    float
    wyhash64::Next()
    {
        state += 0x60bee2bee120fc15;
        __uint128_t tmp;
        tmp = (__uint128_t) state * 0xa3b195354a39b70d;
        uint64_t m1 = (tmp >> 64) ^ tmp;
        tmp = (__uint128_t)m1 * 0x1b03738712fad5c9;
        uint64_t m2 = (tmp >> 64) ^ tmp;
        return static_cast<float>(m2)/std::numeric_limits<uint64_t>::max();
    }
}


#include "base/random.h"

#include "base/numeric.h"
#include "base/range.h"

#include <limits>
#include <ctime>

namespace eu
{
    /*
There are much better choices than Mersenne Twister nowadays. Here is a RNG
called WELL512, designed by the designers of Mersenne, developed 10 years later,
and an all around better choice for games. The code is put in the public domain
by Dr. Chris Lomont. He claims this implementation is 40% faster than Mersenne,
does not suffer from poor diffusion and trapping when the state contains many 0
bits, and is clearly a lot simpler code. It has a period of 2^512; a PC takes
over 10^100 years to cycle through the states, so it is large enough.
Here is a paper overviewing PRNGs where I found the WELL512 implementation.
http://www.lomont.org/Math/Papers/2008/Lomont_PRNG_2008.pdf
So - faster, simpler, created by the same designers 10 years later, and produces
better numbers than Mersenne. How can you go wrong? :)
*/
    // http://stackoverflow.com/questions/1046714/what-is-a-good-random-number-generator-for-a-game
    // http://stackoverflow.com/a/1227137

    U32
    Random::generate_time_seed()
    {
        // idea from http://www.eternallyconfuzzled.com/arts/jsw_art_rand.aspx
        time_t now = time(nullptr);

        auto* bytes = reinterpret_cast<unsigned char*>(&now);
        U32 seed = 0;

        for(size_t byte_index = 0; byte_index < sizeof(time_t); byte_index++)
        {
            seed = seed * (std::numeric_limits<unsigned char>::max() + 2U) + bytes[byte_index];
        }

        return seed;
    }

    Random::Random(U32 seed)
        : index(0), state {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}
    {
        for(U32 state_index = 0; state_index < 16; ++state_index)
        {
            state[state_index] = seed * state_index;
        }
    }

    U32
    Random::get_next_u32()
    {
        U32 a = state[index];
        U32 c = state[(index + 13) & 15];
        U32 b = a ^ c ^ (a << 16) ^ (c << 15);
        c = state[(index + 9) & 15];
        c ^= (c >> 11);
        a = state[index] = b ^ c;
        U32 d = a ^ ((a << 5) & 0xDA442D24UL);
        index = (index + 15) & 15;
        a = state[index];
        state[index] = a ^ b ^ d ^ (a << 2) ^ (b << 18) ^ (c << 28);
        return state[index];
    }

    U64
    Random::get_next_u64()
    {
        const U64 a = get_next_u32();
        const U64 b = get_next_u32();
        return (a << 32) | b;
    }

    float
    Random::get_next_float01()
    {
        return static_cast<float>(get_next_u32()) / static_cast<float>(std::numeric_limits<U32>::max());
    }

    float
    get_random_gaussian_float01(Random* rand)
    {
        // gaussian source:
        // https://www.alanzucconi.com/2015/09/16/how-to-sample-from-a-gaussian-distribution/
        float v1 = 0;
        float v2 = 0;
        float s = 0;
        do
        {
            v1 = get_random_in_range(rand, r11);
            v2 = get_random_in_range(rand, r11);
            s = v1 * v1 + v2 * v2;
        } while(s >= 1.0f || is_zero(s));

        s = sqrt((-2.0f * log(s)) / s);

        return v1 * s;
    }

    float
    get_random_gaussian(Random* rand, float mean, float std_dev)
    {
        return mean + get_random_gaussian_float01(rand) * std_dev;
    }

    float
    get_random_gaussian(Random* rand, float mean, float std_dev, const Range<float>& r)
    {
        float x = 0;
        do
        {
            x = get_random_gaussian(rand, mean, std_dev);
        } while(!is_within(r, x));
        return x;
    }

    bool
    Random::get_next_bool()
    {
        return get_next_float01() > 0.5f;
    }

    int
    Random::get_next_sign()
    {
        return get_next_bool() ? 1 : -1;
    }

    vec2f
    get_random_point_on_unit_circle_center_focused(Random* r)
    {
        const auto angle = Angle::from_percent_of_360(r->get_next_float01());
        const auto dist = r->get_next_float01() * 0.5f;

        return vec2f {dist * cos(angle) + 0.5f, dist * sin(angle) + 0.5f};
    }

    vec2f
    get_random_point_on_unit_circle_uniform(Random* r)
    {
        // http://xdpixel.com/random-points-in-a-circle/
        const auto angle = Angle::from_percent_of_360(r->get_next_float01());
        const auto dist = sqrt(r->get_next_float01()) * 0.5f;

        return vec2f {dist * cos(angle) + 0.5f, dist * sin(angle) + 0.5f};
    }
}

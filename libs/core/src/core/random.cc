#include "core/random.h"

#include "core/numeric.h"
#include "core/range.h"

#include <limits>
#include <ctime>

namespace euphoria::core
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

    u32
    random::generate_time_seed()
    {
        // idea from http://www.eternallyconfuzzled.com/arts/jsw_art_rand.aspx
        time_t now = time(nullptr);

        auto* p = reinterpret_cast<unsigned char*>(&now); // NOLINT
        u32 seed = 0;

        for(size_t i = 0; i < sizeof(time_t); i++)
        {
            seed = seed * (std::numeric_limits<unsigned char>::max() + 2U)
                   + p[i];
        }

        return seed;
    }

    random::random(u32 seed)
        : index(0), state {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}
    {
        for(u32 i = 0; i < 16; ++i)
        {
            state[i] = seed * i;
        }
    }

    u32
    random::get_next_integer32()
    {
        u32 a = state[index];
        u32 c = state[(index + 13) & 15];
        u32 b = a ^ c ^ (a << 16) ^ (c << 15);
        c = state[(index + 9) & 15];
        c ^= (c >> 11);
        a = state[index] = b ^ c;
        u32 d = a ^ ((a << 5) & 0xDA442D24UL);
        index = (index + 15) & 15;
        a = state[index];
        state[index] = a ^ b ^ d ^ (a << 2) ^ (b << 18) ^ (c << 28);
        return state[index];
    }

    u64
    random::get_next_integer64()
    {
        const u64 a = get_next_integer32();
        const u64 b = get_next_integer32();
        return (a << 32) | b;
    }

    float
    random::get_next_float01()
    {
        return static_cast<float>(get_next_integer32()) / static_cast<float>(std::numeric_limits<u32>::max());
    }

    float
    random::get_next_float_range11()
    {
        return get_next_float01() * 2.0f - 1.0f;
    }

    float
    random::get_next_gaussian_float01()
    {
        // gaussian source:
        // https://www.alanzucconi.com/2015/09/16/how-to-sample-from-a-gaussian-distribution/
        float v1 = 0;
        float v2 = 0;
        float s = 0;
        do
        {
            v1 = get_next_float_range11();
            v2 = get_next_float_range11();
            s = v1 * v1 + v2 * v2;
        } while(s >= 1.0f || is_zero(s));

        s = sqrt((-2.0f * log(s)) / s);

        return v1 * s;
    }

    float
    random::get_next_gaussian(float mean, float std_dev)
    {
        return mean + get_next_gaussian_float01() * std_dev;
    }

    float
    random::get_next_gaussian(float mean, float std_dev, const Range<float>& r)
    {
        float x = 0;
        do
        {
            x = get_next_gaussian(mean, std_dev);
        } while(!is_within(r, x));
        return x;
    }

    bool
    random::get_next_bool()
    {
        return get_next_float01() > 0.5f;
    }

    int
    random::get_next_sign()
    {
        return get_next_bool() ? 1 : -1;
    }

    vec2f
    PointOnUnitCircle_CenterFocused(random* r)
    {
        const auto angle = angle::from_percent_of_360(r->get_next_float01());
        const auto dist = r->get_next_float01() * 0.5f;

        return vec2f {dist * cos(angle) + 0.5f, dist * sin(angle) + 0.5f};
    }

    vec2f
    PointOnUnitCircle_Uniform(random* r)
    {
        // http://xdpixel.com/random-points-in-a-circle/
        const auto angle = angle::from_percent_of_360(r->get_next_float01());
        const auto dist = sqrt(r->get_next_float01()) * 0.5f;

        return vec2f {dist * cos(angle) + 0.5f, dist * sin(angle) + 0.5f};
    }
}

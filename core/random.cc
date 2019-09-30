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

uint32
Random::TimeSeed()
{
  // idea from http://www.eternallyconfuzzled.com/arts/jsw_art_rand.aspx
  time_t now = time(nullptr);

  auto*  p    = reinterpret_cast<unsigned char*>(&now);  // NOLINT
  uint32 seed = 0;

  for(size_t i = 0; i < sizeof(time_t); i++)
  {
    seed = seed * (std::numeric_limits<unsigned char>().max() + 2U) + p[i];
  }

  return seed;
}

Random::Random(uint32 seed)
    : index_(0)
    , state_{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}
{
  for(uint32 i = 0; i < 16; ++i)
  {
    state_[i] = seed * i;
  }
}

uint32
Random::NextInteger()
{
  uint32 a, b, c, d;
  a = state_[index_];
  c = state_[(index_ + 13) & 15];
  b = a ^ c ^ (a << 16) ^ (c << 15);
  c = state_[(index_ + 9) & 15];
  c ^= (c >> 11);
  a = state_[index_] = b ^ c;
  d                  = a ^ ((a << 5) & 0xDA442D24UL);
  index_             = (index_ + 15) & 15;
  a                  = state_[index_];
  state_[index_]     = a ^ b ^ d ^ (a << 2) ^ (b << 18) ^ (c << 28);
  return state_[index_];
}

float
Random::NextFloat01()
{
  return static_cast<float>(NextInteger()) / std::numeric_limits<uint32>::max();
}

float
Random::NextGaussianFloat01()
{
  // gaussian source:
  // https://www.alanzucconi.com/2015/09/16/how-to-sample-from-a-gaussian-distribution/
  float v1 = 0;
  float v2 = 0;
  float s  = 0;
  do
  {
    v1 = Next(R11());
    v2 = Next(R11());
    s  = v1 * v1 + v2 * v2;
  } while(s >= 1.0f || IsZero(s));

  s = Sqrt((-2.0f * Log(s)) / s);

  return v1 * s;
}

float
Random::NextGaussian(float mean, float std_dev)
{
  return mean + NextGaussianFloat01() * std_dev;
}

float
Random::NextGaussian(float mean, float std_dev, const Range<float>& r)
{
  float x = 0;
  do
  {
    x = NextGaussian(mean, std_dev);
  } while(!IsWithin(r, x));
  return x;
}

bool
Random::NextBool()
{
  return NextFloat01() > 0.5f;
}

int
Random::NextSign()
{
  return NextBool() ? 1 : -1;
}

vec2f
Random::PointOnUnitCircle_CenterFocused()
{
  const auto angle = Angle::FromPercentOf360(NextFloat01());
  const auto dist  = NextFloat01() * 0.5f;

  return vec2f{dist * Cos(angle) + 0.5f, dist * Sin(angle) + 0.5f};
}

vec2f
Random::PointOnUnitCircle_Uniform()
{
  // http://xdpixel.com/random-points-in-a-circle/
  const auto angle = Angle::FromPercentOf360(NextFloat01());
  const auto dist  = Sqrt(NextFloat01()) * 0.5f;

  return vec2f{dist * Cos(angle) + 0.5f, dist * Sin(angle) + 0.5f};
}

}

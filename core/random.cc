#include "core/random.h"

#include <limits>
#include <ctime>

#include "core/numeric.h"

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

uint32 Random::TimeSeed() {
  // idea from http://www.eternallyconfuzzled.com/arts/jsw_art_rand.aspx
  time_t now = time(nullptr);

  auto* p = reinterpret_cast<unsigned char*>(&now);
  uint32 seed = 0;

  for (size_t i = 0; i < sizeof(time_t); i++)
  {
    seed = seed * (std::numeric_limits<unsigned char>().max() + 2U) + p[i];
  }

  return seed;
}

Random::Random(uint32 seed) : index_(0), state_{0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0} {
  for (uint32 i = 0; i < 16; ++i) {
    state_[i] = seed * i;
  }
}

uint32 Random::NextInteger() {
  uint32 a, b, c, d;
  a = state_[index_];
  c = state_[(index_ + 13) & 15];
  b = a ^ c ^ (a << 16) ^ (c << 15);
  c = state_[(index_ + 9) & 15];
  c ^= (c >> 11);
  a = state_[index_] = b ^ c;
  d = a ^ ((a << 5) & 0xDA442D24UL);
  index_ = (index_ + 15) & 15;
  a = state_[index_];
  state_[index_] = a ^ b ^ d ^ (a << 2) ^ (b << 18) ^ (c << 28);
  return state_[index_];
}

float Random::NextFloat01() {
  return static_cast<float>(NextInteger()) / std::numeric_limits<uint32>().max();
}

float Random::NextRange(float min, float max) {
  return From01(min, NextFloat01(), max);
}

float Random::NextRange(float max) {
  return NextRange(0.0f, max);
}

int Random::NextRange(int min, int max) {
  return static_cast<int>(min + NextFloat01() * (max - min));
}

int Random::NextRange(int max) {
  return NextRange(0, max);
}

Color Random::NextColor()
{
  return static_cast<Color>(NextRange(static_cast<int>(Color::MAX_VALUE)));
}

DawnbringerPalette Random::NextDawnbringerPalette()
{
  return static_cast<DawnbringerPalette>(NextRange(static_cast<int>(DawnbringerPalette::MAX_VALUE)));
}

Rgb Random::NextRgb()
{
  return Rgb { NextFloat01(), NextFloat01(), NextFloat01() };
}

Rgb Random::NextGrey()
{
  return Rgb { NextFloat01() };
}

bool Random::NextBool() {
  return NextFloat01() > 0.5f;
}

int Random::NextSign() {
  return NextBool() ? 1 : -1;
}

vec2f Random::NextPoint(const Rectf& rect)
{
  const float x = NextRange(rect.GetWidth());
  const float y = NextRange(rect.GetHeight());
  return rect.GetPosition(vec2f{x, y});
}

vec2i Random::NextPoint(const Recti& rect)
{
  const int x = NextRange(rect.GetWidth());
  const int y = NextRange(rect.GetHeight());
  return rect.GetPosition(vec2i{x, y});
}

PolarCoord Random::NextPolar()
{
  const float az = Random::NextFloat01();
  const float polar = Random::NextFloat01();
  return PolarCoord { az, polar};
}

vec3f::Unit Random::NextUnit3()
{
  return NextPolar().ToCartesian();
}

quatf Random::NextQuatf()
{
  const auto axis = NextUnit3();
  const auto angle = Angle::FromDegrees( Random::NextRange(360.0f) );

  return quatf::FromAxisAngle(AxisAngle::RightHandAround(axis, angle));
}

vec3f Random::NextVec3(const Aabb& extents)
{
  const auto x = NextRange(extents.GetMin().x, extents.GetMax().x);
  const auto y = NextRange(extents.GetMin().y, extents.GetMax().y);
  const auto z = NextRange(extents.GetMin().z, extents.GetMax().z);

  return vec3f{x, y , z};
}

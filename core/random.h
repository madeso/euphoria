#ifndef EUPHORIA_RANDOM_H
#define EUPHORIA_RANDOM_H

#include <vector>

#include <core/assert.h>

#include "core/ints.h"
#include "core/rgb.h"
#include "core/vec2.h"
#include "core/vec3.h"
#include "core/polarcoord.h"
#include "core/quat.h"
#include "core/rect.h"
#include "core/aabb.h"

class Range;

/** WEL512 Random Number Generator.
 */
class Random
{
 public:
  static uint32
  TimeSeed();
  explicit Random(uint32 seed = TimeSeed());

  uint32
  NextInteger();
  float
  NextFloat01();

  float
  NextRange(const Range& range);

  int
  NextRange(int min, int max);
  int
  NextRange(int max);

  unsigned long
  NextRange(unsigned long max);

  Color
  NextColor();
  DawnbringerPalette
  NextDawnbringerPalette();
  Rgb
  NextRgb();
  Rgb
  NextGrey();

  bool
  NextBool();

  int
  NextSign();

  vec2f
  NextPoint(const Rectf& rect);
  vec2i
  NextPoint(const Recti& rect);

  PolarCoord
  NextPolar();
  vec3f::Unit
  NextUnit3();
  quatf
  NextQuatf();

  vec3f
  NextVec3(const Aabb& extents);

  template <typename T>
  T
  Next(const std::vector<T>& v)
  {
    const auto size = v.size();
    ASSERT(size > 0);
    if(size == 1)
    {
      return v[0];
    }
    return v[NextInteger() % size];
  }

 private:
  uint32 index_;
  uint32 state_[16];
};


#endif  // EUPHORIA_RANDOM_H

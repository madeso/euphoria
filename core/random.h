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
  // todo: make simpler
  // possible suggestion, rename to RandomGenerator and have a global
  // Random function with overloads in all files instead of here.
 public:
  static uint32
  TimeSeed();
  explicit Random(uint32 seed = TimeSeed());

  uint32
  NextInteger();

  float
  NextFloat01();

  float
  Next(const Range& range);

  float
  NextGaussianFloat01();

  float
  NextGaussian(float mean, float std_dev);

  float
  NextGaussian(float mean, float std_dev, const Range& r);

  template <typename T>
  int
  NextRange(T min, T max)
  {
    return static_cast<T>(min + NextFloat01() * (max - min));
  }

  template <typename T>
  int
  NextRange(T max)
  {
    return NextRange<T>(0, max);
  }

  // todo: remove color/palette functions in favor of vector of defined colors
  Color
  NextColor();

  Rgb
  NextRgb();
  Rgb
  NextGrey();

  bool
  NextBool();

  int
  NextSign();

  point2f
  NextPoint(const Rectf& rect);
  point2i
  NextPoint(const Recti& rect);

  point2f
  PointOnUnitCircle_CenterFocused();

  point2f
  PointOnUnitCircle_Uniform();

  // todo: check if this is uniform or not
  // http://xdpixel.com/random-points-in-a-circle/
  PolarCoord
  NextPolar();

  unit3f
  NextUnit3();

  quatf
  NextQuatf();

  point3f
  NextVec3(const Aabb& extents);

  template <typename T>
  const T&
  Next(const std::vector<T>& v)
  {
    const auto size = v.size();
    ASSERT(size > 0);
    if(size == 1)
    {
      return v[0];
    }
    // todo: fix this, use next range instead
    return v[NextInteger() % size];
  }

 private:
  uint32 index_;
  uint32 state_[16];
};


#endif  // EUPHORIA_RANDOM_H

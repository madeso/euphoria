#ifndef EUPHORIA_RANDOM_H
#define EUPHORIA_RANDOM_H

#include <vector>

#include <core/assert.h>

#include "core/ints.h"
#include "core/vec2.h"
#include "core/range.h"

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

  template<typename T>
  T
  Next(const Range<T>& range)
  {
    return From01(range, NextFloat01());
  }

  float
  NextGaussianFloat01();

  float
  NextGaussian(float mean, float std_dev);

  float
  NextGaussian(float mean, float std_dev, const Range<float>& r);

  template <typename T>
  T
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

  bool
  NextBool();

  int
  NextSign();

  // move to point class or a circle class?
  point2f
  PointOnUnitCircle_CenterFocused();
  point2f
  PointOnUnitCircle_Uniform();

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
    return v[NextRange(size)];
  }

 private:
  uint32 index_;
  uint32 state_[16];
};


#endif  // EUPHORIA_RANDOM_H

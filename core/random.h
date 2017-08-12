#ifndef EUPHORIA_RANDOM_H
#define EUPHORIA_RANDOM_H

#include "core/ints.h"
#include "core/rgb.h"
#include "core/vec2.h"
#include "core/vec3.h"
#include "core/polarcoord.h"
#include "core/quat.h"
#include "core/rect.h"
#include "core/aabb.h"

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
  NextRange(float min, float max);
  float
  NextRange(float max);

  int
  NextRange(int min, int max);
  int
  NextRange(int max);

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

 private:
  uint32 index_;
  uint32 state_[16];
};


#endif  // EUPHORIA_RANDOM_H

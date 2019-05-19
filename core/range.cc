#include "core/range.h"

#include "core/angle.h"
#include "core/assert.h"

Range::Range(float min, float max)
    : lower_bound(min)
    , upper_bound(max)
{
}

Range::Range(float max)
    : lower_bound(0)
    , upper_bound(max)
{
}

float
From01(const Range& range, float value)
{
  return value * (range.upper_bound - range.lower_bound) + range.lower_bound;
}

float
To01(const Range& range, float value)
{
  return (value - range.lower_bound) / (range.upper_bound - range.lower_bound);
}

float
Get360Angular(const Range& range, float value)
{
  const float half_difference = (range.upper_bound - range.lower_bound) / 2.0f;
  return range.lower_bound + half_difference -
         half_difference * Cos(Angle::FromDegrees(value * 360.0f));
}

float
RemapTo(const Range& from, const Range& to, float value)
{
  return From01(to, To01(from, value));
}

bool
IsWithin(const Range& range, float value)
{
  return value >= range.lower_bound && value <= range.upper_bound;
}

float
KeepWithin(const Range& range, float value)
{
  if(value > range.upper_bound)
  {
    return range.upper_bound;
  }
  if(value < range.lower_bound)
  {
    return range.lower_bound;
  }

  return value;
}

float
Wrap(const Range& range, float value)
{
  const float diff = range.upper_bound - range.lower_bound;
  ASSERT(diff > 0);
  float wrapped = value - range.lower_bound;
  while(wrapped < 0.0f)
  {
    wrapped += diff;
  }
  while(wrapped > diff)
  {
    wrapped -= diff;
  }
  return range.lower_bound + wrapped;
}


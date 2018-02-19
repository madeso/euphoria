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
Range::From01(float value) const
{
  return value * (upper_bound - lower_bound) + lower_bound;
}

float
Range::To01(float value) const
{
  return (value - lower_bound) / (upper_bound - lower_bound);
}

float
Range::Get360Angular(float value) const
{
  const float half_difference = (upper_bound - lower_bound) / 2.0f;
  return lower_bound + half_difference -
         half_difference * Cos(Angle::FromDegrees(value * 360.0f));
}

float
Range::RemapTo(const Range& other, float value) const
{
  return other.From01(To01(value));
}

bool
Range::IsWithin(float value) const
{
  return value >= lower_bound && value <= upper_bound;
}

float
Range::KeepWithin(float value) const
{
  if(value > upper_bound)
  {
    return upper_bound;
  }
  if(value < lower_bound)
  {
    return lower_bound;
  }

  return value;
}

float
Range::Wrap(float value) const
{
  const float diff = upper_bound - lower_bound;
  ASSERT(diff > 0);
  float wrapped = value - lower_bound;
  while(wrapped < 0.0f)
  {
    wrapped += diff;
  }
  while(wrapped > diff)
  {
    wrapped -= diff;
  }
  return lower_bound + wrapped;
}

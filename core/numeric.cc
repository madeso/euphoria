#include "core/numeric.h"

#include <cmath>
#include "core/assert.h"

#include "core/angle.h"

bool
IsEqual(float lhs, float rhs)
{
  return IsZero(lhs - rhs);
}

bool
IsZero(float r)
{
  const float epsilon = 0.0001f;
  return IsWithin(-epsilon, r, epsilon);
}

float
ZeroOrValue(float r)
{
  if(IsZero(r))
  {
    return 0;
  }

  return r;
}

const int
Sign(const float r)
{
  if(r >= 0.0f)
  {
    return 1;
  }

  return -1;
}

const float
Lerp(const float f, float scale, const float t)
{
  return f + (t - f) * scale;
}

const float
Curve(const float new_value, const float old_value, const float smoothing_value)
{
  const int   sign = Sign(old_value - new_value);
  const float slip = (old_value - new_value) / smoothing_value;
  const float val  = old_value - slip;
  if(sign != Sign(val - new_value))
  {
    return new_value;
  }


  return val;
}

const float
Square(const float r)
{
  return r * r;
}

const float
Sqrt(const float r)
{
  Assert(r >= 0 && "input must be bigger than 0");
  return std::sqrt(r);
}

const float
Abs(const float r)
{
  if(r < 0.0f)
  {
    return -r;
  }


  return r;
}

const float
Min(const float lhs, const float rhs)
{
  if(lhs < rhs)
  {
    return lhs;
  }


  return rhs;
}

const int
Min(const int lhs, const int rhs)
{
  if(lhs < rhs)
  {
    return lhs;
  }


  return rhs;
}

const float
Max(const float lhs, const float rhs)
{
  if(lhs > rhs)
  {
    return lhs;
  }


  return rhs;
}

const int
Max(const int lhs, const int rhs)
{
  if(lhs > rhs)
  {
    return lhs;
  }


  return rhs;
}


const float
To01(const float lower_bound, const float value, const float upper_bound)
{
  return (value - lower_bound) / (upper_bound - lower_bound);
}

const float
From01(const float lower_bound, const float value, const float upper_bound)
{
  return value * (upper_bound - lower_bound) + lower_bound;
}

const float
Remap(const float old_lower_bound, const float old_upper_bound,
      const float value, const float new_lower_bound,
      const float new_upper_bound)
{
  return From01(new_lower_bound, To01(old_lower_bound, value, old_upper_bound),
                new_upper_bound);
}

const float
Get360Angular(const float min, const float value, const float max)
{
  const float half_difference = (max - min) / 2.0f;
  return min + half_difference -
         half_difference * Cos(Angle::FromDegrees(value * 360.0f));
}

const float
KeepWithin(const float min, const float v, const float max)
{
  if(v > max)
  {
    return max;
  }
  if(v < min)
  {
    return min;
  }

  return v;
}

const bool
IsWithin(const float min, const float c, const float max)
{
  return c > min && c < max;
}

const bool
IsWithinInclusive(const float min, const float c, const float max)
{
  return c >= min && c <= max;
}

const bool
IsWithinInclusivei(const int min, const int c, const int max)
{
  return c >= min && c <= max;
}

const float
Wrap(const float min, const float v, const float max)
{
  const float diff = max - min;
  Assert(diff > 0);
  float value = v - min;
  while(value < 0.0f)
  {
    value += diff;
  }
  while(value > diff)
  {
    value -= diff;
  }
  return min + value;
}

float
PingPong01(float v)
{
  int iter = 0;

  while(!IsWithinInclusive(0, v, 1) && iter < 100)
  {
    iter += 1;
    if(v > 1)
    {
      v = 1 - (v - 1);
    }
    if(v < 0)
    {
      v = -v;
    }
  }

  return v;
}

float
PingPong(float min, float v, float max)
{
  return From01(min, PingPong01(To01(min, v, max)), max);
}

const int
Wrapi(const int min, const int v, const int max)
{
  const int diff  = max - min + 1;
  int       value = v;
  while(value < min)
  {
    value += diff;
  }
  while(value > max)
  {
    value -= diff;
  }
  return value;
}

const int
IncrementAndWrap(const float min, float* current, const float change,
                 const float max)
{
  const float diff  = max - min;
  float       value = *current + change;
  int         wraps = 0;
  while(value < min)
  {
    value += diff;
    --wraps;
  }
  while(value > max)
  {
    value -= diff;
    ++wraps;
  }
  *current = value;
  return wraps;
}

const int
IncrementAndWrapi(const int min, int* current, const int change, const int max)
{
  const int diff  = max - min + 1;
  int       value = *current + change;
  int       wraps = 0;
  while(value < min)
  {
    value += diff;
    --wraps;
  }
  while(value > max)
  {
    value -= diff;
    ++wraps;
  }
  *current = value;
  return wraps;
}

namespace  // internal
{
  float
  LowerBound(float num, float gran)
  {
    return std::floor(num / gran) * gran;
  }

  float
  UpperBound(float num, float gran)
  {
    return std::ceil(num / gran) * gran;
  }
}  // namespace

const float
Round(const float num, const float gran)
{
  const float lower = LowerBound(num, gran);
  const float upper = UpperBound(num, gran);
  const float percent =
      (num - lower) / gran;  // 1.0 is upper, 0.0 is lower, 0.25 is 25% between
                             // lower and upper, moving from lower to upper

  if(percent >= 0.5)
  {
    return upper;
  }


  return lower;
}

const float
Pi()
{
  return 3.1415926535897932384626433832795f;
}

const float
HalfPi()
{
  return Pi() / 2.0f;
}

const bool
TrueValue()
{
  return true;
}

const bool
FalseValue()
{
  return false;
}

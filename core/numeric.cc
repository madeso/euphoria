#include "core/numeric.h"

#include "core/assert.h"
#include "core/angle.h"

#include <cmath>


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

float
Floor(float v)
{
  return std::floor(v);
}

float
Ceil(float v)
{
  return std::ceil(v);
}

int
Floori(float v)
{
  return std::floor(v);
}

int
Ceili(float v)
{
  return std::ceil(v);
}

const int
Sign(float r)
{
  if(r >= 0.0f)
  {
    return 1;
  }

  return -1;
}

float
Lerp(float f, float scale, float t)
{
  return f + (t - f) * scale;
}

float
Curve(float new_value, float old_value, float smoothing_value)
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

float
Square(float r)
{
  return r * r;
}

float
Sqrt(float r)
{
  ASSERT(r >= 0 && "input must be bigger than 0");
  return std::sqrt(r);
}

float
Abs(float r)
{
  if(r < 0.0f)
  {
    return -r;
  }


  return r;
}

float
Min(float lhs, float rhs)
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

float
Max(float lhs, float rhs)
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


float
To01(float lower_bound, float value, float upper_bound)
{
  return (value - lower_bound) / (upper_bound - lower_bound);
}

float
From01(float lower_bound, float value, float upper_bound)
{
  return value * (upper_bound - lower_bound) + lower_bound;
}

float
Remap(
    float old_lower_bound,
    float old_upper_bound,
    float value,
    float new_lower_bound,
    float new_upper_bound)
{
  return From01(
      new_lower_bound,
      To01(old_lower_bound, value, old_upper_bound),
      new_upper_bound);
}

float
Get360Angular(float min, float value, float max)
{
  const float half_difference = (max - min) / 2.0f;
  return min + half_difference -
         half_difference * Cos(Angle::FromDegrees(value * 360.0f));
}

float
KeepWithin(float min, float v, float max)
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
IsWithin(float min, float c, float max)
{
  return c > min && c < max;
}

const bool
IsWithinInclusive(float min, float c, float max)
{
  return c >= min && c <= max;
}

const bool
IsWithinInclusivei(const int min, const int c, const int max)
{
  return c >= min && c <= max;
}

float
Wrap(float min, float v, float max)
{
  const float diff = max - min;
  ASSERT(diff > 0);
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
IncrementAndWrap(
    float min, float* current, float change, float max)
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

float
Round(float num, float gran)
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

float
Pi()
{
  return 3.1415926535897932384626433832795f;
}

float
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

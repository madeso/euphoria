#include "spacetyper/numeric.h"

#include <cmath>
#include <cassert>

#include "spacetyper/angle.h"

bool IsEqual(float lhs, float rhs)
{
  return IsZero(lhs - rhs);
}

bool IsZero(float r)
{
  const float kRange = 0.0001f;
  return IsWithin(-kRange, r, kRange);
}

float ZeroOrValue(float r)
{
  if(IsZero(r))
  {
    return 0;
  }
  else
  {
    return r;
  }
}

const int Sign(const float r)
{
  if(r >= 0.0f)
  {
    return 1;
  }
  else
  {
    return -1;
  }
}

const float Lerp(const float f, float scale, const float t)
{
  return f + (t - f) * scale;
}

const float Curve(const float newValue, const float oldValue, const float smoothingValue)
{
  const int sign = Sign(oldValue - newValue);
  const float slip = (oldValue - newValue) / smoothingValue;
  const float val = oldValue - slip;
  if(sign != Sign(val - newValue))
  {
    return newValue;
  }
  else
  {
    return val;
  }
}

const float Square(const float r)
{
  return r * r;
}

const float Sqrt(const float r)
{
  assert(r >= 0 && "input must be bigger than 0");
  return std::sqrt(r);
}

const float Abs(const float r)
{
  if(r < 0.0f)
  {
    return -r;
  }
  else
  {
    return r;
  }
}

const float Min(const float lhs, const float rhs)
{
  if(lhs < rhs)
  {
    return lhs;
  }
  else
  {
    return rhs;
  }
}

const float Max(const float lhs, const float rhs)
{
  if(lhs > rhs)
  {
    return lhs;
  }
  else
  {
    return rhs;
  }
}

const float To01(const float L, const float v, const float U)
{
  return (v - L) / (U - L);
}

const float From01(const float L, const float v, const float U)
{
  return v * (U - L) + L;
}

const float Remap(const float ol, const float ou, const float v, const float nl, const float nu)
{
  return From01(nl, To01(ol, v, ou), nu);
}

const float Get360Angular(const float min, const float value, const float max)
{
  const float halfdiff = (max - min) / 2.0f;
  return min + halfdiff - halfdiff * Cos(Angle::FromDegrees(value * 360.0f));
}

const float KeepWithin(const float min, const float v, const float max)
{
  if(v > max)
  {
    return max;
  }
  else if(v < min)
  {
    return min;
  }
  else
  {
    return v;
  }
}

const bool IsWithin(const float min, const float c, const float max)
{
  return c > min && c < max;
}

const bool IsWithinInclusive(const float min, const float c, const float max)
{
  return c >= min && c <= max;
}

const float Wrap(const float min, const float v, const float max)
{
  const float diff = max - min;
  assert(diff > 0);
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

const int Wrapi(const int min, const int v, const int max)
{
  const int diff = max - min + 1;
  int value = v;
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

const int IncrementAndWrap(const float min, float* current, const float change, const float max)
{
  const float diff = max - min;
  float value = *current + change;
  int wraps = 0;
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

const int IncrementAndWrapi(const int min, int* current, const int change, const int max)
{
  const int diff = max - min + 1;
  int value = *current + change;
  int wraps = 0;
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

namespace // internal
{
float LowerBound(float num, float gran)
{
  return std::floor(num / gran) * gran;
}

float UpperBound(float num, float gran)
{
  return std::ceil(num / gran) * gran;
}
}

const float Round(const float num, const float gran)
{
  const float lower = LowerBound(num, gran);
  const float upper = UpperBound(num, gran);
  const float percent = (num - lower) / gran; // 1.0 is upper, 0.0 is lower, 0.25 is 25% between lower and upper, moving from lower to upper

  if(percent >= 0.5)
  {
    return upper;
  }
  else
  {
    return lower;
  }
}

const float Pi()
{
  return 3.1415926535897932384626433832795f;
}

const float HalfPi()
{
  return Pi() / 2.0f;
}

const bool kTrue()
{
  return true;
}

const bool kFalse()
{
  return false;
}

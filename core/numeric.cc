#include "core/numeric.h"

#include "core/assert.h"
#include "core/angle.h"
#include "core/range.h"

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
  return Range(-epsilon, epsilon).IsWithin(r);
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

int
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
Log(float r)
{
  return std::log(r);
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

int
Min(int lhs, int rhs)
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

int
Max(int lhs, int rhs)
{
  if(lhs > rhs)
  {
    return lhs;
  }


  return rhs;
}

bool
IsWithinInclusivei(int min, int c, int max)
{
  return c >= min && c <= max;
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

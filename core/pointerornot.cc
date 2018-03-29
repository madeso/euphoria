#include "core/pointerornot.h"

#include <cmath>

template <>
bool
IsValid(int)
{
  return true;
}

template <>
bool
IsValid(float f)
{
  return !std::isnan(f);
}

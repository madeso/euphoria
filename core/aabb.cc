#include "core/aabb.h"

#include "core/assert.h"
#include "core/numeric.h"

Aabb::Aabb(const vec3f &min, const vec3f &max)
  : min_(min)
  , max_(max)
{
  Assert(min.x < max.x);
  Assert(min.y < max.y);
  Assert(min.z < max.z);
}

const vec3f& Aabb::GetMin() const
{
  return min_;
}

const vec3f& Aabb::GetMax() const
{
  return max_;
}

vec3f Aabb::Wrap(const vec3f& vec) const
{
#define COMP(C) const auto C = ::Wrap(min_.C, vec.C, max_.C)
  COMP(x);
  COMP(y);
  COMP(z);
#undef COMP
  return vec3f{x, y ,z};
}

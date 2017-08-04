#include "core/aabb.h"

#include "core/assert.h"

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

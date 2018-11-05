#include "core/aabb.h"

#include "core/assert.h"
#include "core/numeric.h"
#include "core/range.h"

Aabb::Aabb(const point3f& amin, const point3f& amax)
    : min(amin)
    , max(amax)
{
  ASSERT(IsValid());
}

const point3f&
Aabb::GetMin() const
{
  ASSERT(IsValid());
  return min;
}

const point3f&
Aabb::GetMax() const
{
  ASSERT(IsValid());
  return max;
}

point3f
Aabb::Wrap(const point3f& vec) const
{
  ASSERT(IsValid());
#define COMP(C) const auto C = Range{min.C, max.C}.Wrap(vec.C)
  COMP(x);
  COMP(y);
  COMP(z);
#undef COMP
  return point3f{x, y, z};
}

point3f
Min(const point3f& lhs, const point3f& rhs)
{
#define M(var) Min(lhs.var, rhs.var)
  return point3f{M(x), M(y), M(z)};
#undef M
}

point3f
Max(const point3f& lhs, const point3f& rhs)
{
#define M(var) Max(lhs.var, rhs.var)
  return point3f{M(x), M(y), M(z)};
#undef M
}

void
Aabb::Extend(const point3f& vec)
{
  ASSERT(IsValid());
  min = Min(min, vec);
  max = Max(max, vec);
}

void
Aabb::Extend(const Aabb& aabb)
{
  ASSERT(IsValid());
  min = Min(min, aabb.min);
  max = Max(max, aabb.max);
}

Aabb
Aabb::Empty()
{
  return Aabb{point3f::Origo(), point3f::Origo()};
}

vec3f
Aabb::GetSize() const
{
  ASSERT(IsValid());
  return vec3f::FromTo(min, max);
}

bool
Aabb::IsValid() const
{
#define M(var) min.var <= max.var
  return M(x) && M(y) && M(z);
#undef M
}

vec3f
Aabb::GetOffset() const
{
  ASSERT(IsValid());
  return vec3f::FromTo(point3f::Origo(), min);
}

#include "core/aabb.h"

#include "core/assert.h"
#include "core/numeric.h"

Aabb::Aabb(const vec3f& min, const vec3f& max)
    : min_(min)
    , max_(max)
{
  ASSERT(IsValid());
}

const vec3f&
Aabb::GetMin() const
{
  ASSERT(IsValid());
  return min_;
}

const vec3f&
Aabb::GetMax() const
{
  ASSERT(IsValid());
  return max_;
}

vec3f
Aabb::Wrap(const vec3f& vec) const
{
  ASSERT(IsValid());
#define COMP(C) const auto C = ::Wrap(min_.C, vec.C, max_.C)
  COMP(x);
  COMP(y);
  COMP(z);
#undef COMP
  return vec3f{x, y, z};
}

vec3f
Min(const vec3f& lhs, const vec3f& rhs)
{
#define M(var) Min(lhs.var, rhs.var)
  return vec3f{M(x), M(y), M(z)};
#undef M
}

vec3f
Max(const vec3f& lhs, const vec3f& rhs)
{
#define M(var) Max(lhs.var, rhs.var)
  return vec3f{M(x), M(y), M(z)};
#undef M
}

void
Aabb::Extend(const vec3f& vec)
{
  ASSERT(IsValid());
  min_ = Min(min_, vec);
  max_ = Max(max_, vec);
}

void
Aabb::Extend(const Aabb& aabb)
{
  ASSERT(IsValid());
  min_ = Min(min_, aabb.min_);
  max_ = Max(max_, aabb.max_);
}

Aabb
Aabb::Empty()
{
  return Aabb{vec3f::Origo(), vec3f::Origo()};
}

vec3f
Aabb::GetSize() const
{
  ASSERT(IsValid());
  return max_ - min_;
}

bool
Aabb::IsValid() const
{
#define M(var) min_.var <= max_.var
  return M(x) && M(y) && M(z);
#undef M
}

vec3f
Aabb::GetOffset() const
{
  ASSERT(IsValid());
  return -min_;
}

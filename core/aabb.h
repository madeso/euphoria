#ifndef EUPHORIA_AABB_H
#define EUPHORIA_AABB_H

#include "core/vec3.h"

class Aabb
{
 public:
  Aabb(const point3f& amin, const point3f& amax);

  point3f
  Wrap(const point3f& vec) const;

  void
  Extend(const point3f& vec);

  void
  Extend(const Aabb& aabb);

  static Aabb
  Empty();

  Vec3f
  GetSize() const;

  bool
  IsValid() const;

  Vec3f
  GetOffset() const;

  const point3f&
  GetMin() const;

  const point3f&
  GetMax() const;

  point3f min;
  point3f max;
};

template <typename Stream>
Stream&
operator<<(Stream& s, const Aabb& a)
{
  s << "{" << a.GetMin() << ", " << a.GetMax() << "}";
  return s;
}


#endif  // EUPHORIA_AABB_H

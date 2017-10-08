#ifndef EUPHORIA_AABB_H
#define EUPHORIA_AABB_H

#include "core/vec3.h"

class Aabb
{
 public:
  Aabb(const vec3f& min, const vec3f& max);

  const vec3f&
  GetMin() const;
  const vec3f&
  GetMax() const;

  vec3f
  Wrap(const vec3f& vec) const;

  void
  Extend(const vec3f& vec);

  void
  Extend(const Aabb& aabb);

  static Aabb
  Empty();

  vec3f
  GetSize() const;

  bool
  IsValid() const;

  vec3f
  GetOffset() const;

  vec3f min_;
  vec3f max_;
};


#endif  // EUPHORIA_AABB_H

#ifndef EUPHORIA_AABB_H
#define EUPHORIA_AABB_H

#include "core/vec3.h"

class Aabb {
 public:
  Aabb(const vec3f& min, const vec3f& max);

  const vec3f& GetMin() const;
  const vec3f& GetMax() const;

  vec3f min_;
  vec3f max_;
};


#endif //EUPHORIA_AABB_H

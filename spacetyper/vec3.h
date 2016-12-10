#ifndef SPACETYPER_VEC3_H
#define SPACETYPER_VEC3_H

#include "spacetyper/vec2.h"

template<typename T>
class vec3 {
 public:
  T x;
  T y;
  T z;

  explicit vec3(const T& a) : x(a), y(a), z(a) {}
  vec3(const T& ax, const T& ay, const T& az) : x(ax), y(ay), z(az) {}
  vec3(const vec2<T>& a, const T& az) : x(a.x), y(a.y), z(az) {}
};

typedef vec3<float> vec3f;
typedef vec3<int> vec3i;

#endif  // SPACETYPER_VEC3_H
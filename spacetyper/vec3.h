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

  static vec3<T> XAxis() {
    return vec3(1, 0, 0);
  }
  static vec3<T> YAxis() {
    return vec3(0, 1, 0);
  }
  static vec3<T> ZAxis() {
    return vec3(0, 0, 1);
  }
};


template <typename T>
bool operator==(const vec3<T>& lhs, const vec3<T>& rhs) {
  return
      lhs.x == rhs.x
      && lhs.y==rhs.y
      && lhs.z==rhs.z
      ;
}

template <typename T>
std::ostream& operator<<(std::ostream& stream, const vec3<T>& v)
{
  return stream << "(" << v.x << ", " << v.y << ", " << v.z << ")";
}


typedef vec3<float> vec3f;
typedef vec3<int> vec3i;

#endif  // SPACETYPER_VEC3_H
#ifndef CORE_VEC3_H
#define CORE_VEC3_H

#include "core/vec2.h"
#include <ostream>

template<typename T>
class vec3 {
 public:
  typedef vec3<T> Vec;
  T x;
  T y;
  T z;

  explicit vec3(const T& a) : x(a), y(a), z(a) {}
  vec3(const T& ax, const T& ay, const T& az) : x(ax), y(ay), z(az) {}
  vec3(const vec2<T>& a, const T& az) : x(a.x), y(a.y), z(az) {}
  static Vec FromTo(const Vec& from, const Vec& to) {
    return to - from;
  }

  static Vec XAxis() {
    return vec3(1, 0, 0);
  }
  static Vec YAxis() {
    return vec3(0, 1, 0);
  }
  static Vec ZAxis() {
    return vec3(0, 0, 1);
  }

  static Vec Up() {return YAxis();}
  static Vec Down() {return -YAxis();}
  static Vec Right() {return XAxis();}
  static Vec Left() {return -XAxis();}
  static Vec In() {return -ZAxis();}
  static Vec Out() {return ZAxis();}

  void operator+=(const Vec& rhs) {
    x += rhs.x;
    y += rhs.y;
    z += rhs.z;
  }

  void operator-=(const Vec& rhs) {
    x -= rhs.x;
    y -= rhs.y;
    z -= rhs.z;
  }

  void operator/=(const T& rhs) {
    x /= rhs;
    y /= rhs;
    z /= rhs;
  }

  void operator*=(const T& rhs) {
    x *= rhs;
    y *= rhs;
    z *= rhs;
  }

  Vec operator-() const {
    return Vec(-x, -y, -z);
  }

  T GetLengthSquared() const {
    return x*x + y*y + z*z;
  }

  T GetLength() const {
    return sqrt(GetLengthSquared());
  }

  void Normalize() {
    *this /= GetLength();
  }

  Vec GetNormalized() const {
    Vec r = *this;
    r.Normalize();
    return r;
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

template<typename T>
T dot(const vec3<T>& lhs, const vec3<T>& rhs) {
  return   lhs.x * rhs.x
         + lhs.y * rhs.y
         + lhs.z * rhs.z;
}

template<typename T>
vec3<T> cross(const vec3<T>& v, const vec3<T> u) {
  return vec3<T>(
      (v.y * u.z) - (v.z * u.y),
      (v.z * u.x) - (v.x * u.z),
      (v.x * u.y) - (v.y * u.x)
  );
}

template <typename T>
std::ostream& operator<<(std::ostream& stream, const vec3<T>& v)
{
  return stream << "(" << v.x << ", " << v.y << ", " << v.z << ")";
}

template<typename T>
vec3<T> operator+(const vec3<T>& lhs, const vec3<T>& rhs) {
  vec3<T> r = lhs;
  r += rhs;
  return r;
}

template<typename T>
vec3<T> operator-(const vec3<T>& lhs, const vec3<T>& rhs) {
  vec3<T> r = lhs;
  r -= rhs;
  return r;
}

template<typename T>
vec3<T> operator*(T lhs, const vec3<T>& rhs) {
  vec3<T> r = rhs;
  r *= lhs;
  return r;
}

template<typename T>
vec3<T> operator*(const vec3<T>& lhs, T rhs) {
  vec3<T> r = lhs;
  r *= rhs;
  return r;
}

template<typename T>
vec3<T> operator/(const vec3<T>& lhs, T rhs) {
  vec3<T> r = lhs;
  r /= rhs;
  return r;
}

typedef vec3<float> vec3f;
typedef vec3<int> vec3i;

#endif  // CORE_VEC3_H
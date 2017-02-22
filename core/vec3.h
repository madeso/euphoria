#ifndef CORE_VEC3_H
#define CORE_VEC3_H

#include "core/vec2.h"
#include "core/numeric.h"
#include <ostream>
#include "core/assert.h"

template<typename T>
class vec3;

template<typename T>
class unit;

template<typename T>
class vec3 {
 public:
  typedef vec3<T> Vec;
  typedef unit<T> Unit;
  T x;
  T y;
  T z;

  explicit vec3(const T& a) : x(a), y(a), z(a) {}
  vec3(const T& ax, const T& ay, const T& az) : x(ax), y(ay), z(az) {}
  vec3(const vec2<T>& a, const T& az) : x(a.x), y(a.y), z(az) {}
  static Vec FromTo(const Vec& from, const Vec& to) {
    return to - from;
  }

  static Unit XAxis();
  static Unit YAxis();
  static Unit ZAxis();

  static Unit Up();
  static Unit Down();
  static Unit Right();
  static Unit Left();
  static Unit In();
  static Unit Out();

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
    const T l2 = GetLengthSquared();
    if( IsEqual(l2, 0) ) {
      *this = Up();
    }
    else {
      *this /= Sqrt(l2);
    }
  }

  Unit GetNormalized() const;
};

////////////////////////////////////////////////////////////////////////////////

template <typename T>
class unit : public vec3<T> {
 public:
  typedef unit<T> Unit;
  bool IsValid() const {
    return ::IsEqual(vec3<T>::GetLengthSquared(), 1);
  }
  Unit operator-() const {
    return Unit(-vec3<T>::x, -vec3<T>::y, -vec3<T>::z);
  }
 private:
  unit(T x, T y, T z) : vec3<T>(x, y, z) { Assert(IsValid()); }
  unit(const vec3<T>& o) : vec3<T>(o) { Assert(IsValid()); }
  friend class vec3<T>;
};

template<typename T>
unit<T> vec3<T>::XAxis() {
  return Unit(1, 0, 0);
}

template<typename T>
unit<T> vec3<T>::YAxis() {
  return Unit(0, 1, 0);
}

template<typename T>
unit<T> vec3<T>::ZAxis() {
  return Unit(0, 0, 1);
}

template<typename T>
unit<T> vec3<T>::Up() {return YAxis();}

template<typename T>
unit<T> vec3<T>::Down() {return -YAxis();}

template<typename T>
unit<T> vec3<T>::Right() {return XAxis();}

template<typename T>
unit<T> vec3<T>::Left() {return -XAxis();}

template<typename T>
unit<T> vec3<T>::In() {return -ZAxis();}

template<typename T>
unit<T> vec3<T>::Out() {return ZAxis();}

template<typename T>
unit<T> vec3<T>::GetNormalized() const {
  Vec r = *this;
  r.Normalize();
  return Unit(r);
}

////////////////////////////////////////////////////////////////////////////////

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

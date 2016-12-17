#ifndef CORE_VEC2_H
#define CORE_VEC2_H

#include <cmath>

template<typename T>
class vec2 {
 public:
  T x;
  T y;

  explicit vec2(const T& a) : x(a), y(a) {}
  vec2(const T& ax, const T& ay) : x(ax), y(ay) {}

  void operator+=(const vec2<T>& rhs) {
    x += rhs.x;
    y += rhs.y;
  }

  void operator-=(const vec2<T>& rhs) {
    x -= rhs.x;
    y -= rhs.y;
  }

  void operator/=(const T& rhs) {
    x /= rhs;
    y /= rhs;
  }

  void operator*=(const T& rhs) {
    x *= rhs;
    y *= rhs;
  }

  T GetLengthSquared() const {
    return x*x + y*y;
  }

  T GetLength() const {
    return sqrt(GetLengthSquared());
  }

  void Normalize() {
    *this /= GetLength();
  }

  vec2<T> GetNormalized() const {
    vec2<T> r = *this;
    r.Normalize();
    return r;
  }
};

// operators

template<typename T>
vec2<T> operator+(const vec2<T>& lhs, const vec2<T>& rhs) {
  vec2<T> r = lhs;
  r += rhs;
  return r;
}

template<typename T>
vec2<T> operator-(const vec2<T>& lhs, const vec2<T>& rhs) {
  vec2<T> r = lhs;
  r -= rhs;
  return r;
}

template<typename T>
vec2<T> operator/(const vec2<T>& lhs, const T& rhs) {
  vec2<T> r = lhs;
  r /= rhs;
  return r;
}

template<typename T>
vec2<T> operator*(const vec2<T>& lhs, const T& rhs) {
  vec2<T> r = lhs;
  r *= rhs;
  return r;
}

template<typename T>
vec2<T> operator*(const T& lhs, const vec2<T>& rhs) {
  vec2<T> r = rhs;
  r *= lhs;
  return r;
}

template<typename T>
bool operator==(const vec2<T>& lhs, const vec2<T>& rhs) {
  return lhs.x == rhs.x && lhs.y==rhs.y;
}

// functions
template<typename T>
T dot(const vec2<T>& lhs, const vec2<T>& rhs) {
  return lhs.x * rhs.x + lhs.y * rhs.y;
}

typedef vec2<float> vec2f;
typedef vec2<int> vec2i;

#endif  // CORE_VEC2_H
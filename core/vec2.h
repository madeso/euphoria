#ifndef CORE_VEC2_H
#define CORE_VEC2_H

#include <cmath>

#include "core/interpolate.h"
#include "core/angle.h"

template <typename T>
class vec2
{
 public:
  typedef vec2<T> Self;
  T               x;
  T               y;

  explicit vec2(const T& a)
      : x(a)
      , y(a)
  {
  }
  vec2(const T& ax, const T& ay)
      : x(ax)
      , y(ay)
  {
  }

  T*
  GetDataPtr()
  {
    return &x;
  }

  const T*
  GetDataPtr() const
  {
    return &x;
  }

  static Self
  FromTo(const Self& from, const Self& to)
  {
    return to - from;
  }

  template <typename F>
  vec2<F>
  StaticCast() const
  {
    return vec2<F>(static_cast<F>(x), static_cast<F>(y));
  }

  void
  operator+=(const vec2<T>& rhs)
  {
    x += rhs.x;
    y += rhs.y;
  }

  void
  operator-=(const vec2<T>& rhs)
  {
    x -= rhs.x;
    y -= rhs.y;
  }

  void
  operator/=(const T& rhs)
  {
    x /= rhs;
    y /= rhs;
  }

  void
  operator*=(const T& rhs)
  {
    x *= rhs;
    y *= rhs;
  }

  Self
  operator-() const
  {
    return Self(-x, -y);
  }

  T
  GetLengthSquared() const
  {
    return x * x + y * y;
  }

  T
  GetLength() const
  {
    return sqrt(GetLengthSquared());
  }

  void
  Normalize()
  {
    *this /= GetLength();
  }

  Self
  GetNormalized() const
  {
    Self r = *this;
    r.Normalize();
    return r;
  }

  Self
  GetRotated(const Angle& a) const
  {
    const T nx = x * Cos(a) - y * Sin(a);
    const T ny = x * Sin(a) + y * Cos(a);
    return Self(nx, ny);
  }

  Self
  GetFlippedY() const
  {
    return Self(x, -y);
  }
};

// operators

template <typename T>
vec2<T>
operator+(const vec2<T>& lhs, const vec2<T>& rhs)
{
  vec2<T> r = lhs;
  r += rhs;
  return r;
}

template <typename T>
vec2<T>
operator-(const vec2<T>& lhs, const vec2<T>& rhs)
{
  vec2<T> r = lhs;
  r -= rhs;
  return r;
}

template <typename T>
vec2<T>
operator/(const vec2<T>& lhs, const T& rhs)
{
  vec2<T> r = lhs;
  r /= rhs;
  return r;
}

template <typename T>
vec2<T> operator*(const vec2<T>& lhs, const T& rhs)
{
  vec2<T> r = lhs;
  r *= rhs;
  return r;
}

template <typename T>
vec2<T> operator*(const T& lhs, const vec2<T>& rhs)
{
  vec2<T> r = rhs;
  r *= lhs;
  return r;
}

template <typename T>
bool
operator==(const vec2<T>& lhs, const vec2<T>& rhs)
{
  return lhs.x == rhs.x && lhs.y == rhs.y;
}

// functions
template <typename T>
T
dot(const vec2<T>& lhs, const vec2<T>& rhs)
{
  return lhs.x * rhs.x + lhs.y * rhs.y;
}

template <typename T>
struct Vec2Transform
{
  static vec2<T>
  Transform(const vec2<T>& from, float v, const vec2<T> to)
  {
    return vec2<T>(
        FloatTransform::Transform(from.x, v, to.x),
        FloatTransform::Transform(from.y, v, to.y));
  }
};

template <typename S, typename T>
S&
operator<<(S& s, const vec2<T>& v)
{
  s << "(" << v.x << ", " << v.y << ")";
  return s;
}

typedef vec2<float>          vec2f;
typedef Vec2Transform<float> Vec2fTransform;
typedef vec2<int>            vec2i;

#endif  // CORE_VEC2_H

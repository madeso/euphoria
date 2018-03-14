#ifndef CORE_VEC2_H
#define CORE_VEC2_H

#include <cmath>

#include "core/interpolate.h"
#include "core/angle.h"
#include "core/pointerornot.h"

template <typename T, typename TStorage>
class vec2
{
 public:
  typedef typename TStorage::Storage Storage;
  typedef vec2<T, TStorage>      Self;
  typedef vec2<T, StoreValue<T>> SelfValue;
  T x;
  T y;

  T
  GetX() const
  {
    return x;
  }

  T
  GetY() const
  {
    return y;
  }

  void
  SetX(T v)
  {
    x = v;
  }

  void
  SetY(T v)
  {
    y = v;
  }

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

  static SelfValue
  FromTo(const Self& from, const Self& to)
  {
    return to - from;
  }

  template <typename F>
  vec2<F, StoreValue<F>>
  StaticCast() const
  {
    return vec2<F, StoreValue<F>>(static_cast<F>(x), static_cast<F>(y));
  }

  template <typename TStorageRhs>
  void
  operator+=(const vec2<T, TStorageRhs>& rhs)
  {
    x += rhs.x;
    y += rhs.y;
  }

  template <typename TStorageRhs>
  void
  operator-=(const vec2<T, TStorageRhs>& rhs)
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

  SelfValue
  operator-() const
  {
    return SelfValue(-x, -y);
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

  SelfValue
  GetNormalized() const
  {
    SelfValue r = *this;
    r.Normalize();
    return r;
  }

  SelfValue
  GetRotated(const Angle& a) const
  {
    const T nx = x * Cos(a) - y * Sin(a);
    const T ny = x * Sin(a) + y * Cos(a);
    return SelfValue(nx, ny);
  }

  SelfValue
  GetFlippedY() const
  {
    return SelfValue(x, -y);
  }
};

// operators

template <typename T, typename TStorage>
vec2<T, StoreValue<T>>
operator+(const vec2<T, TStorage>& lhs, const vec2<T, TStorage>& rhs)
{
  vec2<T, StoreValue<T>> r = lhs;
  r += rhs;
  return r;
}

template <typename T, typename TStorage>
vec2<T, StoreValue<T>>
operator-(const vec2<T, TStorage>& lhs, const vec2<T, StoreValue<T>>& rhs)
{
  vec2<T, StoreValue<T>> r = lhs;
  r -= rhs;
  return r;
}

template <typename T, typename TStorage>
vec2<T, StoreValue<T>>
operator/(const vec2<T, TStorage>& lhs, const T& rhs)
{
  vec2<T, StoreValue<T>> r = lhs;
  r /= rhs;
  return r;
}

template <typename T, typename TStorage>
vec2<T, StoreValue<T>> operator*(const vec2<T, TStorage>& lhs, const T& rhs)
{
  vec2<T, StoreValue<T>> r = lhs;
  r *= rhs;
  return r;
}

template <typename T, typename TStorage>
vec2<T, StoreValue<T>> operator*(const T& lhs, const vec2<T, TStorage>& rhs)
{
  vec2<T, StoreValue<T>> r = rhs;
  r *= lhs;
  return r;
}

template <typename T, typename TStorage>
bool
operator==(const vec2<T, TStorage>& lhs, const vec2<T, TStorage>& rhs)
{
  return lhs.x == rhs.x && lhs.y == rhs.y;
}

// functions
template <typename T, typename TStorage>
T
dot(const vec2<T, TStorage>& lhs, const vec2<T, TStorage>& rhs)
{
  return lhs.x * rhs.x + lhs.y * rhs.y;
}

template <typename T, typename TStorage>
struct Vec2Transform
{
  static vec2<T, StoreValue<T>>
  Transform(const vec2<T, TStorage>& from, float v, const vec2<T, TStorage> to)
  {
    return vec2<T, StoreValue<T>>(
        FloatTransform::Transform(from.x, v, to.x),
        FloatTransform::Transform(from.y, v, to.y));
  }
};

template <typename S, typename T, typename TStorage>
S&
operator<<(S& s, const vec2<T, TStorage>& v)
{
  s << "(" << v.x << ", " << v.y << ")";
  return s;
}

typedef vec2<float, StoreValue<float>>          vec2f;
typedef Vec2Transform<vec2f, StoreValue<float>> Vec2fTransform;
typedef vec2<int, StoreValue<int>>              vec2i;

#endif  // CORE_VEC2_H

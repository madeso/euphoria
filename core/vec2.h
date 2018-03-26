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
  typedef typename TStorage::Storage S;
  typedef vec2<T, TStorage>      Self;
  typedef vec2<T, StoreValue<T>> SelfValue;
  S x;
  S y;

  T
  GetX() const
  {
    return TStorage::Get(x);
  }

  T
  GetY() const
  {
    return TStorage::Get(y);
  }

  void
  SetX(T v)
  {
    TStorage::Set(&x, v);
  }

  void
  SetY(T v)
  {
    TStorage::Set(&y, v);
  }

  vec2(const S& ax, const S& ay)
      : x(ax)
      , y(ay)
  {
  }

  S*
  GetDataPtr()
  {
    return &x;
  }

  const S*
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
    return vec2<F, StoreValue<F>>(
        static_cast<F>(GetX()), static_cast<F>(GetY()));
  }

  template <typename TStorageRhs>
  void
  operator+=(const vec2<T, TStorageRhs>& rhs)
  {
    SetX(GetX() + rhs.GetX());
    SetY(GetY() + rhs.GetY());
  }

  template <typename TStorageRhs>
  void
  operator-=(const vec2<T, TStorageRhs>& rhs)
  {
    SetX(GetX() - rhs.GetX());
    SetY(GetY() - rhs.GetY());
  }

  void
  operator/=(const T& rhs)
  {
    SetX(GetX() / rhs);
    SetY(GetY() / rhs);
  }

  void
  operator*=(const T& rhs)
  {
    SetX(GetX() * rhs);
    SetY(GetY() * rhs);
  }

  SelfValue
  operator-() const
  {
    return SelfValue(-GetX(), -GetY());
  }

  T
  GetLengthSquared() const
  {
    return GetX() * GetX() + GetY() * GetY();
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
    const T nx = GetX() * Cos(a) - GetY() * Sin(a);
    const T ny = GetX() * Sin(a) + GetY() * Cos(a);
    return SelfValue(nx, ny);
  }

  SelfValue
  GetFlippedY() const
  {
    return SelfValue(GetX(), -GetY());
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
  return lhs.GetX() == rhs.GetX() && lhs.GetY() == rhs.GetY();
}

// functions
template <typename T, typename TStorage>
T
dot(const vec2<T, TStorage>& lhs, const vec2<T, TStorage>& rhs)
{
  return lhs.GetX() * rhs.GetX() + lhs.GetY() * rhs.GetY();
}

template <typename T, typename TStorage>
struct Vec2Transform
{
  static vec2<T, StoreValue<T>>
  Transform(const vec2<T, TStorage>& from, float v, const vec2<T, TStorage> to)
  {
    return vec2<T, StoreValue<T>>(
        FloatTransform::Transform(from.GetX(), v, to.GetX()),
        FloatTransform::Transform(from.GetY(), v, to.GetY()));
  }
};

template <typename S, typename T, typename TStorage>
S&
operator<<(S& s, const vec2<T, TStorage>& v)
{
  s << "(" << v.GetX() << ", " << v.GetY() << ")";
  return s;
}

typedef vec2<float, StoreValue<float>>          vec2f;
typedef vec2<float, StorePointer<float>>        vec2fp;
typedef Vec2Transform<float, StoreValue<float>> Vec2fTransform;
typedef vec2<int, StoreValue<int>>              vec2i;
typedef vec2<int, StorePointer<int>>            vec2ip;

#endif  // CORE_VEC2_H

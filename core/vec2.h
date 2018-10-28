#ifndef CORE_VEC2_H
#define CORE_VEC2_H

#include <cmath>

#include "core/interpolate.h"
#include "core/angle.h"
#include "core/typeinfo.h"
#include "core/numeric.h"
#include "core/assert.h"


////////////////////////////////////////////////////////////////////////////////
/// Forward declarations

// a 2d point
template <typename T>
struct point2;

// a 2d vector
template <typename T>
struct Vec2;

// a 2d unit (vector)
template <typename T>
struct unit2;


////////////////////////////////////////////////////////////////////////////////
/// Shared implementations

#define VEC2_CONSTRUCTOR(VEC, T) \
  VEC(const T& ax, const T& ay)  \
      : x(ax)                    \
      , y(ay)                    \
  {                              \
  }

#define VEC2_COMMON_MEMBERS(VEC, T)                         \
  typedef VEC<T> Self;                                      \
  T              x;                                         \
  T              y;                                         \
                                                            \
  T* GetDataPtr()                                           \
  {                                                         \
    return &x;                                              \
  }                                                         \
                                                            \
  const T* GetDataPtr() const                               \
  {                                                         \
    return &x;                                              \
  }                                                         \
                                                            \
  template <typename F>                                     \
  point2<F> StaticCast() const                              \
  {                                                         \
    return point2<F>(static_cast<F>(x), static_cast<F>(y)); \
  }                                                         \
                                                            \
  Self GetRotated(const Angle& a) const                     \
  {                                                         \
    const T nx = x * Cos(a) - y * Sin(a);                   \
    const T ny = x * Sin(a) + y * Cos(a);                   \
    return Self(nx, ny);                                    \
  }                                                         \
                                                            \
  Self GetFlippedY() const                                  \
  {                                                         \
    return Self(x, -y);                                     \
  }

#define VEC2_SELF_ADD_SUBTRACT(VEC)  \
  void operator+=(const VEC<T>& rhs) \
  {                                  \
    x = x + rhs.x;                   \
    y = y + rhs.y;                   \
  }                                  \
                                     \
  void operator-=(const VEC<T>& rhs) \
  {                                  \
    x = x - rhs.x;                   \
    y = y - rhs.y;                   \
  }

#define VEC2_INVERT_SELF() \
  Self operator-() const   \
  {                        \
    return Self(-x, -y);   \
  }

#define VEC2_LENGTH_SQUARED(T) \
  T GetLengthSquared() const   \
  {                            \
    return x * x + y * y;      \
  }


////////////////////////////////////////////////////////////////////////////////

template <typename T>
struct point2
{
  VEC2_CONSTRUCTOR(point2, T)
  VEC2_COMMON_MEMBERS(point2, T)
  VEC2_SELF_ADD_SUBTRACT(Vec2)

  static Self
  Origo()
  {
    return Self{0, 0};
  }
};


////////////////////////////////////////////////////////////////////////////////

template <typename T>
struct Vec2
{
  VEC2_CONSTRUCTOR(Vec2, T)
  VEC2_COMMON_MEMBERS(Vec2, T)
  VEC2_SELF_ADD_SUBTRACT(Vec2)
  VEC2_INVERT_SELF()
  VEC2_LENGTH_SQUARED(T)

  using Unit = unit2<T>;

  explicit Vec2(const Unit& u)
      : x(u.x)
      , y(u.y)
  {
  }

  static Self
  FromTo(const point2<T>& from, const point2<T>& to)
  {
    return Self{to.x - from.x, to.y - from.y};
  }

  void
  operator/=(const T& rhs)
  {
    x = x / rhs;
    y = y / rhs;
  }

  void
  operator*=(const T& rhs)
  {
    x = x * rhs;
    y = y * rhs;
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

  Unit
  GetNormalized() const
  {
    Self r = *this;
    r.Normalize();
    return Unit{r};
  }
};


////////////////////////////////////////////////////////////////////////////////

template <typename T>
struct unit2
{
  VEC2_COMMON_MEMBERS(unit2, T)
  VEC2_INVERT_SELF()
  VEC2_LENGTH_SQUARED(T)

  bool
  IsValid() const
  {
    return ::IsEqual(GetLengthSquared(), 1);
  }

 private:
  explicit unit2(const Vec2<T>& v)
      : x(v.x)
      , y(v.y)
  {
    ASSERT(IsValid());
  }

  friend struct Vec2<T>;
};


////////////////////////////////////////////////////////////////////////////////
/// Math operators

// point

template <typename T>
point2<T>
operator+(const point2<T>& lhs, const Vec2<T>& rhs)
{
  point2<T> r = lhs;
  r += rhs;
  return r;
}

template <typename T>
point2<T>
operator+(const Vec2<T>& lhs, const point2<T>& rhs)
{
  point2<T> r = rhs;
  r += lhs;
  return r;
}

template <typename T>
point2<T>
operator-(const point2<T>& lhs, const Vec2<T>& rhs)
{
  point2<T> r = lhs;
  r -= rhs;
  return r;
}

template <typename T>
point2<T>
operator-(const Vec2<T>& lhs, const point2<T>& rhs)
{
  return -rhs + lhs;
}

// vector

template <typename T>
Vec2<T>
operator+(const Vec2<T>& lhs, const Vec2<T>& rhs)
{
  Vec2<T> r = lhs;
  r += rhs;
  return r;
}

template <typename T>
Vec2<T>
operator-(const Vec2<T>& lhs, const Vec2<T>& rhs)
{
  Vec2<T> r = lhs;
  r -= rhs;
  return r;
}

template <typename T>
Vec2<T> operator*(const Vec2<T>& lhs, const T& rhs)
{
  Vec2<T> r = lhs;
  r *= rhs;
  return r;
}

template <typename T>
Vec2<T> operator*(const T& lhs, const Vec2<T>& rhs)
{
  Vec2<T> r = rhs;
  r *= lhs;
  return r;
}

template <typename T>
Vec2<T>
operator/(const Vec2<T>& lhs, const T& rhs)
{
  Vec2<T> r = lhs;
  r /= rhs;
  return r;
}


////////////////////////////////////////////////////////////////////////////////
/// Equality operators

template <typename T>
bool
operator==(const point2<T>& lhs, const point2<T>& rhs)
{
  return lhs.x == rhs.x && lhs.y == rhs.y;
}

template <typename T>
bool
operator==(const Vec2<T>& lhs, const Vec2<T>& rhs)
{
  return lhs.x == rhs.x && lhs.y == rhs.y;
}

template <typename T>
bool
operator==(const unit2<T>& lhs, const unit2<T>& rhs)
{
  return lhs.x == rhs.x && lhs.y == rhs.y;
}


////////////////////////////////////////////////////////////////////////////////
/// Functions

template <typename T>
T
dot(const Vec2<T>& lhs, const Vec2<T>& rhs)
{
  return lhs.x * rhs.x + lhs.y * rhs.y;
}

template <typename T>
point2<T>
Scale(const point2<T>& p, T scale)
{
  return point2<T>{p.x * scale, p.y * scale};
}

////////////////////////////////////////////////////////////////////////////////
/// Transform

template <typename T>
struct Vec2Transform
{
  static Vec2<T>
  Transform(const Vec2<T>& from, float v, const Vec2<T> to)
  {
    return Vec2<T>(
        FloatTransform::Transform(from.x, v, to.x),
        FloatTransform::Transform(from.y, v, to.y));
  }
};

template <typename T>
struct Point2Transform
{
  static point2<T>
  Transform(const point2<T>& from, float v, const point2<T> to)
  {
    return point2<T>(
        FloatTransform::Transform(from.x, v, to.x),
        FloatTransform::Transform(from.y, v, to.y));
  }
};

// todo: add one lerp and one slerp unit transform?


////////////////////////////////////////////////////////////////////////////////
/// Printing

template <typename S, typename T>
S&
operator<<(S& s, const Vec2<T>& v)
{
  s << "(" << v.x << ", " << v.y << ")";
  return s;
}

template <typename S, typename T>
S&
operator<<(S& s, const point2<T>& v)
{
  s << "(" << v.x << ", " << v.y << ")";
  return s;
}

template <typename S, typename T>
S&
operator<<(S& s, const unit2<T>& v)
{
  s << "(" << v.x << ", " << v.y << ")";
  return s;
}


////////////////////////////////////////////////////////////////////////////////
/// Typedefs

typedef point2<float>          point2f;
typedef Vec2<float>            Vec2f;
typedef unit2<float>           unit2f;
typedef Vec2Transform<float>   Vec2fTransform;
typedef Point2Transform<float> Point2fTransform;

typedef point2<int>          point2i;
typedef Vec2<int>            Vec2i;
typedef unit2<int>           unit2i;
typedef Vec2Transform<int>   Vec2iTransform;
typedef Point2Transform<int> Point2iTransform;


////////////////////////////////////////////////////////////////////////////////
/// Typeids

TYPEID_SETUP_TYPE(Vec2f);

#endif  // CORE_VEC2_H

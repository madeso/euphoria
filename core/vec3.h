#ifndef CORE_VEC3_H
#define CORE_VEC3_H

#include <ostream>
#include <tuple>

#include "core/vec2.h"
#include "core/numeric.h"
#include "core/assert.h"


////////////////////////////////////////////////////////////////////////////////
/// Forward declarations

template <typename T>
struct point3;

template <typename T>
struct Vec3;

template <typename T>
struct unit3;


template <typename T>
struct scale3;

////////////////////////////////////////////////////////////////////////////////
/// Shared implementations

#define VEC3_COMMON_MEMBERS(VEC, T) \
  typedef VEC<T> Self;              \
                                    \
  T x;                              \
  T y;                              \
  T z;                              \
                                    \
  T* GetDataPtr()                   \
  {                                 \
    return &x;                      \
  }                                 \
                                    \
  const T* GetDataPtr() const       \
  {                                 \
    return &x;                      \
  }                                 \
                                    \
  T GetComponentSum() const         \
  {                                 \
    return x + y + z;               \
  }

#define VEC3_CONSTRUCTOR(VEC, T)             \
  explicit VEC(const T& a)                   \
      : x(a)                                 \
      , y(a)                                 \
      , z(a)                                 \
  {                                          \
  }                                          \
                                             \
  explicit VEC(const std::tuple<T, T, T>& a) \
      : x(a[0])                              \
      , y(a[1])                              \
      , z(a[2])                              \
  {                                          \
  }                                          \
                                             \
  VEC(const T& ax, const T& ay, const T& az) \
      : x(ax)                                \
      , y(ay)                                \
      , z(az)                                \
  {                                          \
  }                                          \
                                             \
  explicit VEC(const T* a)                   \
      : x(a[0])                              \
      , y(a[1])                              \
      , z(a[2])                              \
  {                                          \
  }

#define VEC3_CONSTRUCTOR2(VEC, T, VEC2) \
  VEC(const VEC2<T>& a, const T& az)    \
      : x(a.x)                          \
      , y(a.y)                          \
      , z(az)                           \
  {                                     \
  }

#define VEC3_SELF_ADD_SUBTRACT(VEC)  \
  void operator+=(const VEC<T>& rhs) \
  {                                  \
    x += rhs.x;                      \
    y += rhs.y;                      \
    z += rhs.z;                      \
  }                                  \
                                     \
  void operator-=(const VEC<T>& rhs) \
  {                                  \
    x -= rhs.x;                      \
    y -= rhs.y;                      \
    z -= rhs.z;                      \
  }

#define VEC3_INVERT_SELF()                     \
  Self operator-() const                       \
  {                                            \
    return Self(-this->x, -this->y, -this->z); \
  }

#define VEC3_LENGTH_SQUARED(T)    \
  T GetLengthSquared() const      \
  {                               \
    return x * x + y * y + z * z; \
  }

#define VEC3_DELETE_EQUAL()
/*
#define VEC3_DELETE_EQUAL()              \
  bool operator==(const Self&) = delete; \
  bool operator!=(const Self&) = delete;
  */

////////////////////////////////////////////////////////////////////////////////

template <typename T>
struct point3
{
  VEC3_COMMON_MEMBERS(point3, T)
  VEC3_CONSTRUCTOR(point3, T)
  VEC3_CONSTRUCTOR2(point3, T, point2)
  VEC3_SELF_ADD_SUBTRACT(Vec3)
  VEC3_DELETE_EQUAL()

  explicit point3(const Vec3<T>& v)
      : x(v.x)
      , y(v.y)
      , z(v.z)
  {
  }

  static Self
  Origo()
  {
    return Self{0, 0, 0};
  }
};


////////////////////////////////////////////////////////////////////////////////

template <typename T>
struct scale3
{
  VEC3_COMMON_MEMBERS(scale3, T)
  VEC3_CONSTRUCTOR(scale3, T)
  VEC3_DELETE_EQUAL()
};

////////////////////////////////////////////////////////////////////////////////

template <typename T>
struct Vec3
{
  VEC3_COMMON_MEMBERS(Vec3, T)
  VEC3_CONSTRUCTOR(Vec3, T)
  VEC3_CONSTRUCTOR2(Vec3, T, Vec2)
  VEC3_SELF_ADD_SUBTRACT(Vec3)
  VEC3_INVERT_SELF()
  VEC3_LENGTH_SQUARED(T)
  VEC3_DELETE_EQUAL()

  typedef unit3<T> Unit;

  explicit Vec3(const point3<T>& v)
      : x(v.x)
      , y(v.y)
      , z(v.z)
  {
  }

  static Self
  FromTo(const point3<T>& from, const point3<T>& to)
  {
    return Self{to.x - from.x, to.y - from.y, to.z - from.z};
  }

  void
  operator/=(const T& rhs)
  {
    x /= rhs;
    y /= rhs;
    z /= rhs;
  }

  void
  operator*=(const T& rhs)
  {
    x *= rhs;
    y *= rhs;
    z *= rhs;
  }

  T
  GetLength() const
  {
    return sqrt(GetLengthSquared());
  }

  void
  Normalize()
  {
    const T l2 = GetLengthSquared();
    if(IsEqual(l2, 0))
    {
      *this = Unit::Up();
    }
    else
    {
      *this /= Sqrt(l2);
    }
  }


  Unit
  GetNormalized() const
  {
    Self r = *this;
    r.Normalize();
    return Unit::ToUnit(r);
  }
};


////////////////////////////////////////////////////////////////////////////////

template <typename T>
struct unit3 : public Vec3<T>
{
  using Self = unit3<T>;

  VEC3_INVERT_SELF()
  VEC3_DELETE_EQUAL()

  bool
  IsValid() const
  {
    return ::IsEqual(this->GetLengthSquared(), 1);
  }

  static Self
  XAxis()
  {
    return Self{1, 0, 0};
  }

  static Self
  YAxis()
  {
    return Self{0, 1, 0};
  }

  static Self
  ZAxis()
  {
    return Self(0, 0, 1);
  }

  static Self
  Up()
  {
    return YAxis();
  }

  static Self
  Down()
  {
    return -YAxis();
  }

  static Self
  Right()
  {
    return XAxis();
  }

  static Self
  Left()
  {
    return -XAxis();
  }

  static Self
  In()
  {
    return -ZAxis();
  }

  static Self
  Out()
  {
    return ZAxis();
  }

  static Self
  ToUnit(T x, T y, T z)
  {
    return Self{x, y, z};
  }

  static Self
  ToUnit(const Vec3<T>& v)
  {
    return Self{v.x, v.y, v.z};
  }

 private:
  explicit unit3(T a, T b, T c)
      : Vec3<T>(a, b, c)
  {
    ASSERT(IsValid());
  }
};


////////////////////////////////////////////////////////////////////////////////
/// Math operators

// point

template <typename T>
point3<T>
operator+(const point3<T>& lhs, const Vec3<T>& rhs)
{
  point3<T> r = lhs;
  r += rhs;
  return r;
}

template <typename T>
point3<T>
operator+(const Vec3<T>& lhs, const point3<T>& rhs)
{
  point3<T> r = rhs;
  r += lhs;
  return r;
}

template <typename T>
point3<T>
operator-(const point3<T>& lhs, const Vec3<T>& rhs)
{
  point3<T> r = lhs;
  r -= rhs;
  return r;
}

template <typename T>
point3<T>
operator-(const Vec3<T>& lhs, const point3<T>& rhs)
{
  return -rhs + lhs;
}

// vector

template <typename T>
Vec3<T>
operator+(const Vec3<T>& lhs, const Vec3<T>& rhs)
{
  Vec3<T> r = lhs;
  r += rhs;
  return r;
}

template <typename T>
Vec3<T>
operator-(const Vec3<T>& lhs, const Vec3<T>& rhs)
{
  Vec3<T> r = lhs;
  r -= rhs;
  return r;
}

template <typename T>
Vec3<T> operator*(T lhs, const Vec3<T>& rhs)
{
  Vec3<T> r = rhs;
  r *= lhs;
  return r;
}

template <typename T>
Vec3<T> operator*(const Vec3<T>& lhs, T rhs)
{
  Vec3<T> r = lhs;
  r *= rhs;
  return r;
}

template <typename T>
Vec3<T>
operator/(const Vec3<T>& lhs, T rhs)
{
  Vec3<T> r = lhs;
  r /= rhs;
  return r;
}

template <typename T>
Vec3<T>
operator/(T lhs, const Vec3<T>& rhs)
{
  const Vec3<T> r{1 / rhs.x, 1 / rhs.y, 1 / rhs.z};
  return r;
}

template <typename T>
Vec3<T>
ComponentMultiply(const Vec3<T>& lhs, const Vec3<T>& rhs)
{
  return Vec3<T>(lhs.x * rhs.x, lhs.y * rhs.y, lhs.z * rhs.z);
}


////////////////////////////////////////////////////////////////////////////////
/// Equality operators

#define VEC3_EQUAL_OPERATOR(VEC)                                 \
  template <                                                     \
      typename T,                                                \
      typename K = std::enable_if_t<std::is_integral<T>::value>> \
  bool operator==(const VEC<T>& lhs, const VEC<T>& rhs)          \
  {                                                              \
    return lhs.x == rhs.x && lhs.y == rhs.y && lhs.z == rhs.z;   \
  }

VEC3_EQUAL_OPERATOR(point3)
VEC3_EQUAL_OPERATOR(Vec3)
VEC3_EQUAL_OPERATOR(unit3)
VEC3_EQUAL_OPERATOR(scale3)


////////////////////////////////////////////////////////////////////////////////
/// Functions

template <typename T>
T
dot(const Vec3<T>& lhs, const Vec3<T>& rhs)
{
  return lhs.x * rhs.x + lhs.y * rhs.y + lhs.z * rhs.z;
}

template <typename T>
Vec3<T>
cross(const Vec3<T>& v, const Vec3<T> u)
{
  return Vec3<T>(
      (v.y * u.z) - (v.z * u.y),
      (v.z * u.x) - (v.x * u.z),
      (v.x * u.y) - (v.y * u.x));
}

template <typename T>
point3<T>
Scale(const point3<T>& p, T scale)
{
  return point3<T>{p.x * scale, p.y * scale, p.z * scale};
}


////////////////////////////////////////////////////////////////////////////////
/// Transformations

// todo: implement transformations


////////////////////////////////////////////////////////////////////////////////
/// Printing

template <typename T>
std::ostream&
operator<<(std::ostream& stream, const point3<T>& v)
{
  return stream << "(" << v.x << ", " << v.y << ", " << v.z << ")";
}


template <typename T>
std::ostream&
operator<<(std::ostream& stream, const Vec3<T>& v)
{
  return stream << "(" << v.x << ", " << v.y << ", " << v.z << ")";
}

template <typename T>
std::ostream&
operator<<(std::ostream& stream, const unit3<T>& v)
{
  return stream << "(" << v.x << ", " << v.y << ", " << v.z << ")";
}

template <typename T>
std::ostream&
operator<<(std::ostream& stream, const scale3<T>& v)
{
  return stream << "(" << v.x << ", " << v.y << ", " << v.z << ")";
}


////////////////////////////////////////////////////////////////////////////////
/// Typedefs


typedef point3<float> point3f;
typedef Vec3<float>   Vec3f;
typedef unit3<float>  unit3f;

typedef point3<int> point3i;
typedef Vec3<int>   Vec3i;
typedef unit3<int>  unit3i;


////////////////////////////////////////////////////////////////////////////////
/// Typeids

#endif  // CORE_VEC3_H

#ifndef CORE_RAY_H
#define CORE_RAY_H

#include "core/vec3.h"

template <typename T>
class UnitRay3
{
 public:
  typedef unit3<T>    Unit;
  typedef point3<T>   Point;
  typedef UnitRay3<T> Self;

  UnitRay3(const Point& from, const Unit& dir)
      : from(from)
      , dir(dir)
  {
  }

  static Self
  FromTo(const Point& from, const Point& to)
  {
    return Self{from, Vec3<T>::FromTo(from, to).GetNormalized()};
  }

  static Self
  FromDirection(const Unit& dir)
  {
    return Self{Point::Origo(), dir};
  }

  Point
  GetPoint(T at) const
  {
    return from + dir * at;
  }

  Point from;
  Unit  dir;
};

template <typename T>
class Ray3
{
 public:
  typedef point3<T> Point;
  typedef Vec3<T> Vec;
  typedef Ray3<T> Self;

  Ray3(const Point& from, const Vec& dir)
      : from(from)
      , dir(dir)
  {
  }

  static Self
  FromTo(const Point& from, const Point& to)
  {
    return Self{from, Vec::FromTo(from, to)};
  }

  static Self
  FromDirection(const Vec& dir)
  {
    return Self{Point::Origo(), dir};
  }

  Point
  GetPoint(T at) const
  {
    return from + dir * at;
  }

  UnitRay3<T>
  GetNormalized() const
  {
    return UnitRay3<T>{from, dir.GetNormalized()};
  }

  Point from;
  Vec dir;
};

typedef Ray3<float>     Ray3f;
typedef UnitRay3<float> UnitRay3f;

#endif  // CORE_RAY_H

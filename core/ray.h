#ifndef CORE_RAY_H
#define CORE_RAY_H

#include "core/vec3.h"

template <typename T>
class UnitRay3
{
 public:
  typedef vec3<T>       Vec;
  typedef typename vec3<T>::Unit Unit;
  typedef UnitRay3<T>   Self;

  UnitRay3(const Vec& from, const Unit& dir)
      : from(from)
      , dir(dir)
  {
  }

  static Self
  FromTo(const Vec& from, const Vec& to)
  {
    return Self{from, Vec::FromTo(from, to).GetNormalized()};
  }

  static Self
  FromDirection(const Unit& dir)
  {
    return Self{Vec::Origo(), dir};
  }

  Vec
  GetPoint(T at) const
  {
    return from + dir * at;
  }

  Vec  from;
  Unit dir;
};

template <typename T>
class Ray3
{
 public:
  typedef vec3<T> Vec;
  typedef Ray3<T> Self;

  Ray3(const Vec& from, const Vec& dir)
      : from(from)
      , dir(dir)
  {
  }

  static Self
  FromTo(const Vec& from, const Vec& to)
  {
    return Self{from, Vec::FromTo(from, to)};
  }

  static Self
  FromDirection(const Vec& dir)
  {
    return Self{Vec::Origo(), dir};
  }

  Vec
  GetPoint(T at) const
  {
    return from + dir * at;
  }

  UnitRay3<T>
  GetNormalized() const
  {
    return UnitRay3<T>{from, dir.GetNormalized()};
  }

  Vec from;
  Vec dir;
};

typedef Ray3<float>     Ray3f;
typedef UnitRay3<float> UnitRay3f;

#endif  // CORE_RAY_H

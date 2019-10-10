#ifndef CORE_RAY_H
#define CORE_RAY_H

#include "core/vec3.h"

namespace euphoria::core
{
    template <typename T>
    class UnitRay3
    {
    public:
        typedef unit3<T>    Unit;
        typedef vec3<T>     Point;
        typedef UnitRay3<T> Self;

        UnitRay3(const Point& from, const Unit& dir) : from(from), dir(dir) {}

        [[nodiscard]]
        static Self
        FromTo(const Point& from, const Point& to)
        {
            return Self {from, vec3<T>::FromTo(from, to).GetNormalized()};
        }

        [[nodiscard]]
        static Self
        FromDirection(const Unit& dir)
        {
            return Self {Point::Zero(), dir};
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
        typedef vec3<T> Point;
        typedef vec3<T> Vec;
        typedef Ray3<T> Self;

        Ray3(const Point& from, const Vec& dir) : from(from), dir(dir) {}

        [[nodiscard]]
        static Self
        FromTo(const Point& from, const Point& to)
        {
            return Self {from, Vec::FromTo(from, to)};
        }

        [[nodiscard]]
        static Self
        FromDirection(const Vec& dir)
        {
            return Self {Point::Zero(), dir};
        }

        Point
        GetPoint(T at) const
        {
            return from + dir * at;
        }

        UnitRay3<T>
        GetNormalized() const
        {
            return UnitRay3<T> {from, dir.GetNormalized()};
        }

        Point from;
        Vec   dir;
    };

    typedef Ray3<float>     Ray3f;
    typedef UnitRay3<float> UnitRay3f;

}  // namespace euphoria::core

#endif  // CORE_RAY_H

#ifndef CORE_VEC4_H
#define CORE_VEC4_H

#include "core/vec3.h"

#include "core/assert.h"
#include "core/numeric.h"

#include <ostream>

namespace euphoria::core
{
    // represents a homogeneous coordinate
    template <typename T>
    class vec4
    {
    public:
        T x;
        T y;
        T z;
        T w;

        explicit vec4(const T& a) : x(a), y(a), z(a), w(a) {}
        vec4(const T& ax, const T& ay, const T& az, const T& aw)
            : x(ax), y(ay), z(az), w(aw)
        {}

        // w for point is 1
        // w for vector is 0
        vec4(const vec3<T>& a, float w) : x(a.x), y(a.y), z(a.z), w(w) {}

        vec4(const scale3<T>& a) : x(a.x), y(a.y), z(a.z), w(1) {}

        vec3<T>
        ToVec3(T ww) const
        {
            ASSERTX(IsEqual(w, ww), w, ww);
            return vec3<T>(x, y, z);
        }

        // todo: replace this and actually call the assert version always
        vec3<T>
        ToVec3() const
        {
            return vec3<T>(x, y, z);
        }

        explicit vec4(const T* a) : x(a[0]), y(a[1]), z(a[2]), w(a[3]) {}

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

        T
        GetComponentSum() const
        {
            return x + y + z + w;
        }
    };

    template <typename T>
    bool
    operator==(const vec4<T>& lhs, const vec4<T>& rhs)
    {
        return lhs.x == rhs.x && lhs.y == rhs.y && lhs.z == rhs.z
               && lhs.w == rhs.w;
    }

    template <typename T>
    std::ostream&
    operator<<(std::ostream& stream, const vec4<T>& v)
    {
        return stream << "(" << v.x << ", " << v.y << ", " << v.z << ", " << v.w
                      << ")";
    }

    template <typename T>
    vec4<T>
    ComponentMultiply(const vec4<T>& lhs, const vec4<T>& rhs)
    {
        return vec4<T>(
                lhs.x * rhs.x, lhs.y * rhs.y, lhs.z * rhs.z, lhs.w * rhs.w);
    }

    typedef vec4<float> vec4f;
    typedef vec4<int>   vec4i;

}  // namespace euphoria::core

#endif  // CORE_VEC4_H

#pragma once

#include "core/vec3.h"

#include "core/assert.h"
#include "core/numeric.h"

#include <ostream>

namespace euphoria::core
{
    // represents a homogeneous coordinate
    template <typename T>
    struct Vec4
    {
        T x;
        T y;
        T z;
        T w;

        explicit Vec4(const T& a) : x(a), y(a), z(a), w(a) {}
        Vec4(const T& ax, const T& ay, const T& az, const T& aw)
            : x(ax), y(ay), z(az), w(aw)
        {}

        // w for point is 1
        // w for vector is 0
        Vec4(const Vec3<T>& a, float aw) : x(a.x), y(a.y), z(a.z), w(aw) {}

        Vec4(const Scale3<T>& a) : x(a.x), y(a.y), z(a.z), w(1) {}

        Vec3<T>
        to_vec3(T ww) const
        {
            ASSERTX(is_equal(w, ww), w, ww);
            return Vec3<T>(x, y, z);
        }

        // todo(Gustav): replace this and actually call the assert version always
        Vec3<T>
        to_vec3() const
        {
            return Vec3<T>(x, y, z);
        }

        explicit Vec4(const T* a) : x(a[0]), y(a[1]), z(a[2]), w(a[3]) {}

        T*
        get_data_ptr()
        {
            return &x;
        }

        const T*
        get_data_ptr() const
        {
            return &x;
        }

        T
        get_component_sum() const
        {
            return x + y + z + w;
        }
    };

    template <typename T>
    bool
    operator==(const Vec4<T>& lhs, const Vec4<T>& rhs)
    {
        return lhs.x == rhs.x && lhs.y == rhs.y && lhs.z == rhs.z
               && lhs.w == rhs.w;
    }

    template <typename T>
    std::ostream&
    operator<<(std::ostream& stream, const Vec4<T>& v)
    {
        return stream << "(" << v.x << ", " << v.y << ", " << v.z << ", " << v.w
                      << ")";
    }

    template <typename T>
    Vec4<T>
    component_multiply(const Vec4<T>& lhs, const Vec4<T>& rhs)
    {
        return Vec4<T>(
                lhs.x * rhs.x, lhs.y * rhs.y, lhs.z * rhs.z, lhs.w * rhs.w);
    }

    using Vec4f = Vec4<float>;
    using Vec4i = Vec4<int>;

}

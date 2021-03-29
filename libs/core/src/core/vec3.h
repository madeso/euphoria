#ifndef CORE_VEC3_H
#define CORE_VEC3_H

#include <ostream>
#include <tuple>

#include "core/vec2.h"
#include "core/numeric.h"
#include "core/assert.h"


namespace euphoria::core
{
    ////////////////////////////////////////////////////////////////////////////////
    /// Forward declarations

    template <typename T>
    struct vec3;

    template <typename T>
    struct unit3;

    template <typename T>
    struct scale3;

    ////////////////////////////////////////////////////////////////////////////////

    template <typename T>
    struct scale3
    {
        using Self = scale3<T>;

        T x;
        T y;
        T z;

        T* GetDataPtr() { return &x; }

        [[nodiscard]] const T* GetDataPtr() const { return &x; }

        [[nodiscard]] T GetComponentSum() const { return x + y + z; }

        explicit scale3(const T& a) : x(a), y(a), z(a)
        {
        }

        explicit scale3(const std::tuple<T, T, T>& a) : x(std::get<0>(a)), y(std::get<1>(a)), z(std::get<2>(a))
        {
        }

        scale3(const T& ax, const T& ay, const T& az) : x(ax), y(ay), z(az)
        {
        }

        explicit scale3(const T* a) : x(a[0]), y(a[1]), z(a[2])
        {
        }
    };

    ////////////////////////////////////////////////////////////////////////////////

    template <typename T>
    struct vec3
    {
        using Self = vec3<T>;

        T x;
        T y;
        T z;

        T* GetDataPtr() { return &x; }

        [[nodiscard]] const T* GetDataPtr() const { return &x; }

        [[nodiscard]] T GetComponentSum() const { return x + y + z; }

        explicit vec3(const T& a) : x(a), y(a), z(a)
        {
        }

        explicit vec3(const std::tuple<T, T, T>& a) : x(std::get<0>(a)), y(std::get<1>(a)), z(std::get<2>(a))
        {
        }

        vec3(const T& ax, const T& ay, const T& az) : x(ax), y(ay), z(az)
        {
        }

        explicit vec3(const T* a) : x(a[0]), y(a[1]), z(a[2])
        {
        }

        vec3(const vec2<T>& a, const T& az) : x(a.x), y(a.y), z(az)
        {
        }

        void operator+=(const vec3<T>& rhs)
        {
            x += rhs.x;
            y += rhs.y;
            z += rhs.z;
        }

        void operator-=(const vec3<T>& rhs)
        {
            x -= rhs.x;
            y -= rhs.y;
            z -= rhs.z;
        }

        Self operator-() const { return Self(-this->x, -this->y, -this->z); }

        T GetLengthSquared() const { return x * x + y * y + z * z; }

        using Unit = unit3<T>;

        static Self
        FromTo(const vec3<T>& from, const vec3<T>& to)
        {
            return Self {to.x - from.x, to.y - from.y, to.z - from.z};
        }

        static Self
        Zero()
        {
            return Self {0, 0, 0};
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

        [[nodiscard]] T
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


        [[nodiscard]] Unit
        GetNormalized() const
        {
            Self r = *this;
            r.Normalize();
            return Unit::ToUnit(r);
        }
    };


    ////////////////////////////////////////////////////////////////////////////////

    template <typename T>
    struct unit3 : public vec3<T>
    {
        using Self = unit3<T>;

        Self operator-() const { return Self(-this->x, -this->y, -this->z); }

        [[nodiscard]] bool
        IsValid() const
        {
            return IsEqual(this->GetLengthSquared(), 1);
        }

        static Self
        XAxis()
        {
            return Self {1, 0, 0};
        }

        static Self
        YAxis()
        {
            return Self {0, 1, 0};
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
            return Self {x, y, z};
        }

        static Self
        ToUnit(const vec3<T>& v)
        {
            return Self {v.x, v.y, v.z};
        }

    private:
        explicit unit3(T a, T b, T c) : vec3<T>(a, b, c)
        {
            ASSERT(IsValid());
        }
    };


    ////////////////////////////////////////////////////////////////////////////////
    /// Math operators

    template <typename T>
    vec3<T>
    operator+(const vec3<T>& lhs, const vec3<T>& rhs)
    {
        vec3<T> r = lhs;
        r += rhs;
        return r;
    }

    template <typename T>
    vec3<T>
    operator-(const vec3<T>& lhs, const vec3<T>& rhs)
    {
        vec3<T> r = lhs;
        r -= rhs;
        return r;
    }

    template <typename T>
    vec3<T> operator*(T lhs, const vec3<T>& rhs)
    {
        vec3<T> r = rhs;
        r *= lhs;
        return r;
    }

    template <typename T>
    vec3<T> operator*(const vec3<T>& lhs, T rhs)
    {
        vec3<T> r = lhs;
        r *= rhs;
        return r;
    }

    template <typename T>
    vec3<T>
    operator/(const vec3<T>& lhs, T rhs)
    {
        vec3<T> r = lhs;
        r /= rhs;
        return r;
    }

    template <typename T>
    vec3<T>
    operator/(T lhs, const vec3<T>& rhs)
    {
        const vec3<T> r {lhs / rhs.x, lhs / rhs.y, lhs / rhs.z};
        return r;
    }

    template <typename T>
    vec3<T>
    ComponentMultiply(const vec3<T>& lhs, const vec3<T>& rhs)
    {
        return vec3<T>(lhs.x * rhs.x, lhs.y * rhs.y, lhs.z * rhs.z);
    }


    ////////////////////////////////////////////////////////////////////////////////
    /// Equality operators

    template <typename T, typename K = std::enable_if_t<std::is_integral<T>::value>>
    bool operator==(const vec3<T>& lhs, const vec3<T>& rhs)
    {
        return lhs.x == rhs.x && lhs.y == rhs.y && lhs.z == rhs.z;
    }

    template <typename T, typename K = std::enable_if_t<std::is_integral<T>::value>>
    bool operator==(const unit3<T>& lhs, const unit3<T>& rhs)
    {
        return lhs.x == rhs.x && lhs.y == rhs.y && lhs.z == rhs.z;
    }

    template <typename T, typename K = std::enable_if_t<std::is_integral<T>::value>>
    bool operator==(const scale3<T>& lhs, const scale3<T>& rhs)
    {
        return lhs.x == rhs.x && lhs.y == rhs.y && lhs.z == rhs.z;
    }


    ////////////////////////////////////////////////////////////////////////////////
    /// Functions

    template <typename T>
    T
    dot(const vec3<T>& lhs, const vec3<T>& rhs)
    {
        return lhs.x * rhs.x + lhs.y * rhs.y + lhs.z * rhs.z;
    }

    template <typename T>
    vec3<T>
    cross(const vec3<T>& v, const vec3<T> u)
    {
        return vec3<T>(
                (v.y * u.z) - (v.z * u.y),
                (v.z * u.x) - (v.x * u.z),
                (v.x * u.y) - (v.y * u.x));
    }


    ////////////////////////////////////////////////////////////////////////////////
    /// Transformations

    // todo: implement transformations


    ////////////////////////////////////////////////////////////////////////////////
    /// Printing

    template <typename T>
    std::ostream&
    operator<<(std::ostream& stream, const vec3<T>& v)
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


    using vec3f = vec3<float>;
    using unit3f = unit3<float>;
    using scale3f = scale3<float>;

    using vec3i = vec3<int>;
    using unit3i = unit3<int>;
    using scale3i = scale3<int>;
}

#endif  // CORE_VEC3_H

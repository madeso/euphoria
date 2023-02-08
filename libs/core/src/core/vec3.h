#pragma once



#include <tuple>

#include "core/vec2.h"
#include "core/numeric.h"
#include "assert/assert.h"
#include "core/cint.h"


namespace euphoria::core
{
    ////////////////////////////////////////////////////////////////////////////////
    /// Forward declarations

    template <typename T>
    struct vec3;

    template <typename T>
    struct unit3;

    template <typename T>
    struct Scale3;

    ////////////////////////////////////////////////////////////////////////////////

    template <typename T>
    struct Scale3
    {
        using Self = Scale3<T>;

        T x;
        T y;
        T z;

        T* get_data_ptr() { return &x; }

        [[nodiscard]] const T* get_data_ptr() const { return &x; }

        [[nodiscard]] T get_component_sum() const { return x + y + z; }

        explicit Scale3(const T& a) : x(a), y(a), z(a)
        {
        }

        explicit Scale3(const std::tuple<T, T, T>& a) : x(std::get<0>(a)), y(std::get<1>(a)), z(std::get<2>(a))
        {
        }

        Scale3(const T& ax, const T& ay, const T& az) : x(ax), y(ay), z(az)
        {
        }

        explicit Scale3(const T* a) : x(a[0]), y(a[1]), z(a[2])
        {
        }

        bool operator==(const Scale3<T>& rhs) = delete;
    };

    ////////////////////////////////////////////////////////////////////////////////

    template <typename T>
    struct vec3
    {
        using Self = vec3<T>;
        using Unit = unit3<T>;

        T x;
        T y;
        T z;

        T* get_data_ptr() { return &x; }

        [[nodiscard]] const T* get_data_ptr() const { return &x; }

        [[nodiscard]] T get_component_sum() const { return x + y + z; }

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

        T get_length_squared() const { return x * x + y * y + z * z; }

        static Self
        from_to(const vec3<T>& from, const vec3<T>& to)
        {
            return Self {to.x - from.x, to.y - from.y, to.z - from.z};
        }

        static Self
        zero()
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
        get_length() const
        {
            return sqrt(get_length_squared());
        }

        void
        normalize()
        {
            const T l2 = get_length_squared();
            if(is_equal(l2, c_int_to_t<T>(0)))
            {
                *this = Unit::up();
            }
            else
            {
                *this /= sqrt(l2);
            }
        }


        [[nodiscard]] Unit
        get_normalized() const
        {
            Self r = *this;
            r.normalize();
            return Unit::to_unit(r);
        }

        [[nodiscard]] Unit
        as_normalized() const
        {
            return Unit::to_unit(*this);
        }

        bool operator==(const vec3<T>& rhs) = delete;
    };


    ////////////////////////////////////////////////////////////////////////////////

    template <typename T>
    struct unit3 : public vec3<T>
    {
        using Self = unit3<T>;

        Self operator-() const { return Self(-this->x, -this->y, -this->z); }

        [[nodiscard]] bool
        is_valid() const
        {
            return is_equal(this->get_length_squared(), c_int_to_t<T>(1));
        }

        static Self
        x_axis()
        {
            return Self {1, 0, 0};
        }

        static Self
        y_axis()
        {
            return Self {0, 1, 0};
        }

        static Self
        z_axis()
        {
            return Self(0, 0, 1);
        }

        static Self
        up()
        {
            return y_axis();
        }

        static Self
        down()
        {
            return -y_axis();
        }

        static Self
        right()
        {
            return x_axis();
        }

        static Self
        left()
        {
            return -x_axis();
        }

        static Self
        in()
        {
            return -z_axis();
        }

        static Self
        out()
        {
            return z_axis();
        }

        static Self
        to_unit(T x, T y, T z)
        {
            return Self {x, y, z};
        }

        static Self
        to_unit(const vec3<T>& v)
        {
            return Self {v.x, v.y, v.z};
        }

        bool operator==(const unit3<T>& rhs) = delete;
    private:
        explicit unit3(T a, T b, T c) : vec3<T>(a, b, c)
        {
            ASSERT(is_valid());
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
    component_multiply(const vec3<T>& lhs, const vec3<T>& rhs)
    {
        return vec3<T>(lhs.x * rhs.x, lhs.y * rhs.y, lhs.z * rhs.z);
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

    // todo(Gustav): implement transformations


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
    operator<<(std::ostream& stream, const Scale3<T>& v)
    {
        return stream << "(" << v.x << ", " << v.y << ", " << v.z << ")";
    }


    ////////////////////////////////////////////////////////////////////////////////
    /// Typedefs


    using vec3f = vec3<float>;
    using unit3f = unit3<float>;
    using Scale3f = Scale3<float>;
}

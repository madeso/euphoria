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
        using self = scale3<T>;

        T x;
        T y;
        T z;

        T* get_data_ptr() { return &x; }

        [[nodiscard]] const T* get_data_ptr() const { return &x; }

        [[nodiscard]] T get_component_sum() const { return x + y + z; }

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
        using self = vec3<T>;

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

        self operator-() const { return self(-this->x, -this->y, -this->z); }

        T get_length_squared() const { return x * x + y * y + z * z; }

        using unit = unit3<T>;

        static self
        from_to(const vec3<T>& from, const vec3<T>& to)
        {
            return self {to.x - from.x, to.y - from.y, to.z - from.z};
        }

        static self
        zero()
        {
            return self {0, 0, 0};
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
            if(is_equal(l2, 0))
            {
                *this = unit::up();
            }
            else
            {
                *this /= sqrt(l2);
            }
        }


        [[nodiscard]] unit
        get_normalized() const
        {
            self r = *this;
            r.normalize();
            return unit::to_unit(r);
        }
    };


    ////////////////////////////////////////////////////////////////////////////////

    template <typename T>
    struct unit3 : public vec3<T>
    {
        using self = unit3<T>;

        self operator-() const { return self(-this->x, -this->y, -this->z); }

        [[nodiscard]] bool
        is_valid() const
        {
            return is_equal(this->get_length_squared(), 1);
        }

        static self
        x_axis()
        {
            return self {1, 0, 0};
        }

        static self
        y_axis()
        {
            return self {0, 1, 0};
        }

        static self
        z_axis()
        {
            return self(0, 0, 1);
        }

        static self
        up()
        {
            return y_axis();
        }

        static self
        down()
        {
            return -y_axis();
        }

        static self
        right()
        {
            return x_axis();
        }

        static self
        left()
        {
            return -x_axis();
        }

        static self
        in()
        {
            return -z_axis();
        }

        static self
        out()
        {
            return z_axis();
        }

        static self
        to_unit(T x, T y, T z)
        {
            return self {x, y, z};
        }

        static self
        to_unit(const vec3<T>& v)
        {
            return self {v.x, v.y, v.z};
        }

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

#pragma once

#include <tuple>

#include "core/interpolate.h"
#include "core/angle.h"
#include "core/typeinfo.h"
#include "core/numeric.h"
#include "core/assert.h"
#include "core/cint.h"


namespace euphoria::core
{
    struct Random;

    ////////////////////////////////////////////////////////////////////////////////
    /// Forward declarations

    // a 2d vector
    template <typename T>
    struct Vec2;


    // a 2d unit (vector)
    template <typename T>
    struct Unit2;


    ////////////////////////////////////////////////////////////////////////////////

    template <typename T>
    struct Vec2
    {
        Vec2(const T& ax, const T& ay) : x(ax), y(ay)
        {
        }

        explicit Vec2(const std::tuple<T, T>& a) : x(std::get<0>(a)), y(std::get<1>(a))
        {
        }

        [[nodiscard]] static Vec2 zero() { return {0, 0}; }

        using Self = Vec2<T>;
        using Unit = Unit2<T>;

        T x;
        T y;

        T* get_data_ptr() { return &x; }

        const T* get_data_ptr() const { return &x; }

        template <typename F>
        explicit operator Vec2<F>() const { return Vec2<F>(static_cast<F>(x), static_cast<F>(y)); }

        Self get_rotated(const Angle& a) const
        {
            const T nx = x * cos(a) - y * sin(a);
            const T ny = x * sin(a) + y * cos(a);
            return Self(nx, ny);
        }

        Self get_flipped_y() const { return Self(x, -y); }
        T get_component_sum() const { return x + y; }

        template <typename O>
        void operator+=(const Vec2<O>& rhs)
        {
            x = x + rhs.x;
            y = y + rhs.y;
        }

        template <typename O>
        void operator-=(const Vec2<O>& rhs)
        {
            x = x - rhs.x;
            y = y - rhs.y;
        }

        Self operator-() const { return Self(-x, -y); }
        T get_length_squared() const { return x * x + y * y; }


        explicit Vec2(const Unit& u) : x(u.x), y(u.y) {}

        Vec2() = default;

        [[nodiscard]] static Self
        from_to(const Vec2<T>& from, const Vec2<T>& to)
        {
            return Self {to.x - from.x, to.y - from.y};
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
        get_length() const
        {
            return sqrt(get_length_squared());
        }

        T
        normalize()
        {
            const auto l = get_length();
            *this /= l;
            return l;
        }

        std::pair<T, Unit>
        get_normalized_and_length() const
        {
            Self r = *this;
            const auto l = r.normalize();
            return std::make_pair(l, Unit {r});
        }

        Unit
        get_normalized() const
        {
            return get_normalized_and_length().second;
        }
    };


    ////////////////////////////////////////////////////////////////////////////////

    template <typename T>
    struct Unit2
    {
        using Self = Unit2<T>;

        T x;
        T y;

        T* get_data_ptr() { return &x; }

        const T* get_data_ptr() const { return &x; }

        template <typename F>
        explicit operator Unit2<F>() const { return Unit2<F>(static_cast<F>(x), static_cast<F>(y)); }

        Self get_rotated(const Angle& a) const
        {
            const T nx = x * cos(a) - y * sin(a);
            const T ny = x * sin(a) + y * cos(a);
            return Self(nx, ny);
        }

        Self get_flipped_y() const { return Self(x, -y); }
        T get_component_sum() const { return x + y; }
        Self operator-() const { return Self(-x, -y); }
        T get_length_squared() const { return x * x + y * y; }

        operator Vec2<T>() const
        {
            return Vec2<T>(x, y);
        }

        Vec2<T>
        vec() const
        {
            return Vec2<T>(x, y);
        }

        [[nodiscard]] bool
        is_valid() const
        {
            return is_equal(get_length_squared(), c_int_to_t<T>(1));
        }

        explicit Unit2(T ax, T ay) : x(ax), y(ay)
        {
            ASSERT(is_valid());
        }

    private:
        explicit Unit2(const Vec2<T>& v) : x(v.x), y(v.y)
        {
            ASSERT(is_valid());
        }

        friend struct Vec2<T>;
    };


    ////////////////////////////////////////////////////////////////////////////////

    template <typename T>
    struct Scale2
    {
        Scale2(const T& ax, const T& ay) : x(ax), y(ay)
        {
        }

        explicit Scale2(const std::tuple<T, T>& a) : x(std::get<0>(a)), y(std::get<1>(a))
        {
        }

        using Self = Scale2<T>;

        T x;
        T y;

        T* get_data_ptr() { return &x; }

        const T* get_data_ptr() const { return &x; }

        template <typename F>
        explicit operator Scale2<F>() const { return Scale2<F>(static_cast<F>(x), static_cast<F>(y)); }

        Self get_rotated(const Angle& a) const
        {
            const T nx = x * cos(a) - y * sin(a);
            const T ny = x * sin(a) + y * cos(a);
            return Self(nx, ny);
        }

        Self get_flipped_y() const { return Self(x, -y); }
        T get_component_sum() const { return x + y; }
        Self operator-() const { return Self(-x, -y); }
    };

    ////////////////////////////////////////////////////////////////////////////////
    /// Math operators

    template <typename T, typename O>
    Vec2<T>
    operator+(const Vec2<T>& lhs, const Vec2<O>& rhs)
    {
        Vec2<T> r = lhs;
        r += rhs;
        return r;
    }

    template <typename T, typename O>
    Vec2<T>
    operator-(const Vec2<T>& lhs, const Vec2<O>& rhs)
    {
        Vec2<T> r = lhs;
        r -= rhs;
        return r;
    }

    template <typename T, typename O>
    Vec2<T> operator*(const Vec2<T>& lhs, const O& rhs)
    {
        Vec2<T> r = lhs;
        r *= rhs;
        return r;
    }

    template <typename T, typename O>
    Vec2<T> operator*(const O& lhs, const Vec2<T>& rhs)
    {
        Vec2<T> r = rhs;
        r *= lhs;
        return r;
    }

    template <typename T, typename O>
    Vec2<T> operator*(const Unit2<T>& lhs, const O& rhs)
    {
        Vec2<T> r = lhs;
        r *= rhs;
        return r;
    }

    template <typename T, typename O>
    Vec2<T> operator*(const O& lhs, const Unit2<T>& rhs)
    {
        Vec2<T> r = rhs;
        r *= lhs;
        return r;
    }

    template <typename T, typename O>
    Vec2<T>
    operator/(const Vec2<T>& lhs, const O& rhs)
    {
        Vec2<T> r = lhs;
        r /= rhs;
        return r;
    }

    template <typename T>
    Vec2<T>
    component_multiply(const Vec2<T>& lhs, const Vec2<T>& rhs)
    {
        return Vec2<T>(lhs.x * rhs.x, lhs.y * rhs.y);
    }


    ////////////////////////////////////////////////////////////////////////////////
    /// Equality operators

    template <typename T>
    bool
    operator==(const Vec2<T>& lhs, const Vec2<T>& rhs)
    {
        return lhs.x == rhs.x && lhs.y == rhs.y;
    }

    template <typename T>
    bool
    operator==(const Unit2<T>& lhs, const Unit2<T>& rhs)
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

    ////////////////////////////////////////////////////////////////////////////////
    /// Transform

    template <typename T>
    struct Vec2Transform
    {
        static Vec2<T>
        transform(const Vec2<T>& from, float v, const Vec2<T> to)
        {
            return Vec2<T>(
                    FloatTransform::transform(from.x, v, to.x),
                    FloatTransform::transform(from.y, v, to.y));
        }
    };

    // todo(Gustav): add one lerp and one slerp unit transform?


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
    operator<<(S& s, const Unit2<T>& v)
    {
        s << "(" << v.x << ", " << v.y << ")";
        return s;
    }


    ////////////////////////////////////////////////////////////////////////////////
    /// Typedefs

    using Vec2f = Vec2<float>;
    using Scale2f = Scale2<float>;
    using Unit2f = Unit2<float>;
    using Vec2fTransform = Vec2Transform<float>;
    using Vec2i = Vec2<int>;
    using Scale2i = Scale2<int>;
    using Unit2i = Unit2<int>;
    using Vec2iTransform = Vec2Transform<int>;

    // util functions
    Unit2f create_random_unit(Random* random);

}


////////////////////////////////////////////////////////////////////////////////
/// Typeids

TYPEID_SETUP_TYPE(euphoria::core::Vec2f);


#pragma once

#include <tuple>

#include "assert/assert.h"

#include "core/interpolate.h"
#include "core/angle.h"
#include "core/numeric.h"
#include "core/cint.h"
#include "core/vec.base.h"


namespace euphoria::core
{
    struct Random;

    ////////////////////////////////////////////////////////////////////////////////
    /// Forward declarations

    // a 2d vector
    template <typename T>
    struct vec2;


    // a 2d unit (vector)
    template <typename T>
    struct unit2;


    ////////////////////////////////////////////////////////////////////////////////

    template <typename T>
    struct vec2
    {
        vec2(const T& ax, const T& ay) : x(ax), y(ay)
        {
        }

        explicit vec2(const std::tuple<T, T>& a) : x(std::get<0>(a)), y(std::get<1>(a))
        {
        }

        [[nodiscard]] static vec2 zero() { return {0, 0}; }

        using Self = vec2<T>;
        using Unit = unit2<T>;

        T x;
        T y;

        T* get_data_ptr() { return &x; }

        const T* get_data_ptr() const { return &x; }

        template <typename F>
        explicit operator vec2<F>() const { return vec2<F>(static_cast<F>(x), static_cast<F>(y)); }

        Self get_rotated(const angle& a) const
        {
            const T nx = x * cos(a) - y * sin(a);
            const T ny = x * sin(a) + y * cos(a);
            return Self(nx, ny);
        }

        Self get_flipped_y() const { return Self(x, -y); }
        T get_component_sum() const { return x + y; }

        template <typename O>
        void operator+=(const vec2<O>& rhs)
        {
            x = x + rhs.x;
            y = y + rhs.y;
        }

        template <typename O>
        void operator-=(const vec2<O>& rhs)
        {
            x = x - rhs.x;
            y = y - rhs.y;
        }

        Self operator-() const { return Self(-x, -y); }
        T get_length_squared() const { return x * x + y * y; }


        explicit vec2(const Unit& u) : x(u.x), y(u.y) {}

        vec2() = default;

        [[nodiscard]] static Self
        from_to(const vec2<T>& from, const vec2<T>& to)
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

        NormalizedAndLength<Unit, T>
        get_normalized_and_length() const
        {
            Self r = *this;
            const auto l = r.normalize();
            return {Unit{r}, l};
        }

        Unit
        get_normalized() const
        {
            return get_normalized_and_length().normalized;
        }
    };


    ////////////////////////////////////////////////////////////////////////////////

    template <typename T>
    struct unit2
    {
        using Self = unit2<T>;

        T x;
        T y;

        T* get_data_ptr() { return &x; }

        const T* get_data_ptr() const { return &x; }

        template <typename F>
        explicit operator unit2<F>() const { return unit2<F>(static_cast<F>(x), static_cast<F>(y)); }

        Self get_rotated(const angle& a) const
        {
            const T nx = x * cos(a) - y * sin(a);
            const T ny = x * sin(a) + y * cos(a);
            return Self(nx, ny);
        }

        Self get_flipped_y() const { return Self(x, -y); }
        T get_component_sum() const { return x + y; }
        Self operator-() const { return Self(-x, -y); }
        T get_length_squared() const { return x * x + y * y; }

        operator vec2<T>() const
        {
            return vec2<T>(x, y);
        }

        vec2<T>
        to_vec() const
        {
            return vec2<T>(x, y);
        }

        [[nodiscard]] bool
        is_valid() const
        {
            return is_equal(get_length_squared(), c_int_to_t<T>(1));
        }

        explicit unit2(T ax, T ay) : x(ax), y(ay)
        {
            ASSERT(is_valid());
        }

    private:
        explicit unit2(const vec2<T>& v) : x(v.x), y(v.y)
        {
            ASSERT(is_valid());
        }

        friend struct vec2<T>;
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

        Self get_rotated(const angle& a) const
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
    vec2<T>
    operator+(const vec2<T>& lhs, const vec2<O>& rhs)
    {
        vec2<T> r = lhs;
        r += rhs;
        return r;
    }

    template <typename T, typename O>
    vec2<T>
    operator-(const vec2<T>& lhs, const vec2<O>& rhs)
    {
        vec2<T> r = lhs;
        r -= rhs;
        return r;
    }

    template <typename T, typename O>
    bool
    operator==(const vec2<T>& lhs, const vec2<O>& rhs)
    {
        return
            lhs.x == rhs.x &&
            lhs.y == rhs.y
            ;
    }

    template <typename T, typename O>
    bool
    operator!=(const vec2<T>& lhs, const vec2<O>& rhs)
    {
        return !(lhs == rhs);
    }

    template <typename T, typename O>
    vec2<T> operator*(const vec2<T>& lhs, const O& rhs)
    {
        vec2<T> r = lhs;
        r *= rhs;
        return r;
    }

    template <typename T, typename O>
    vec2<T> operator*(const O& lhs, const vec2<T>& rhs)
    {
        vec2<T> r = rhs;
        r *= lhs;
        return r;
    }

    template <typename T, typename O>
    vec2<T> operator*(const unit2<T>& lhs, const O& rhs)
    {
        vec2<T> r = lhs;
        r *= rhs;
        return r;
    }

    template <typename T, typename O>
    vec2<T> operator*(const O& lhs, const unit2<T>& rhs)
    {
        vec2<T> r = rhs;
        r *= lhs;
        return r;
    }

    template <typename T, typename O>
    vec2<T>
    operator/(const vec2<T>& lhs, const O& rhs)
    {
        vec2<T> r = lhs;
        r /= rhs;
        return r;
    }

    template <typename T>
    vec2<T>
    component_multiply(const vec2<T>& lhs, const vec2<T>& rhs)
    {
        return vec2<T>(lhs.x * rhs.x, lhs.y * rhs.y);
    }


    ////////////////////////////////////////////////////////////////////////////////
    /// Equality operators

    template <typename T>
    bool
    operator==(const vec2<T>& lhs, const vec2<T>& rhs)
    {
        return lhs.x == rhs.x && lhs.y == rhs.y;
    }

    template <typename T>
    bool
    operator==(const unit2<T>& lhs, const unit2<T>& rhs)
    {
        return lhs.x == rhs.x && lhs.y == rhs.y;
    }


    ////////////////////////////////////////////////////////////////////////////////
    /// Functions

    template <typename T>
    T
    dot(const vec2<T>& lhs, const vec2<T>& rhs)
    {
        return lhs.x * rhs.x + lhs.y * rhs.y;
    }

    ////////////////////////////////////////////////////////////////////////////////
    /// Transform

    template <typename T>
    struct Vec2Transform
    {
        static vec2<T>
        transform(const vec2<T>& from, float v, const vec2<T> to)
        {
            return vec2<T>(
                    FloatTransform::transform(from.x, v, to.x),
                    FloatTransform::transform(from.y, v, to.y));
        }
    };

    // todo(Gustav): add one lerp and one slerp unit transform?


    ////////////////////////////////////////////////////////////////////////////////
    /// Printing

    template <typename S, typename T>
    S&
    operator<<(S& s, const vec2<T>& v)
    {
        s << "(" << v.x << ", " << v.y << ")";
        return s;
    }

    template <typename S, typename T>
    S&
    operator<<(S& s, const unit2<T>& v)
    {
        s << "(" << v.x << ", " << v.y << ")";
        return s;
    }


    ////////////////////////////////////////////////////////////////////////////////
    /// Typedefs

    using vec2f = vec2<float>;
    using Scale2f = Scale2<float>;
    using unit2f = unit2<float>;
    using Vec2fTransform = Vec2Transform<float>;
    using vec2i = vec2<int>;
    using Scale2i = Scale2<int>;
    using Unit2i = unit2<int>;
    using Vec2iTransform = Vec2Transform<int>;

    // util functions
    unit2f create_random_unit(Random* random);

}


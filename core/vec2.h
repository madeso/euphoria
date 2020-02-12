#ifndef CORE_VEC2_H
#define CORE_VEC2_H

#include <cmath>
#include <tuple>

#include "core/interpolate.h"
#include "core/angle.h"
#include "core/typeinfo.h"
#include "core/numeric.h"
#include "core/assert.h"

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
    /// Shared implementations

#define VEC2_CONSTRUCTOR(VEC, T)                                               \
    VEC(const T& ax, const T& ay) : x(ax), y(ay) {}                            \
                                                                               \
    explicit VEC(const std::tuple<T, T>& a) : x(std::get<0>(a)), y(std::get<1>(a)) {} \
                                                                               \
    [[nodiscard]] static VEC Zero()                                            \
    {                                                                          \
        return {0, 0};                                                         \
    }


#define VEC2_COMMON_MEMBERS(VEC, T)                                            \
    typedef VEC<T> Self;                                                       \
    T              x;                                                          \
    T              y;                                                          \
                                                                               \
    T* GetDataPtr()                                                            \
    {                                                                          \
        return &x;                                                             \
    }                                                                          \
                                                                               \
    const T* GetDataPtr() const                                                \
    {                                                                          \
        return &x;                                                             \
    }                                                                          \
                                                                               \
    template <typename F>                                                      \
    VEC<F> StaticCast() const                                                  \
    {                                                                          \
        return VEC<F>(static_cast<F>(x), static_cast<F>(y));                   \
    }                                                                          \
                                                                               \
    Self GetRotated(const Angle& a) const                                      \
    {                                                                          \
        const T nx = x * Cos(a) - y * Sin(a);                                  \
        const T ny = x * Sin(a) + y * Cos(a);                                  \
        return Self(nx, ny);                                                   \
    }                                                                          \
                                                                               \
    Self GetFlippedY() const                                                   \
    {                                                                          \
        return Self(x, -y);                                                    \
    }                                                                          \
                                                                               \
    T                                                                          \
    GetComponentSum() const                                                    \
    {                                                                          \
        return x + y;                                                          \
    }


#define VEC2_SELF_ADD_SUBTRACT(VEC)                                            \
    template <typename O>                                                      \
    void operator+=(const VEC<O>& rhs)                                         \
    {                                                                          \
        x = x + rhs.x;                                                         \
        y = y + rhs.y;                                                         \
    }                                                                          \
                                                                               \
    template <typename O>                                                      \
    void operator-=(const VEC<O>& rhs)                                         \
    {                                                                          \
        x = x - rhs.x;                                                         \
        y = y - rhs.y;                                                         \
    }

#define VEC2_INVERT_SELF()                                                     \
    Self operator-() const                                                     \
    {                                                                          \
        return Self(-x, -y);                                                   \
    }

#define VEC2_LENGTH_SQUARED(T)                                                 \
    T GetLengthSquared() const                                                 \
    {                                                                          \
        return x * x + y * y;                                                  \
    }


    ////////////////////////////////////////////////////////////////////////////////

    template <typename T>
    struct vec2
    {
        VEC2_CONSTRUCTOR(vec2, T)
        VEC2_COMMON_MEMBERS(vec2, T)
        VEC2_SELF_ADD_SUBTRACT(vec2)
        VEC2_INVERT_SELF()
        VEC2_LENGTH_SQUARED(T)

        using Unit = unit2<T>;

        explicit vec2(const Unit& u) : x(u.x), y(u.y) {}

        vec2() = default;

        [[nodiscard]] static Self
        FromTo(const vec2<T>& from, const vec2<T>& to)
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
        GetLength() const
        {
            return sqrt(GetLengthSquared());
        }

        T
        Normalize()
        {
            const auto l = GetLength();
            *this /= l;
            return l;
        }

        std::pair<T, Unit>
        GetNormalizedVec() const
        {
            Self       r = *this;
            const auto l = r.Normalize();
            return std::make_pair(l, Unit {r});
        }

        Unit
        GetNormalized() const
        {
            return GetNormalizedVec().second;
        }
    };


    ////////////////////////////////////////////////////////////////////////////////

    template <typename T>
    struct unit2
    {
        VEC2_COMMON_MEMBERS(unit2, T)
        VEC2_INVERT_SELF()
        VEC2_LENGTH_SQUARED(T)

        operator vec2<T>() const
        {
            return vec2<T>(x, y);
        }

        vec2<T>
        vec() const
        {
            return vec2<T>(x, y);
        }

        bool
        IsValid() const
        {
            return IsEqual(GetLengthSquared(), 1);
        }

        explicit unit2(T ax, T ay) : x(ax), y(ay)
        {
            ASSERT(IsValid());
        }

    private:
        explicit unit2(const vec2<T>& v) : x(v.x), y(v.y)
        {
            ASSERT(IsValid());
        }

        friend struct vec2<T>;
    };


    ////////////////////////////////////////////////////////////////////////////////

    template <typename T>
    struct scale2
    {
        VEC2_CONSTRUCTOR(scale2, T)
        VEC2_COMMON_MEMBERS(scale2, T)
        VEC2_INVERT_SELF()
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
    ComponentMultiply(const vec2<T>& lhs, const vec2<T>& rhs)
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
        Transform(const vec2<T>& from, float v, const vec2<T> to)
        {
            return vec2<T>(
                    FloatTransform::Transform(from.x, v, to.x),
                    FloatTransform::Transform(from.y, v, to.y));
        }
    };

    // todo: add one lerp and one slerp unit transform?


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

    typedef vec2<float>          vec2f;
    typedef scale2<float>        scale2f;
    typedef unit2<float>         unit2f;
    typedef Vec2Transform<float> Vec2fTransform;

    typedef vec2<int>          vec2i;
    typedef scale2<int>        scale2i;
    typedef unit2<int>         unit2i;
    typedef Vec2Transform<int> Vec2iTransform;

    // util functions
    unit2f RandomUnit(Random* random);

}  // namespace euphoria::core


////////////////////////////////////////////////////////////////////////////////
/// Typeids

TYPEID_SETUP_TYPE(euphoria::core::vec2f);

#endif  // CORE_VEC2_H

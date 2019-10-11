#ifndef CORE_SIZE_H
#define CORE_SIZE_H

#include "core/vec2.h"
#include "core/numeric.h"

namespace euphoria::core
{
    template <typename T>
    struct Size
    {
        typedef Size<T> Self;

        T width;
        T height;

        Size() : width(-1), height(-1) {}

        [[nodiscard]] static Self
        FromWidthHeight(T w, T h)
        {
            return Self(w, h);
        }

        [[nodiscard]] static Self
        FromHeightWidth(T h, T w)
        {
            return Self(w, h);
        }

        [[nodiscard]] static Self
        FromSquare(T s)
        {
            return Self(s, s);
        }

        void
        operator+=(const Self& rhs)
        {
            width += rhs.width;
            height += rhs.height;
        }

        void
        operator-=(const Self& rhs)
        {
            width -= rhs.width;
            height -= rhs.height;
        }

        void
        operator*=(const T& rhs)
        {
            width *= rhs;
            height *= rhs;
        }

        void
        operator/=(const T& rhs)
        {
            width /= rhs;
            height /= rhs;
        }

        vec2<T>
        CalculateCenterOffsetFor(const Self& o)
        {
            const T x = (width - o.width) / 2;
            const T y = (height - o.height) / 2;
            return vec2<T>(x, y);
        }

    private:
        Size(T w, T h) : width(w), height(h) {}
    };

    template <typename T>
    Size<T>
    Min(const Size<T> lhs, const Size<T> rhs)
    {
        return Size<T>::FromWidthHeight(
                Min(lhs.width, rhs.width),
                Min(lhs.height, rhs.height));
    }

    template <typename T>
    Size<T>
    Max(const Size<T> lhs, const Size<T> rhs)
    {
        return Size<T>::FromWidthHeight(
                Max(lhs.width, rhs.width),
                Max(lhs.height, rhs.height));
    }

    template <typename T>
    Size<T>
    operator+(const Size<T>& lhs, const Size<T>& rhs)
    {
        Size<T> t = lhs;
        t += rhs;
        return t;
    }

    template <typename T>
    Size<T>
    operator-(const Size<T>& lhs, const Size<T>& rhs)
    {
        Size<T> t = lhs;
        t -= rhs;
        return t;
    }

    template <typename T>
    Size<T> operator*(const Size<T>& lhs, const T& rhs)
    {
        Size<T> t = lhs;
        t *= rhs;
        return t;
    }

    template <typename T>
    Size<T> operator*(const T& lhs, const Size<T>& rhs)
    {
        Size<T> t = rhs;
        t *= lhs;
        return t;
    }

    template <typename T>
    Size<T>
    operator/(const Size<T>& lhs, const T& rhs)
    {
        Size<T> t = lhs;
        t /= rhs;
        return t;
    }

    template <typename S, typename T>
    S&
    operator<<(S& s, const Size<T>& r)
    {
        s << r.width << " x " << r.height << ")";
        return s;
    }

    typedef Size<int>   Sizei;
    typedef Size<float> Sizef;
}  // namespace euphoria::core

#endif  // CORE_SIZE_H

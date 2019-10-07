#ifndef CORE_SIZE_H
#define CORE_SIZE_H

#include "core/vec2.h"
#include "core/numeric.h"

namespace euphoria::core
{
    template <typename T>
    class Size
    {
    public:
        typedef Size<T> Self;
        Size() : width_(-1), height_(-1) {}
        static Self
        FromWidthHeight(T w, T h)
        {
            return Self(w, h);
        }
        static Self
        FromHeightWidth(T h, T w)
        {
            return Self(w, h);
        }
        static Self
        FromSquare(T s)
        {
            return Self(s, s);
        }

        T
        GetWidth() const
        {
            return width_;
        }

        void
        SetWidth(T w)
        {
            width_ = w;
        }

        T
        GetHeight() const
        {
            return height_;
        }

        void
        SetHeight(T h)
        {
            height_ = h;
        }

        void
        operator+=(const Self& rhs)
        {
            width_ += rhs.width_;
            height_ += rhs.height_;
        }

        void
        operator-=(const Self& rhs)
        {
            width_ -= rhs.width_;
            height_ -= rhs.height_;
        }

        void
        operator*=(const T& rhs)
        {
            width_ *= rhs;
            height_ *= rhs;
        }

        void
        operator/=(const T& rhs)
        {
            width_ /= rhs;
            height_ /= rhs;
        }

        /*
    operator vec2<T>() const
    {
      return vec2<T>(width_, height_);
    }
    */

        vec2<T>
        CalculateCenterOffsetFor(const Self& o)
        {
            const T x = (GetWidth() - o.GetWidth()) / 2;
            const T y = (GetHeight() - o.GetHeight()) / 2;
            return vec2<T>(x, y);
        }

    private:
        Size(T w, T h) : width_(w), height_(h) {}
        T width_;
        T height_;
    };

    template <typename T>
    Size<T>
    Min(const Size<T> lhs, const Size<T> rhs)
    {
        return Size<T>::FromWidthHeight(
                Min(lhs.GetWidth(), rhs.GetWidth()),
                Min(lhs.GetHeight(), rhs.GetHeight()));
    }

    template <typename T>
    Size<T>
    Max(const Size<T> lhs, const Size<T> rhs)
    {
        return Size<T>::FromWidthHeight(
                Max(lhs.GetWidth(), rhs.GetWidth()),
                Max(lhs.GetHeight(), rhs.GetHeight()));
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
        s << r.GetWidth() << " x " << r.GetHeight() << ")";
        return s;
    }

    typedef Size<int>   Sizei;
    typedef Size<float> Sizef;
}  // namespace euphoria::core

#endif  // CORE_SIZE_H

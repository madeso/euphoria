#pragma once

#include "core/vec2.h"
#include "core/numeric.h"
#include "core/stringutils.h"
#include "core/default_parse.h"


namespace euphoria::core
{
    template <typename T>
    struct Size
    {
        using Self = Size<T>;

        T width;
        T height;

        Size()
            : width(-1)
            , height(-1)
        {
        }

        [[nodiscard]]
        static
        Self
        FromWidthHeight(T w, T h)
        {
            return Self(w, h);
        }

        [[nodiscard]]
        static
        Self
        FromHeightWidth(T h, T w)
        {
            return Self(w, h);
        }

        [[nodiscard]]
        static
        Self
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


    template<typename T>
    struct custom_argparser<Size<T>>
    {
        enum { value = 1 };

        static
        std::string
        ToString(const Size<T>& s)
        {
            std::ostringstream ss;
            ss << s.width << "x" << s.height;
            return ss.str();
        }

        static
        Result<Size<T>>
        Parse(const std::string& value)
        {
            using R = Result<Size<T>>;
            const auto values = Split(value, 'x');
            const auto xes = values.size();
            if(xes != 2)
            {
                if(xes < 2)
                {
                    return R::False
                    (
                        Str{} << value << " contains less than one x"
                    );
                }
                else
                {
                    return R::False
                    (
                        Str{}
                            << value << " contains more than one x: "
                            << (xes-1)
                    );
                }
            }

            auto parse_hs = [&](int index)
            {
                return argparse::default_parse_function<T>
                (
                    Trim(values[index])
                );
            };
            const auto lhs = parse_hs(0);
            const auto rhs = parse_hs(1);

            if(!lhs) { return R::False(lhs.Error()); }
            if(!rhs) { return R::False(rhs.Error()); }

            return R::True(Size<T>::FromWidthHeight(*lhs, *rhs));
        }
    };

    template <typename T>
    Size<T>
    Min(const Size<T> lhs, const Size<T> rhs)
    {
        return Size<T>::FromWidthHeight(
                Min(lhs.width, rhs.width), Min(lhs.height, rhs.height));
    }

    template <typename T>
    Size<T>
    Max(const Size<T> lhs, const Size<T> rhs)
    {
        return Size<T>::FromWidthHeight(
                Max(lhs.width, rhs.width), Max(lhs.height, rhs.height));
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
        s << "(" << r.width << " x " << r.height << ")";
        return s;
    }

    using Sizei = Size<int>;
    using Sizef = Size<float>;
}

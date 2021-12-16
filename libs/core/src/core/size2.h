#pragma once

#include "core/vec2.h"
#include "core/numeric.h"
#include "core/stringutils.h"
#include "core/default_parse.h"


namespace euphoria::core
{
    template <typename T>
    struct Size2
    {
        using Self = Size2<T>;

        T width;
        T height;

        Size2()
            : width(-1)
            , height(-1)
        {
        }

        [[nodiscard]]
        static
        Self
        create_from_width_height(T w, T h)
        {
            return Self(w, h);
        }

        [[nodiscard]]
        static
        Self
        create_from_height_width(T h, T w)
        {
            return Self(w, h);
        }

        [[nodiscard]]
        static
        Self
        create_from_square(T s)
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

        Vec2<T>
        calculate_center_offset_for(const Self& o)
        {
            const T x = (width - o.width) / 2;
            const T y = (height - o.height) / 2;
            return Vec2<T>(x, y);
        }

    private:
        Size2(T w, T h) : width(w), height(h) {}
    };


    template<typename T>
    struct CustomArgparser<Size2<T>>
    {
        enum { value = 1 };

        static
        std::string
        to_string(const Size2<T>& s)
        {
            std::ostringstream ss;
            ss << s.width << "x" << s.height;
            return ss.str();
        }

        static
        Result<Size2<T>>
        parse(const std::string& value)
        {
            using R = Result<Size2<T>>;
            const auto values = split(value, 'x');
            const auto xes = values.size();
            if(xes != 2)
            {
                if(xes < 2)
                {
                    return R::create_error
                    (
                        StringBuilder{} << value << " contains less than one x"
                    );
                }
                else
                {
                    return R::create_error
                    (
                        StringBuilder{}
                            << value << " contains more than one x: "
                            << (xes-1)
                    );
                }
            }

            auto parse_hs = [&](int index)
            {
                return argparse::default_parse_function<T>
                (
                    trim(values[index])
                );
            };
            const auto lhs = parse_hs(0);
            const auto rhs = parse_hs(1);

            if(!lhs) { return R::create_error(lhs.get_error()); }
            if(!rhs) { return R::create_error(rhs.get_error()); }

            return R::create_value(Size2<T>::create_from_width_height(*lhs, *rhs));
        }
    };

    template <typename T>
    Size2<T>
    min(const Size2<T> lhs, const Size2<T> rhs)
    {
        return Size2<T>::create_from_width_height(
                min(lhs.width, rhs.width), min(lhs.height, rhs.height));
    }

    template <typename T>
    Size2<T>
    max(const Size2<T> lhs, const Size2<T> rhs)
    {
        return Size2<T>::create_from_width_height(
                max(lhs.width, rhs.width), max(lhs.height, rhs.height));
    }

    template <typename T>
    Size2<T>
    operator+(const Size2<T>& lhs, const Size2<T>& rhs)
    {
        Size2<T> t = lhs;
        t += rhs;
        return t;
    }

    template <typename T>
    Size2<T>
    operator-(const Size2<T>& lhs, const Size2<T>& rhs)
    {
        Size2<T> t = lhs;
        t -= rhs;
        return t;
    }

    template <typename T>
    Size2<T> operator*(const Size2<T>& lhs, const T& rhs)
    {
        Size2<T> t = lhs;
        t *= rhs;
        return t;
    }

    template <typename T>
    Size2<T> operator*(const T& lhs, const Size2<T>& rhs)
    {
        Size2<T> t = rhs;
        t *= lhs;
        return t;
    }

    template <typename T>
    Size2<T>
    operator/(const Size2<T>& lhs, const T& rhs)
    {
        Size2<T> t = lhs;
        t /= rhs;
        return t;
    }

    template <typename S, typename T>
    S&
    operator<<(S& s, const Size2<T>& r)
    {
        s << "(" << r.width << " x " << r.height << ")";
        return s;
    }

    using Size2i = Size2<int>;
    using Size2f = Size2<float>;
}

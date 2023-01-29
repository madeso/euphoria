#pragma once

#include "core/vec2.h"
#include "core/numeric.h"
#include "core/stringutils.h"
#include "core/default_parse.h"


namespace euphoria::core
{
    template <typename T>
    struct size2
    {
        using Self = size2<T>;

        T width;
        T height;

        size2()
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

        vec2<T>
        calculate_center_offset_for(const Self& o)
        {
            const T x = (width - o.width) / 2;
            const T y = (height - o.height) / 2;
            return vec2<T>(x, y);
        }

    private:
        size2(T w, T h) : width(w), height(h) {}
    };


    template<typename T>
    struct CustomArgparser<size2<T>>
    {
        enum { value = 1 };

        static
        std::string
        to_string(const size2<T>& s)
        {
            return "{}x{}"_format(s.width, s.height);
        }

        static
        Result<size2<T>>
        parse(const std::string& value)
        {
            using R = Result<size2<T>>;
            const auto values = split(value, 'x');
            const auto xes = values.size();
            if(xes != 2)
            {
                if(xes < 2)
                {
                    return R::create_error
                    (
                        "{} contains less than one x"_format(value)
                    );
                }
                else
                {
                    return R::create_error
                    (
                        "{} contains more than one x: {}"_format(value, xes-1)
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

            return R::create_value(size2<T>::create_from_width_height(*lhs, *rhs));
        }
    };

    template <typename T>
    size2<T>
    min(const size2<T> lhs, const size2<T> rhs)
    {
        return size2<T>::create_from_width_height(
                min(lhs.width, rhs.width), min(lhs.height, rhs.height));
    }

    template <typename T>
    size2<T>
    max(const size2<T> lhs, const size2<T> rhs)
    {
        return size2<T>::create_from_width_height(
                max(lhs.width, rhs.width), max(lhs.height, rhs.height));
    }

    template <typename T>
    size2<T>
    operator+(const size2<T>& lhs, const size2<T>& rhs)
    {
        size2<T> t = lhs;
        t += rhs;
        return t;
    }

    template <typename T>
    size2<T>
    operator-(const size2<T>& lhs, const size2<T>& rhs)
    {
        size2<T> t = lhs;
        t -= rhs;
        return t;
    }

    template <typename T>
    size2<T> operator*(const size2<T>& lhs, const T& rhs)
    {
        size2<T> t = lhs;
        t *= rhs;
        return t;
    }

    template <typename T>
    size2<T> operator*(const T& lhs, const size2<T>& rhs)
    {
        size2<T> t = rhs;
        t *= lhs;
        return t;
    }

    template <typename T>
    size2<T>
    operator/(const size2<T>& lhs, const T& rhs)
    {
        size2<T> t = lhs;
        t /= rhs;
        return t;
    }

    template <typename S, typename T>
    S&
    operator<<(S& s, const size2<T>& r)
    {
        s << "(" << r.width << " x " << r.height << ")";
        return s;
    }

    using size2i = size2<int>;
    using size2f = size2<float>;
}

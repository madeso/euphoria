#pragma once

#include "core/vec2.h"
#include "core/numeric.h"
#include "core/stringutils.h"
#include "core/default_parse.h"


namespace euphoria::core
{
    template <typename T>
    struct size
    {
        using self = size<T>;

        T width;
        T height;

        size()
            : width(-1)
            , height(-1)
        {
        }

        [[nodiscard]]
        static
        self
        create_from_width_height(T w, T h)
        {
            return self(w, h);
        }

        [[nodiscard]]
        static
        self
        create_from_height_width(T h, T w)
        {
            return self(w, h);
        }

        [[nodiscard]]
        static
        self
        create_from_square(T s)
        {
            return self(s, s);
        }

        void
        operator+=(const self& rhs)
        {
            width += rhs.width;
            height += rhs.height;
        }

        void
        operator-=(const self& rhs)
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
        calculate_center_offset_for(const self& o)
        {
            const T x = (width - o.width) / 2;
            const T y = (height - o.height) / 2;
            return vec2<T>(x, y);
        }

    private:
        size(T w, T h) : width(w), height(h) {}
    };


    template<typename T>
    struct custom_argparser<size<T>>
    {
        enum { value = 1 };

        static
        std::string
        to_string(const size<T>& s)
        {
            std::ostringstream ss;
            ss << s.width << "x" << s.height;
            return ss.str();
        }

        static
        result<size<T>>
        parse(const std::string& value)
        {
            using R = result<size<T>>;
            const auto values = split(value, 'x');
            const auto xes = values.size();
            if(xes != 2)
            {
                if(xes < 2)
                {
                    return R::create_error
                    (
                        string_builder{} << value << " contains less than one x"
                    );
                }
                else
                {
                    return R::create_error
                    (
                        string_builder{}
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

            return R::create_value(size<T>::create_from_width_height(*lhs, *rhs));
        }
    };

    template <typename T>
    size<T>
    min(const size<T> lhs, const size<T> rhs)
    {
        return size<T>::create_from_width_height(
                min(lhs.width, rhs.width), min(lhs.height, rhs.height));
    }

    template <typename T>
    size<T>
    max(const size<T> lhs, const size<T> rhs)
    {
        return size<T>::create_from_width_height(
                max(lhs.width, rhs.width), max(lhs.height, rhs.height));
    }

    template <typename T>
    size<T>
    operator+(const size<T>& lhs, const size<T>& rhs)
    {
        size<T> t = lhs;
        t += rhs;
        return t;
    }

    template <typename T>
    size<T>
    operator-(const size<T>& lhs, const size<T>& rhs)
    {
        size<T> t = lhs;
        t -= rhs;
        return t;
    }

    template <typename T>
    size<T> operator*(const size<T>& lhs, const T& rhs)
    {
        size<T> t = lhs;
        t *= rhs;
        return t;
    }

    template <typename T>
    size<T> operator*(const T& lhs, const size<T>& rhs)
    {
        size<T> t = rhs;
        t *= lhs;
        return t;
    }

    template <typename T>
    size<T>
    operator/(const size<T>& lhs, const T& rhs)
    {
        size<T> t = lhs;
        t /= rhs;
        return t;
    }

    template <typename S, typename T>
    S&
    operator<<(S& s, const size<T>& r)
    {
        s << "(" << r.width << " x " << r.height << ")";
        return s;
    }

    using Sizei = size<int>;
    using Sizef = size<float>;
}

#include "base/size2.h"

#include "base/numparse.h"


namespace eu
{
    size2f::size2f()
        : width(-1)
        , height(-1)
    {
    }

    [[nodiscard]] size2f
    size2f::create_from_width_height(float w, float h)
    {
        return {w, h};
    }

    [[nodiscard]] size2f
    size2f::create_from_height_width(float h, float w)
    {
        return {w, h};
    }

    [[nodiscard]] size2f
    size2f::create_from_square(float s)
    {
        return {s, s};
    }

    void
    size2f::operator+=(const size2f& rhs)
    {
        width += rhs.width;
        height += rhs.height;
    }

    void
    size2f::operator-=(const size2f& rhs)
    {
        width -= rhs.width;
        height -= rhs.height;
    }

    void
    size2f::operator*=(float rhs)
    {
        width *= rhs;
        height *= rhs;
    }

    void
    size2f::operator/=(float rhs)
    {
        width /= rhs;
        height /= rhs;
    }

    vec2f
    size2f::calc_center_offset_for(const size2f& o) const
    {
        const float x = (width - o.width) / 2;
        const float y = (height - o.height) / 2;
        return {x, y};
    }

    size2f::size2f(float w, float h)
        : width(w)
        , height(h)
    {
    }


    ///////////////////////////////////////////////////////////////////////////

    namespace
    {
        template<typename T>
        Result<T> parse_number(const std::string& str, const char* const name)
        {
            const auto v = locale_parse_generic<T>
            (
                trim(str)
            );
            if (v)
            {
                return Result<T>::create_value(*v);
            }
            else
            {
                return Result<T>::create_error(fmt::format("{} is not a valid number for {}", str, name));
            }
        };
    }


    std::string
    StringParser<size2f>::to_string(const size2f& s)
    {
        return fmt::format("{}x{}", s.width, s.height);
    }

    Result<size2f>
    StringParser<size2f>::parse(const std::string& value)
    {
        using R = Result<size2f>;
        const auto values = split(value, 'x');
        const auto xes = values.size();
        if(xes != 2)
        {
            if(xes < 2)
            {
                return R::create_error
                (
                    fmt::format("{} contains less than one x", value)
                );
            }
            else
            {
                return R::create_error
                (
                    fmt::format("{} contains more than one x: {}", value, xes-1)
                );
            }
        }

        const auto lhs = parse_number<float>(values[0], "lhs");
        const auto rhs = parse_number<float>(values[1], "rhs");

        if(!lhs) { return R::create_error(lhs.get_error()); }
        if(!rhs) { return R::create_error(rhs.get_error()); }

        return R::create_value(size2f::create_from_width_height(*lhs, *rhs));
    }

    ///////////////////////////////////////////////////////////////////////////

    size2f
    min(const size2f lhs, const size2f rhs)
    {
        return size2f::create_from_width_height
        (
            min(lhs.width, rhs.width), min(lhs.height, rhs.height)
        );
    }

    size2f
    max(const size2f lhs, const size2f rhs)
    {
        return size2f::create_from_width_height
        (
            max(lhs.width, rhs.width), max(lhs.height, rhs.height)
        );
    }

    size2f
    operator+(const size2f& lhs, const size2f& rhs)
    {
        size2f t = lhs;
        t += rhs;
        return t;
    }

    size2f
    operator-(const size2f& lhs, const size2f& rhs)
    {
        size2f t = lhs;
        t -= rhs;
        return t;
    }

    size2f operator*(const size2f& lhs, float rhs)
    {
        size2f t = lhs;
        t *= rhs;
        return t;
    }

    size2f operator*(float lhs, const size2f& rhs)
    {
        size2f t = rhs;
        t *= lhs;
        return t;
    }

    size2f
    operator/(const size2f& lhs, float rhs)
    {
        size2f t = lhs;
        t /= rhs;
        return t;
    }

    std::string to_string(const size2f& r)
    {
        return fmt::format("({} x {})", r.width, r.height);
    }


    ///////////////////////////////////////////////////////////////////////////

    
    size2i::size2i()
        : width(-1)
        , height(-1)
    {
    }

    [[nodiscard]] size2i
    size2i::create_from_width_height(int w, int h)
    {
        return {w, h};
    }

    [[nodiscard]] size2i
    size2i::create_from_height_width(int h, int w)
    {
        return {w, h};
    }

    [[nodiscard]] size2i
    size2i::create_from_square(int s)
    {
        return {s, s};
    }

    void
    size2i::operator+=(const size2i& rhs)
    {
        width += rhs.width;
        height += rhs.height;
    }

    void
    size2i::operator-=(const size2i& rhs)
    {
        width -= rhs.width;
        height -= rhs.height;
    }

    void
    size2i::operator*=(int rhs)
    {
        width *= rhs;
        height *= rhs;
    }

    vec2i
    size2i::calc_center_offset_for(const size2i& o) const
    {
        const int x = (width - o.width) / 2;
        const int y = (height - o.height) / 2;
        return {x, y};
    }


    size2i::size2i(int w, int h)
        : width(w)
        , height(h)
    {
    }

    ///////////////////////////////////////////////////////////////////////////


    std::string
    StringParser<size2i>::to_string(const size2i& s)
    {
        return fmt::format("{}x{}", s.width, s.height);
    }

    Result<size2i>
    StringParser<size2i>::parse(const std::string& value)
    {
        using R = Result<size2i>;
        const auto values = split(value, 'x');
        const auto xes = values.size();
        if(xes != 2)
        {
            if(xes < 2)
            {
                return R::create_error
                (
                    fmt::format("{} contains less than one x", value)
                );
            }
            else
            {
                return R::create_error
                (
                    fmt::format("{} contains more than one x: {}", value, xes-1)
                );
            }
        }

        const auto lhs = parse_number<int>(values[0], "lhs");
        const auto rhs = parse_number<int>(values[1], "rhs");

        if(!lhs) { return R::create_error(lhs.get_error()); }
        if(!rhs) { return R::create_error(rhs.get_error()); }

        return R::create_value(size2i::create_from_width_height(*lhs, *rhs));
    }
    
    ///////////////////////////////////////////////////////////////////////////

    size2i
    min(const size2i lhs, const size2i rhs)
    {
        return size2i::create_from_width_height
        (
            min(lhs.width, rhs.width), min(lhs.height, rhs.height)
        );
    }

    size2i
    max(const size2i lhs, const size2i rhs)
    {
        return size2i::create_from_width_height
        (
            max(lhs.width, rhs.width), max(lhs.height, rhs.height)
        );
    }

    size2i
    operator+(const size2i& lhs, const size2i& rhs)
    {
        size2i t = lhs;
        t += rhs;
        return t;
    }

    size2i
    operator-(const size2i& lhs, const size2i& rhs)
    {
        size2i t = lhs;
        t -= rhs;
        return t;
    }

    size2i operator*(const size2i& lhs, int rhs)
    {
        size2i t = lhs;
        t *= rhs;
        return t;
    }

    size2i operator*(int lhs, const size2i& rhs)
    {
        size2i t = rhs;
        t *= lhs;
        return t;
    }

    std::string to_string(const size2i& r)
    {
        return fmt::format("({} x {})", r.width, r.height);
    }
}



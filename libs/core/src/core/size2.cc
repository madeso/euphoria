#include "core/size2.h"


namespace euphoria::core
{

    size2f::size2f()
        : width(-1)
        , height(-1)
    {
    }

    [[nodiscard]] size2f
    size2f::create_from_width_height(float w, float h)
    {
        return size2f(w, h);
    }

    [[nodiscard]] size2f
    size2f::create_from_height_width(float h, float w)
    {
        return size2f(w, h);
    }

    [[nodiscard]] size2f
    size2f::create_from_square(float s)
    {
        return size2f(s, s);
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
    size2f::calculate_center_offset_for(const size2f& o)
    {
        const float x = (width - o.width) / 2;
        const float y = (height - o.height) / 2;
        return vec2f(x, y);
    }

    size2f::size2f(float w, float h)
        : width(w)
        , height(h)
    {
    }


    ///////////////////////////////////////////////////////////////////////////


    std::string
    CustomArgparser<size2f>::to_string(const size2f& s)
    {
        return "{}x{}"_format(s.width, s.height);
    }

    Result<size2f>
    CustomArgparser<size2f>::parse(const std::string& value)
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
            return argparse::default_parse_function<float>
            (
                trim(values[index])
            );
        };
        const auto lhs = parse_hs(0);
        const auto rhs = parse_hs(1);

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

    std::ostream&
    operator<<(std::ostream& s, const size2f& r)
    {
        s << "(" << r.width << " x " << r.height << ")";
        return s;
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
        return size2i(w, h);
    }

    [[nodiscard]] size2i
    size2i::create_from_height_width(int h, int w)
    {
        return size2i(w, h);
    }

    [[nodiscard]] size2i
    size2i::create_from_square(int s)
    {
        return size2i(s, s);
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
    size2i::calculate_center_offset_for(const size2i& o)
    {
        const int x = (width - o.width) / 2;
        const int y = (height - o.height) / 2;
        return vec2i(x, y);
    }


    size2i::size2i(int w, int h)
        : width(w)
        , height(h)
    {
    }

    ///////////////////////////////////////////////////////////////////////////


    std::string
    CustomArgparser<size2i>::to_string(const size2i& s)
    {
        return "{}x{}"_format(s.width, s.height);
    }

    Result<size2i>
    CustomArgparser<size2i>::parse(const std::string& value)
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
            return argparse::default_parse_function<int>
            (
                trim(values[index])
            );
        };
        const auto lhs = parse_hs(0);
        const auto rhs = parse_hs(1);

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

    std::ostream&
    operator<<(std::ostream& s, const size2i& r)
    {
        s << "(" << r.width << " x " << r.height << ")";
        return s;
    }
}



#include "base/vec2.h"

#include <cmath>

#include "base/angle.h"
#include "assert/assert.h"


namespace eu
{
    v2::v2(const std::tuple<float, float>& a)
        : x(std::get<0>(a))
        , y(std::get<1>(a))
    {
    }

    v2::v2(const n2& u)
        : x(u.x)
        , y(u.y)
    {
    }


    float*
    v2::get_data_ptr()
    {
        return &x;
    }

    const float*
    v2::get_data_ptr() const
    {
        return &x;
    }

    v2
    v2::get_rotated(const An& a) const
    {
        const float nx = x * cos(a) - y * sin(a);
        const float ny = x * sin(a) + y * cos(a);
        return {nx, ny};
    }

    v2
    v2::get_flipped_y() const
    {
        return {x, -y};
    }

    void
    v2::operator+=(const v2& rhs)
    {
        x = x + rhs.x;
        y = y + rhs.y;
    }

    void
    v2::operator-=(const v2& rhs)
    {
        x = x - rhs.x;
        y = y - rhs.y;
    }

    v2
    v2::operator-() const
    {
        return {-x, -y};
    }

    float
    v2::get_length_squared() const
    {
        return x * x + y * y;
    }


    [[nodiscard]] v2
    v2::from_to(const v2& from, const v2& to)
    {
        return {to.x - from.x, to.y - from.y};
    }

    v2
    v2::from(const n2& n)
    {
        return {n.x, n.y};
    }

    void
    v2::operator/=(float rhs)
    {
        x = x / rhs;
        y = y / rhs;
    }

    void
    v2::operator*=(float rhs)
    {
        x = x * rhs;
        y = y * rhs;
    }

    float
    v2::get_length() const
    {
        return std::sqrt(get_length_squared());
    }

    bool
    v2::normalize()
    {
        const auto l2 = get_length();
        if (is_equal(l2, 0.0f))
        {
            *this = v2{ 1.0f, 0.0f };
            return false;
        }

        *this /= l2;
        return true;
    }

    std::optional<n2>
    v2::get_normalized() const
    {
        v2 r = *this;
        if(r.normalize() == false)
        {
            return std::nullopt;
        }
        return n2{r};
    }

    const float*
    n2::get_data_ptr() const
    {
        return &x;
    }

    n2
    n2::get_rotated(const An& a) const
    {
        const float nx = x * cos(a) - y * sin(a);
        const float ny = x * sin(a) + y * cos(a);
        return n2{nx, ny};
    }

    n2
    n2::get_flipped_y() const
    {
        return n2{x, -y};
    }

    n2
    n2::operator-() const
    {
        return n2{-x, -y};
    }

    float
    length_squared_from(const n2& n)
    {
        return n.x * n.x + n.y * n.y;
    }

    [[nodiscard]] bool
    n2::is_valid() const
    {
        return is_equal(length_squared_from(*this), 1.0f);
    }

    n2::n2(float ax, float ay) : x(ax), y(ay)
    {
        ASSERT(is_valid());
    }

    n2::n2(const v2& v) : x(v.x), y(v.y)
    {
        ASSERT(is_valid());
    }

    v2 operator+(const v2& lhs, const v2& rhs)
    {
        v2 r = lhs;
        r += rhs;
        return r;
    }

    v2 operator-(const v2& lhs, const v2& rhs)
    {
        v2 r = lhs;
        r -= rhs;
        return r;
    }

    v2 operator*(const v2& lhs, float rhs)
    {
        v2 r = lhs;
        r *= rhs;
        return r;
    }

    v2 operator*(float lhs, const v2& rhs)
    {
        v2 r = rhs;
        r *= lhs;
        return r;
    }

    v2 operator*(const n2& lhs, float rhs)
    {
        v2 r = v2{lhs};
        r *= rhs;
        return r;
    }

    v2 operator*(float lhs, const n2& rhs)
    {
        v2 r{rhs};
        r *= lhs;
        return r;
    }

    v2
    operator/(const v2& lhs, float rhs)
    {
        v2 r = lhs;
        r /= rhs;
        return r;
    }


    float
    dot(const v2& lhs, const v2& rhs)
    {
        return lhs.x * rhs.x + lhs.y * rhs.y;
    }


    v2 lerp_vec2f(const v2& from, float v, const v2& to)
    {
        return
        {
            lerp_float(from.x, v, to.x),
            lerp_float(from.y, v, to.y)
        };
    }


    std::string string_from(const v2& v)
        { return fmt::format("({}, {})", v.x, v.y); }

    std::string string_from(const n2& v)
        { return fmt::format("({}, {})", v.x, v.y); }

    ADD_CATCH_FORMATTER_IMPL(v2)
    ADD_CATCH_FORMATTER_IMPL(n2)
}

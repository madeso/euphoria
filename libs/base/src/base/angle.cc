#include "base/angle.h"

#include "assert/assert.h"
#include "base/numeric.h"
#include "base/range.h"

#include <cmath>

namespace eu
{
    void
    An::wrap()
    {
        radians = ::eu::wrap(make_range(pi * 2.0f), radians);
    }


    void
    An::operator+=(const An& rhs)
    {
        radians += rhs.radians;
    }


    void
    An::operator-=(const An& rhs)
    {
        radians -= rhs.radians;
    }


    void
    An::operator*=(float rhs)
    {
        radians *= rhs;
    }


    void
    An::operator/=(float rhs)
    {
        radians /= rhs;
    }


    An
    An::operator-() const
    {
        return An::from_radians(-radians);
    }


    float
    sin(const An& ang)
    {
        return std::sin(ang.as_radians());
    }


    float
    cos(const An& ang)
    {
        return std::cos(ang.as_radians());
    }


    float
    tan(const An& ang)
    {
        return std::tan(ang.as_radians());
    }


    An
    asin(float v)
    {
        ASSERT(v <= 1 && "v must be smaller than 1");
        ASSERT(v >= -1 && "v must be greater than -1");
        return An::from_radians(std::asin(v));
    }


    An
    acos(float v)
    {
        ASSERT(v <= 1 && "v must be smaller than 1");
        ASSERT(v >= -1 && "v must be greater than -1");
        return An::from_radians(std::acos(v));
    }


    An
    atan(float v)
    {
        return An::from_radians(std::atan(v));
    }


    An
    atan2(float y, float x)
    {
        return An::from_radians(std::atan2(y, x));
    }


    An
    An::get_wrapped() const
    {
        An temp = *this;
        temp.wrap();
        return temp;
    }


    An
    operator+(const An& lhs, const An& rhs)
    {
        An temp(lhs);
        temp += rhs;
        return temp;
    }


    An
    operator-(const An& lhs, const An& rhs)
    {
        An temp(lhs);
        temp -= rhs;
        return temp;
    }


    An
    operator/(const An& lhs, float rhs)
    {
        An temp(lhs);
        temp /= rhs;
        return temp;
    }


    An operator*(const An& lhs, float rhs)
    {
        An temp(lhs);
        temp *= rhs;
        return temp;
    }

    An operator*(float rhs, const An& lhs)
    {
        return lhs * rhs;
    }


    std::string string_from(const An& a)
        { return fmt::format("{} deg", a.as_degrees()); }


    bool
    operator<(const An& lhs, const An& rhs)
    {
        return lhs.as_radians() < rhs.as_radians();
    }


    bool
    operator<=(const An& lhs, const An& rhs)
    {
        return lhs.as_radians() <= rhs.as_radians();
    }


    bool
    operator>(const An& lhs, const An& rhs)
    {
        return lhs.as_radians() > rhs.as_radians();
    }


    bool
    operator>=(const An& lhs, const An& rhs)
    {
        return lhs.as_radians() >= rhs.as_radians();
    }


    An lerp_angle(const An& from, float v, const An& to)
    {
        // https://gamedev.stackexchange.com/a/72364
        const auto dtheta = to - from;

        const auto new_from = dtheta > half_turn
            ? from + one_turn
            : dtheta < -half_turn
                ? from - one_turn
                : from
            ;
        
        return from + ( to - new_from ) * v;
    }

    ADD_CATCH_FORMATTER_IMPL(An)
}

#include "core/angle.h"

#include "assert/assert.h"
#include "core/range.h"
#include "core/random.h"

#include <cmath>

namespace euphoria::core
{
    Angle
    get_random_angle(::euphoria::core::Random* random)
    {
        return Angle::from_percent_of_360(random->get_next_float01());
    }


    void
    Angle::wrap()
    {
        radians = ::euphoria::core::wrap(make_range(pi * 2.0f), radians);
    }


    void
    Angle::operator+=(const Angle& rhs)
    {
        radians += rhs.radians;
    }


    void
    Angle::operator-=(const Angle& rhs)
    {
        radians -= rhs.radians;
    }


    void
    Angle::operator*=(float rhs)
    {
        radians *= rhs;
    }


    void
    Angle::operator/=(float rhs)
    {
        radians /= rhs;
    }


    Angle
    Angle::operator-() const
    {
        return Angle::from_radians(-radians);
    }


    float
    sin(const Angle& ang)
    {
        return std::sin(ang.as_radians());
    }


    float
    cos(const Angle& ang)
    {
        return std::cos(ang.as_radians());
    }


    float
    tan(const Angle& ang)
    {
        return std::tan(ang.as_radians());
    }


    Angle
    asin(float v)
    {
        ASSERT(v <= 1 && "v must be smaller than 1");
        ASSERT(v >= -1 && "v must be greater than -1");
        return Angle::from_radians(std::asin(v));
    }


    Angle
    acos(float v)
    {
        ASSERT(v <= 1 && "v must be smaller than 1");
        ASSERT(v >= -1 && "v must be greater than -1");
        return Angle::from_radians(std::acos(v));
    }


    Angle
    atan(float v)
    {
        return Angle::from_radians(std::atan(v));
    }


    Angle
    atan2(float y, float x)
    {
        return Angle::from_radians(std::atan2(y, x));
    }


    Angle
    Angle::get_wrapped() const
    {
        Angle temp = *this;
        temp.wrap();
        return temp;
    }


    Angle
    operator+(const Angle& lhs, const Angle& rhs)
    {
        Angle temp(lhs);
        temp += rhs;
        return temp;
    }


    Angle
    operator-(const Angle& lhs, const Angle& rhs)
    {
        Angle temp(lhs);
        temp -= rhs;
        return temp;
    }


    Angle
    operator/(const Angle& lhs, float rhs)
    {
        Angle temp(lhs);
        temp /= rhs;
        return temp;
    }


    Angle operator*(const Angle& lhs, float rhs)
    {
        Angle temp(lhs);
        temp *= rhs;
        return temp;
    }

    Angle operator*(float rhs, const Angle& lhs)
    {
        return lhs * rhs;
    }


    std::string to_string(const Angle& a)
        { return fmt::format("{} deg", a.as_degrees()); }


    bool
    operator<(const Angle& lhs, const Angle& rhs)
    {
        return lhs.as_radians() < rhs.as_radians();
    }


    bool
    operator<=(const Angle& lhs, const Angle& rhs)
    {
        return lhs.as_radians() <= rhs.as_radians();
    }


    bool
    operator>(const Angle& lhs, const Angle& rhs)
    {
        return lhs.as_radians() > rhs.as_radians();
    }


    bool
    operator>=(const Angle& lhs, const Angle& rhs)
    {
        return lhs.as_radians() >= rhs.as_radians();
    }


    Angle lerp_angle(const Angle& from, float v, const Angle& to)
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
}

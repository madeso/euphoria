#include "core/angle.h"

#include "assert/assert.h"
#include "core/range.h"
#include "core/random.h"

#include <cmath>

namespace euphoria::core
{
    angle
    angle::random(::euphoria::core::Random* random)
    {
        return from_percent_of_360(random->get_next_float01());
    }


    void
    angle::wrap()
    {
        radians = ::euphoria::core::wrap(make_range(pi * 2.0f), radians);
    }


    void
    angle::operator+=(const angle& rhs)
    {
        radians += rhs.radians;
    }


    void
    angle::operator-=(const angle& rhs)
    {
        radians -= rhs.radians;
    }


    void
    angle::operator*=(float rhs)
    {
        radians *= rhs;
    }


    void
    angle::operator/=(float rhs)
    {
        radians /= rhs;
    }


    angle
    angle::operator-() const
    {
        return angle::from_radians(-radians);
    }


    float
    sin(const angle& ang)
    {
        return std::sin(ang.in_radians());
    }


    float
    cos(const angle& ang)
    {
        return std::cos(ang.in_radians());
    }


    float
    tan(const angle& ang)
    {
        return std::tan(ang.in_radians());
    }


    angle
    asin(float v)
    {
        ASSERT(v <= 1 && "v must be smaller than 1");
        ASSERT(v >= -1 && "v must be greater than -1");
        return angle::from_radians(std::asin(v));
    }


    angle
    acos(float v)
    {
        ASSERT(v <= 1 && "v must be smaller than 1");
        ASSERT(v >= -1 && "v must be greater than -1");
        return angle::from_radians(std::acos(v));
    }


    angle
    atan(float v)
    {
        return angle::from_radians(std::atan(v));
    }


    angle
    atan2(float y, float x)
    {
        return angle::from_radians(std::atan2(y, x));
    }


    angle
    angle::get_wrapped() const
    {
        angle temp = *this;
        temp.wrap();
        return temp;
    }


    angle
    operator+(const angle& lhs, const angle& rhs)
    {
        angle temp(lhs);
        temp += rhs;
        return temp;
    }


    angle
    operator-(const angle& lhs, const angle& rhs)
    {
        angle temp(lhs);
        temp -= rhs;
        return temp;
    }


    angle
    operator/(const angle& lhs, float rhs)
    {
        angle temp(lhs);
        temp /= rhs;
        return temp;
    }


    angle operator*(const angle& lhs, float rhs)
    {
        angle temp(lhs);
        temp *= rhs;
        return temp;
    }

    angle operator*(float rhs, const angle& lhs)
    {
        return lhs * rhs;
    }


    std::ostream&
    operator<<(std::ostream& stream, const angle& a)
    {
        return stream << a.in_degrees() << " deg";
    }


    bool
    operator<(const angle& lhs, const angle& rhs)
    {
        return lhs.in_radians() < rhs.in_radians();
    }


    bool
    operator<=(const angle& lhs, const angle& rhs)
    {
        return lhs.in_radians() <= rhs.in_radians();
    }


    bool
    operator>(const angle& lhs, const angle& rhs)
    {
        return lhs.in_radians() > rhs.in_radians();
    }


    bool
    operator>=(const angle& lhs, const angle& rhs)
    {
        return lhs.in_radians() >= rhs.in_radians();
    }


    angle
    AngleTransform::transform(const angle& from, float v, const angle& to)
    {
        // https://gamedev.stackexchange.com/a/72364
        const auto dtheta = to - from;

        const auto new_from = 
            dtheta > angle::half()?
                from + angle::one_turn():
            dtheta < -angle::half()?
                from - angle::one_turn():
                from;
        
        return from + ( to - new_from ) * v;

        // return angle::from_radians(lerp(from.in_radians(), v, to.in_radians()));
    }
}

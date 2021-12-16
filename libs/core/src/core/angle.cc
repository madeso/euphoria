#include "core/angle.h"

#include "core/assert.h"
#include "core/range.h"
#include "core/random.h"

#include <cmath>

namespace euphoria::core
{
    Angle
    Angle::random(::euphoria::core::Random* random)
    {
        return from_percent_of_360(random->get_next_float01());
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
        return std::sin(ang.in_radians());
    }


    float
    cos(const Angle& ang)
    {
        return std::cos(ang.in_radians());
    }


    float
    tan(const Angle& ang)
    {
        return std::tan(ang.in_radians());
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


    std::ostream&
    operator<<(std::ostream& stream, const Angle& a)
    {
        return stream << a.in_degrees() << " deg";
    }


    bool
    operator<(const Angle& lhs, const Angle& rhs)
    {
        return lhs.in_radians() < rhs.in_radians();
    }


    bool
    operator<=(const Angle& lhs, const Angle& rhs)
    {
        return lhs.in_radians() <= rhs.in_radians();
    }


    bool
    operator>(const Angle& lhs, const Angle& rhs)
    {
        return lhs.in_radians() > rhs.in_radians();
    }


    bool
    operator>=(const Angle& lhs, const Angle& rhs)
    {
        return lhs.in_radians() >= rhs.in_radians();
    }


    Angle
    AngleTransform::transform(const Angle& from, float v, const Angle& to)
    {
        return Angle::from_radians(lerp(from.in_radians(), v, to.in_radians()));
    }
}

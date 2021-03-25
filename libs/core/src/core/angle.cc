#include "core/angle.h"

#include "core/assert.h"
#include "core/range.h"
#include "core/random.h"

#include <cmath>

namespace euphoria::core
{
    Angle
    Angle::Random(::euphoria::core::Random* random)
    {
        return FromPercentOf360(random->NextFloat01());
    }


    void
    Angle::Wrap()
    {
        radians = ::euphoria::core::Wrap(MakeRange(Pi() * 2.0f), radians);
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
        return Angle::FromRadians(-radians);
    }


    float
    Sin(const Angle& ang)
    {
        return std::sin(ang.InRadians());
    }


    float
    Cos(const Angle& ang)
    {
        return std::cos(ang.InRadians());
    }


    float
    Tan(const Angle& ang)
    {
        return std::tan(ang.InRadians());
    }


    Angle
    Asin(float v)
    {
        ASSERT(v <= 1 && "v must be smaller than 1");
        ASSERT(v >= -1 && "v must be greater than -1");
        return Angle::FromRadians(std::asin(v));
    }


    Angle
    Acos(float v)
    {
        ASSERT(v <= 1 && "v must be smaller than 1");
        ASSERT(v >= -1 && "v must be greater than -1");
        return Angle::FromRadians(std::acos(v));
    }


    Angle
    Atan(float v)
    {
        return Angle::FromRadians(std::atan(v));
    }


    Angle
    Atan2(float y, float x)
    {
        return Angle::FromRadians(std::atan2(y, x));
    }


    Angle
    Angle::GetWrapped() const
    {
        Angle temp = *this;
        temp.Wrap();
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
        return stream << a.InDegrees() << " deg";
    }


    bool
    operator<(const Angle& lhs, const Angle& rhs)
    {
        return lhs.InRadians() < rhs.InRadians();
    }


    bool
    operator<=(const Angle& lhs, const Angle& rhs)
    {
        return lhs.InRadians() <= rhs.InRadians();
    }


    bool
    operator>(const Angle& lhs, const Angle& rhs)
    {
        return lhs.InRadians() > rhs.InRadians();
    }


    bool
    operator>=(const Angle& lhs, const Angle& rhs)
    {
        return lhs.InRadians() >= rhs.InRadians();
    }


    Angle
    AngleTransform::Transform(const Angle& from, float v, const Angle& to)
    {
        return Angle::FromRadians(Lerp(from.InRadians(), v, to.InRadians()));
    }
}

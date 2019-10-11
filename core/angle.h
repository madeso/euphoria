#ifndef CORE_ANGLE_H
#define CORE_ANGLE_H

#include <iostream>

#include "core/numeric.h"

namespace euphoria::core
{
    struct Random;

    struct Angle
    {
    private:
        [[nodiscard]] static constexpr float
        RadianToDegrees(float rad)
        {
            return (180.0f / Pi()) * rad;
        }

        [[nodiscard]] static constexpr float
        DegreesToRadian(float deg)
        {
            return Pi() / 180.0f * deg;
        }

    public:
        [[nodiscard]] constexpr float
        InDegrees() const
        {
            return RadianToDegrees(mRad);
        }

        [[nodiscard]] constexpr float
        InRadians() const
        {
            return mRad;
        }

        [[nodiscard]] constexpr static Angle
        FromDegrees(float deg)
        {
            return Angle(DegreesToRadian(deg));
        }

        [[nodiscard]] constexpr static Angle
        FromRadians(float rad)
        {
            return Angle(rad);
        }

        [[nodiscard]] constexpr static Angle
        FromPercentOf360(float percent)
        {
            return Angle::FromRadians(percent * Pi() * 2.0f);
        }

        [[nodiscard]] constexpr float
        InPercentOf360() const
        {
            return InRadians() / (Pi() * 2.0f);
        }

        [[nodiscard]] constexpr static Angle
        FromPercentOf180(float percent)
        {
            return Angle::FromRadians(percent * Pi());
        }

        [[nodiscard]] const static Angle
        Random(::euphoria::core::Random* random);

        Angle
        GetWrapped() const;

        void
        Wrap();

        void
        operator+=(const Angle& rhs);

        void
        operator-=(const Angle& rhs);

        void
        operator*=(float rhs);
        void
        operator/=(float rhs);

        Angle
        operator-() const;

    private:
        constexpr explicit Angle(float rad) : mRad(rad) {}

        float mRad;
    };

    float
    Sin(const Angle& ang);

    float
    Cos(const Angle& ang);

    float
    Tan(const Angle& ang);

    Angle
    Asin(float v);
    Angle
    Acos(float v);
    Angle
    Atan(float v);

    Angle
    operator+(const Angle& lhs, const Angle& rhs);

    Angle
    operator-(const Angle& lhs, const Angle& rhs);

    Angle operator*(const Angle& lhs, float rhs);

    Angle
    operator/(const Angle& lhs, float rhs);

    Angle operator*(float rhs, const Angle& lhs);

    std::ostream&
    operator<<(std::ostream& stream, const Angle& a);

    struct AngleTransform
    {
        static Angle
        Transform(const Angle& from, float v, const Angle& to);
    };
}  // namespace euphoria::core

namespace euphoria::convert
{
    constexpr core::Angle operator"" _deg(long double d)
    {
        return core::Angle::FromDegrees(static_cast<float>(d));
    }

    constexpr core::Angle operator"" _rad(long double r)
    {
        return core::Angle::FromRadians(static_cast<float>(r));
    }
}  // namespace euphoria::convert

#endif  // CORE_ANGLE_H

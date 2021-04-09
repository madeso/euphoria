#ifndef CORE_ANGLE_H
#define CORE_ANGLE_H

#include <iostream>

#include "core/numeric.h"

namespace euphoria::core
{
    struct Random;

    struct Angle
    {
        [[nodiscard]] constexpr float
        InDegrees() const
        {
            return RadianToDegrees(radians);
        }


        [[nodiscard]] constexpr float
        InRadians() const
        {
            return radians;
        }


        [[nodiscard]] constexpr static Angle
        FromDegrees(float degrees)
        {
            return Angle(DegreesToRadian(degrees));
        }


        [[nodiscard]] constexpr static Angle
        FromRadians(float radians)
        {
            return Angle(radians);
        }


        [[nodiscard]] constexpr static Angle
        FromPercentOf360(float percent)
        {
            return Angle::FromRadians(percent * Pi() * 2.0f);
        }


        [[nodiscard]] constexpr static Angle
        OneTurn()
        {
            return Angle::FromRadians(Pi() * 2.0f);
        }


        [[nodiscard]] constexpr static Angle
        Quarter()
        {
            return Angle::FromRadians(Pi() / 2.0f);
        }


        [[nodiscard]] constexpr static Angle
        Zero()
        {
            return Angle::FromRadians(0.0f);
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


        [[nodiscard]] static Angle
        Random(::euphoria::core::Random* random);


        [[nodiscard]] Angle
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
        [[nodiscard]] static constexpr float
        RadianToDegrees(float radians)
        {
            return (180.0f / Pi()) * radians;
        }

        [[nodiscard]] static constexpr float
        DegreesToRadian(float degrees)
        {
            return Pi() / 180.0f * degrees;
        }

        constexpr explicit Angle(float r) : radians(r) {}

        float radians;
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
    Atan2(float y, float x);


    Angle
    operator+(const Angle& lhs, const Angle& rhs);


    Angle
    operator-(const Angle& lhs, const Angle& rhs);


    Angle
    operator*(const Angle& lhs, float rhs);


    Angle
    operator/(const Angle& lhs, float rhs);


    Angle
    operator*(float rhs, const Angle& lhs);


    std::ostream&
    operator<<(std::ostream& stream, const Angle& a);


    bool
    operator<(const Angle& lhs, const Angle& rhs);


    bool
    operator<=(const Angle& lhs, const Angle& rhs);


    bool
    operator>(const Angle& lhs, const Angle& rhs);


    bool
    operator>=(const Angle& lhs, const Angle& rhs);


    struct AngleTransform
    {
        static Angle
        Transform(const Angle& from, float v, const Angle& to);
    };
}


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
}

#endif  // CORE_ANGLE_H

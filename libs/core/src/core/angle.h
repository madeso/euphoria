#pragma once

#include <iostream>

#include "core/numeric.h"

namespace euphoria::core
{
    struct Random;

    struct Angle
    {
        [[nodiscard]] constexpr float
        in_degrees() const
        {
            return radian_to_degrees(radians);
        }


        [[nodiscard]] constexpr float
        in_radians() const
        {
            return radians;
        }


        [[nodiscard]] constexpr static Angle
        from_degrees(float degrees)
        {
            return Angle(degrees_to_radian(degrees));
        }


        [[nodiscard]] constexpr static Angle
        from_radians(float radians)
        {
            return Angle(radians);
        }


        [[nodiscard]] constexpr static Angle
        from_percent_of_360(float percent)
        {
            return Angle::from_radians(percent * pi * 2.0f);
        }


        [[nodiscard]] constexpr static Angle
        one_turn()
        {
            return Angle::from_radians(pi * 2.0f);
        }


        [[nodiscard]] constexpr static Angle
        quarter()
        {
            return Angle::from_radians(pi / 2.0f);
        }


        [[nodiscard]] constexpr static Angle
        zero()
        {
            return Angle::from_radians(0.0f);
        }


        [[nodiscard]] constexpr float
        in_percent_of_360() const
        {
            return in_radians() / (pi * 2.0f);
        }


        [[nodiscard]] constexpr static Angle
        from_percent_of_180(float percent)
        {
            return Angle::from_radians(percent * pi);
        }


        [[nodiscard]] static Angle
        random(::euphoria::core::Random* random);


        [[nodiscard]] Angle
        get_wrapped() const;


        void
        wrap();


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
        radian_to_degrees(float radians)
        {
            return (180.0f / pi) * radians;
        }

        [[nodiscard]] static constexpr float
        degrees_to_radian(float degrees)
        {
            return pi / 180.0f * degrees;
        }

        constexpr explicit Angle(float r) : radians(r) {}

        float radians;
    };


    float
    sin(const Angle& ang);


    float
    cos(const Angle& ang);


    float
    tan(const Angle& ang);


    Angle
    asin(float v);


    Angle
    acos(float v);


    Angle
    atan(float v);


    Angle
    atan2(float y, float x);


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


    struct angle_transform
    {
        static Angle
        transform(const Angle& from, float v, const Angle& to);
    };
}


namespace euphoria::convert
{
    constexpr core::Angle operator"" _deg(long double d)
    {
        return core::Angle::from_degrees(static_cast<float>(d));
    }


    constexpr core::Angle operator"" _rad(long double r)
    {
        return core::Angle::from_radians(static_cast<float>(r));
    }
}

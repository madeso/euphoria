#pragma once

#include <iostream>

#include "core/numeric.h"

namespace euphoria::core
{
    struct Random;

    struct angle
    {
        [[nodiscard]] constexpr static angle
        from_degrees(float degrees)
        {
            return angle(degrees_to_radian(degrees));
        }


        [[nodiscard]] constexpr static angle
        from_radians(float radians)
        {
            return angle(radians);
        }

        [[nodiscard]] constexpr static angle
        from_percent_of_360(float percent)
        {
            return angle::from_radians(percent * pi * 2.0f);
        }

        [[nodiscard]] constexpr static angle
        from_percent_of_180(float percent)
        {
            return angle::from_radians(percent * pi);
        }

        [[nodiscard]] constexpr static angle
        one_turn()
        {
            return angle::from_radians(pi * 2.0f);
        }


        [[nodiscard]] constexpr static angle
        half()
        {
            return angle::from_radians(pi);
        }


        [[nodiscard]] constexpr static angle
        quarter()
        {
            return angle::from_radians(pi / 2.0f);
        }


        [[nodiscard]] constexpr static angle
        zero()
        {
            return angle::from_radians(0.0f);
        }

        [[nodiscard]] static angle random(::euphoria::core::Random* random);

        void wrap();


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

        [[nodiscard]] constexpr float
        in_percent_of_360() const
        {
            return in_radians() / (pi * 2.0f);
        }

        void operator+=(const angle& rhs);
        void operator-=(const angle& rhs);
        void operator*=(float rhs);
        void operator/=(float rhs);
        angle operator-() const;

        [[nodiscard]] angle get_wrapped() const;


    private:
        float radians;

        constexpr explicit angle(float r) : radians(r) {}

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
    };


    float
    sin(const angle& ang);


    float
    cos(const angle& ang);


    float
    tan(const angle& ang);


    angle
    asin(float v);


    angle
    acos(float v);


    angle
    atan(float v);


    angle
    atan2(float y, float x);


    angle
    operator+(const angle& lhs, const angle& rhs);


    angle
    operator-(const angle& lhs, const angle& rhs);


    angle
    operator*(const angle& lhs, float rhs);


    angle
    operator/(const angle& lhs, float rhs);


    angle
    operator*(float rhs, const angle& lhs);


    std::ostream&
    operator<<(std::ostream& stream, const angle& a);


    bool
    operator<(const angle& lhs, const angle& rhs);


    bool
    operator<=(const angle& lhs, const angle& rhs);


    bool
    operator>(const angle& lhs, const angle& rhs);


    bool
    operator>=(const angle& lhs, const angle& rhs);


    struct AngleTransform
    {
        static angle
        transform(const angle& from, float v, const angle& to);
    };
}


namespace euphoria::convert
{
    constexpr core::angle operator"" _deg(long double d)
    {
        return core::angle::from_degrees(static_cast<float>(d));
    }


    constexpr core::angle operator"" _rad(long double r)
    {
        return core::angle::from_radians(static_cast<float>(r));
    }
}

#pragma once

#include <iostream>

#include "core/numeric.h"
#include "core/interpolate.default.h"


namespace euphoria::core
{
    struct Random;

    struct angle
    {
        [[nodiscard]] constexpr static angle
        from_degrees(float degrees)
        {
            return angle(c_degrees_to_radian(degrees));
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


        void wrap();


        [[nodiscard]] constexpr float
        as_degrees() const
        {
            return c_radian_to_degrees(radians);
        }

        [[nodiscard]] constexpr float
        as_radians() const
        {
            return radians;
        }

        [[nodiscard]] constexpr float
        from_percent_of_360() const
        {
            return as_radians() / (pi * 2.0f);
        }

        [[nodiscard]] angle get_wrapped() const;

        void operator+=(const angle& rhs);
        void operator-=(const angle& rhs);
        void operator*=(float rhs);
        void operator/=(float rhs);
        angle operator-() const;

    private:
        float radians;

        constexpr explicit angle(float r) : radians(r) {}

        [[nodiscard]] static constexpr float
        c_radian_to_degrees(float radians)
        {
            return (180.0f / pi) * radians;
        }

        [[nodiscard]] static constexpr float
        c_degrees_to_radian(float degrees)
        {
            return pi / 180.0f * degrees;
        }
    };

    constexpr angle one_turn = angle::from_radians(pi * 2.0f);
    constexpr angle half_turn = angle::from_radians(pi);
    constexpr angle quarter_turn = angle::from_radians(pi / 2.0f);
    constexpr angle no_rotation = angle::from_radians(0.0f);

    [[nodiscard]] angle get_random_angle(::euphoria::core::Random* random);


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


    std::string to_string(const angle& a);


    bool
    operator<(const angle& lhs, const angle& rhs);


    bool
    operator<=(const angle& lhs, const angle& rhs);


    bool
    operator>(const angle& lhs, const angle& rhs);


    bool
    operator>=(const angle& lhs, const angle& rhs);


    angle lerp_angle(const angle& from, float v, const angle& to);

    DEFAULT_INTERPOLATE(angle, lerp_angle);
}

ADD_DEFAULT_FORMATTER(euphoria::core::angle, std::string, euphoria::core::to_string);

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

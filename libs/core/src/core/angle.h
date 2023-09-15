#pragma once

#include <iostream>

#include "core/numeric.h"
#include "core/interpolate.default.h"


namespace eu::core
{
    struct Random;

    struct Angle
    {
        [[nodiscard]] constexpr static Angle
        from_degrees(float degrees)
        {
            return Angle(c_degrees_to_radian(degrees));
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
        from_percent_of_180(float percent)
        {
            return Angle::from_radians(percent * pi);
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

        [[nodiscard]] Angle get_wrapped() const;

        void operator+=(const Angle& rhs);
        void operator-=(const Angle& rhs);
        void operator*=(float rhs);
        void operator/=(float rhs);
        Angle operator-() const;

    private:
        float radians;

        constexpr explicit Angle(float r) : radians(r) {}

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

    constexpr Angle one_turn = Angle::from_radians(pi * 2.0f);
    constexpr Angle half_turn = Angle::from_radians(pi);
    constexpr Angle quarter_turn = Angle::from_radians(pi / 2.0f);
    constexpr Angle no_rotation = Angle::from_radians(0.0f);

    [[nodiscard]] Angle get_random_angle(::eu::core::Random* random);


    float sin(const Angle& ang);
    float cos(const Angle& ang);
    float tan(const Angle& ang);
    Angle asin(float v);
    Angle acos(float v);
    Angle atan(float v);
    Angle atan2(float y, float x);

    Angle operator+(const Angle& lhs, const Angle& rhs);
    Angle operator-(const Angle& lhs, const Angle& rhs);
    Angle operator*(const Angle& lhs, float rhs);
    Angle operator/(const Angle& lhs, float rhs);
    Angle operator*(float rhs, const Angle& lhs);

    std::string to_string(const Angle& a);

    bool operator<(const Angle& lhs, const Angle& rhs);
    bool operator<=(const Angle& lhs, const Angle& rhs);
    bool operator>(const Angle& lhs, const Angle& rhs);
    bool operator>=(const Angle& lhs, const Angle& rhs);

    Angle lerp_angle(const Angle& from, float v, const Angle& to);

    DEFAULT_INTERPOLATE(Angle, lerp_angle);
}

ADD_DEFAULT_FORMATTER(eu::core::Angle, std::string, eu::core::to_string);

namespace eu::convert
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

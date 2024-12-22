#pragma once

#include "base/numeric.h"

namespace eu
{
    /// An angle in both degrees and radians.
    struct An
    {
        [[nodiscard]] constexpr static An
        from_degrees(float degrees)
        {
            return An(rad_from_deg(degrees));
        }

        [[nodiscard]] constexpr static An
        from_radians(float radians)
        {
            return An(radians);
        }

        [[nodiscard]] constexpr static An
        from_percent_of_360(float percent)
        {
            return An::from_radians(percent * pi * 2.0f);
        }

        [[nodiscard]] constexpr static An
        from_percent_of_180(float percent)
        {
            return An::from_radians(percent * pi);
        }


        void wrap();


        [[nodiscard]] constexpr float
        as_degrees() const
        {
            return deg_from_rad(radians);
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

        [[nodiscard]] An get_wrapped() const;

        void operator+=(const An& rhs);
        void operator-=(const An& rhs);
        void operator*=(float rhs);
        void operator/=(float rhs);
        An operator-() const;

    private:
        float radians;

        constexpr explicit An(float r) : radians(r) {}

        [[nodiscard]] static constexpr float
        deg_from_rad(float radians)
        {
            return (180.0f / pi) * radians;
        }

        [[nodiscard]] static constexpr float
        rad_from_deg(float degrees)
        {
            return pi / 180.0f * degrees;
        }
    };

    constexpr An one_turn = An::from_radians(pi * 2.0f);
    constexpr An half_turn = An::from_radians(pi);
    constexpr An quarter_turn = An::from_radians(pi / 2.0f);
    constexpr An no_rotation = An::from_radians(0.0f);


    float sin(const An& ang);
    float cos(const An& ang);
    float tan(const An& ang);
    An asin(float v);
    An acos(float v);
    An atan(float v);
    An atan2(float y, float x);

    An operator+(const An& lhs, const An& rhs);
    An operator-(const An& lhs, const An& rhs);
    An operator*(const An& lhs, float rhs);
    An operator/(const An& lhs, float rhs);
    An operator*(float rhs, const An& lhs);

    std::string string_from(const An& a);

    bool operator<(const An& lhs, const An& rhs);
    bool operator<=(const An& lhs, const An& rhs);
    bool operator>(const An& lhs, const An& rhs);
    bool operator>=(const An& lhs, const An& rhs);

    An lerp_angle(const An& from, float v, const An& to);
}

ADD_DEFAULT_FORMATTER(eu::An, std::string, eu::string_from);

namespace eu::convert
{
    constexpr An operator""_deg(long double d)
    {
        return An::from_degrees(static_cast<float>(d));
    }


    constexpr An operator""_rad(long double r)
    {
        return An::from_radians(static_cast<float>(r));
    }
}

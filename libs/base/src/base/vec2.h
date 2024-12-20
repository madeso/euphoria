#pragma once

#include "base/angle.h"


namespace eu
{
    struct Random;

    ////////////////////////////////////////////////////////////////////////////////
    /// Forward declarations

    /// a 2d vector
    struct v2;


    /// a 2d unit (vector)
    struct n2;


    ////////////////////////////////////////////////////////////////////////////////


    struct v2
    {
        float x;
        float y;

        v2() = default;
        constexpr v2(float ax, float ay)
            : x(ax)
            , y(ay)
        {
        }
        explicit v2(const std::tuple<float, float>& a);
        explicit v2(const n2& u);

        [[nodiscard]] static v2 from_to(const v2& from, const v2& to);

        float* get_data_ptr();
        float normalize();

        [[nodiscard]] v2 get_rotated(const Angle& a) const;

        [[nodiscard]] const float* get_data_ptr() const;
        [[nodiscard]] v2 get_flipped_y() const;
        [[nodiscard]] float get_length_squared() const;
        [[nodiscard]] float get_length() const;
        [[nodiscard]] n2 get_normalized() const;

        void operator+=(const v2& rhs);
        void operator-=(const v2& rhs);
        v2 operator-() const;
        void operator/=(float rhs);
        void operator*=(float rhs);
    };

    constexpr v2 zero2f = v2{ 0, 0 };

    ////////////////////////////////////////////////////////////////////////////////

    
    struct n2
    {
        float x;
        float y;

        explicit n2(float ax, float ay);

        float* get_data_ptr();

        [[nodiscard]] n2 get_rotated(const Angle& a) const;

        [[nodiscard]] const float* get_data_ptr() const;
        [[nodiscard]] v2 to_vec() const;
        [[nodiscard]] n2 get_flipped_y() const;

        // todo(Gustav): remove this...
        [[nodiscard]] float get_length_squared() const;
        [[nodiscard]] bool is_valid() const;

        n2 operator-() const;

    private:
        friend struct v2;

        explicit n2(const v2& v);
    };


    ////////////////////////////////////////////////////////////////////////////////
    /// Math operators
    
    v2 operator+(const v2& lhs, const v2& rhs);
    v2 operator-(const v2& lhs, const v2& rhs);
    v2 operator*(const v2& lhs, float rhs);
    v2 operator*(float lhs, const v2& rhs);
    v2 operator*(const n2& lhs, float rhs);
    v2 operator*(float lhs, const n2& rhs);
    v2 operator/(const v2& lhs, float rhs);


    ////////////////////////////////////////////////////////////////////////////////
    /// Functions

    float dot(const v2& lhs, const v2& rhs);

    ////////////////////////////////////////////////////////////////////////////////
    /// Transform

    v2 lerp_vec2f(const v2& from, float v, const v2& to);

    // todo(Gustav): add one lerp and one slerp unit transform?


    ////////////////////////////////////////////////////////////////////////////////
    /// Printing

    std::string to_string(const v2& v);
    std::string to_string(const n2& v);
    
    // util functions
    n2 create_random_unit(Random* random);
}

ADD_DEFAULT_FORMATTER(eu::v2, std::string, eu::to_string);
ADD_DEFAULT_FORMATTER(eu::n2, std::string, eu::to_string);

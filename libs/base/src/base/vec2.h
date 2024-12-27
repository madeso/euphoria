#pragma once

#include "base/angle.h"


namespace eu
{
    /** \addtogroup math
     *  @{
     */
    
    struct v2;
    struct n2;

    /// a 2d vector
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
        [[nodiscard]] static v2 from(const n2& n);

        /** Changes the length to 1.
        * If the calculated length is zero, the vector is changed to a known value and false is returned
        */
        bool normalize();

        /// Assumes the vector is a point and returns it if it was rotated around `(0,0)`
        [[nodiscard]] v2 get_rotated(const An& a) const;

        /// Returns an array to the data.
        /// non const so it's useful for letting an API tweak the members.
        float* get_data_ptr();

        /// Returns an array to the data
        [[nodiscard]] const float* get_data_ptr() const;

        /// Returns a new vector where the sign of the y component is flipped.
        [[nodiscard]] v2 get_flipped_y() const;

        /// Returns the squared length of the vector.
        /// This is useful if you want to compare to zero, sort by length or similar, otherwise see @ref get_length()
        [[nodiscard]] float get_length_squared() const;

        /// Returns the length of the vector
        [[nodiscard]] float get_length() const;

        /** Changes the length to 1.
        * If the calculated length is zero, the vector is changed to a known value and false is returned
        */
        [[nodiscard]] std::optional<n2> get_normalized() const;

        void operator+=(const v2& rhs);
        void operator-=(const v2& rhs);
        v2 operator-() const;
        void operator/=(float rhs);
        void operator*=(float rhs);
    };

    constexpr v2 zero2f = v2{ 0, 0 };

    /// a 2d unit (vector)
    struct n2
    {
        float x;
        float y;

        /// asserts that the length is 1
        explicit n2(float ax, float ay);

        /// asserts that the length is 1
        explicit n2(const v2& v);

        /// Assumes the unit vector is a point and returns it if it was rotated around `(0,0)`
        [[nodiscard]] n2 get_rotated(const An& a) const;

        /// Returns an array to the data
        [[nodiscard]] const float* get_data_ptr() const;

        /// Returns a new unit vector where the sign of the y component is flipped.
        [[nodiscard]] n2 get_flipped_y() const;

        /// returns false if the length isn't 1
        [[nodiscard]] bool is_valid() const;

        n2 operator-() const;
    };


    v2 operator+(const v2& lhs, const v2& rhs);
    v2 operator-(const v2& lhs, const v2& rhs);
    v2 operator*(const v2& lhs, float rhs);
    v2 operator*(float lhs, const v2& rhs);
    v2 operator*(const n2& lhs, float rhs);
    v2 operator*(float lhs, const n2& rhs);
    v2 operator/(const v2& lhs, float rhs);


    float dot(const v2& lhs, const v2& rhs);

    v2 lerp_vec2f(const v2& from, float v, const v2& to);

    /// converts a 2d vector to a string, prefer fmt
    std::string string_from(const v2& v);

    /// converts a 2d unit vector to a string, prefer fmt
    std::string string_from(const n2& v);

    /** @}*/
}

ADD_DEFAULT_FORMATTER(eu::v2, std::string, eu::string_from);
ADD_DEFAULT_FORMATTER(eu::n2, std::string, eu::string_from);

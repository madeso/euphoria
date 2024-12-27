#pragma once

#include <tuple>

#include "assert/assert.h"

#include "base/numeric.h"

namespace eu
{
    /** \addtogroup math
     *  @{
     */

    ////////////////////////////////////////////////////////////////////////////////
    /// Forward declarations

    struct v3;
    struct n3;

    ////////////////////////////////////////////////////////////////////////////////

    /// a 3d vector
    struct v3
    {
        float x;
        float y;
        float z;

        explicit v3(float a);
        explicit v3(const std::tuple<float, float, float> &a);
        constexpr v3(float ax, float ay, float az)
            : x(ax), y(ay), z(az)
        {
        }

        /// assumes the given pointer is a array of 3 floats
        explicit v3(const float *a);

        /// creates a vector going from `from` to `to`
        static v3 from_to(const v3 &from, const v3 &to);

        [[nodiscard]] float dot(const v3 &rhs) const;
        [[nodiscard]] v3 cross(const v3 &u) const;

        void operator+=(const v3 &rhs);
        void operator-=(const v3 &rhs);
        void operator/=(float rhs);
        void operator*=(float rhs);
        v3 operator-() const;

        /// Returns an array to the data.
        /// non const so it's useful for letting an API tweak the members.
        float *get_data_ptr();

        /// Returns an array to the data
        [[nodiscard]] const float *get_data_ptr() const;

        /// Returns the squared length of the vector.
        /// This is useful if you want to compare to zero, sort by length or similar, otherwise see @ref get_length()
        [[nodiscard]] constexpr float get_length_squared() const
        {
            return x * x + y * y + z * z;
        }

        /// Returns the length of the vector
        [[nodiscard]] float get_length() const;

        /** Changes the length to 1.
        * If the calculated length is zero, the vector is changed to a known value and false is returned
        */
        bool normalize();

        /// Returns a unit vector.
        /// if the length is zero, `nullopt` is returend
        [[nodiscard]] std::optional<n3> get_normalized() const;

        bool operator==(const v3 &rhs) = delete;
    };

    constexpr v3 zero3f = v3{0.0f, 0.0f, 0.0f};

    ////////////////////////////////////////////////////////////////////////////////

    /// a 3d unit (vector)
    struct n3 : public v3
    {
        constexpr n3 operator-() const
        {
            return {-this->x, -this->y, -this->z};
        }

        /// returns false if the length isn't 1
        [[nodiscard]] constexpr bool
        is_valid() const
        {
            return is_equal(get_length_squared(), 1.0f);
        }

        bool operator==(const n3 &rhs) = delete;

        /// asserts that the length is 1
        constexpr n3(float a, float b, float c)
            : v3(a, b, c)
        {
            ASSERT(is_valid());
        }

        /// asserts that the length is 1
        constexpr explicit n3(const v3& v)
            : v3(v)
        {
            ASSERT(is_valid());
        }
    };

    namespace common
    {
        constexpr n3 x_axis = n3{1.0f, 0.0f, 0.0f};
        constexpr n3 y_axis = n3{0.0f, 1.0f, 0.0f};
        constexpr n3 z_axis = n3{0.0f, 0.0f, 1.0f};
        constexpr n3 up = y_axis;
        constexpr n3 down = -y_axis;
        constexpr n3 right = x_axis;
        constexpr n3 left = -x_axis;
        constexpr n3 in = -z_axis;
        constexpr n3 out = z_axis;
    }

    v3 operator+(const v3 &lhs, const v3 &rhs);
    v3 operator-(const v3 &lhs, const v3 &rhs);
    v3 operator*(float lhs, const v3 &rhs);
    v3 operator*(const v3 &lhs, float rhs);
    v3 operator/(const v3 &lhs, float rhs);
    v3 operator/(float lhs, const v3 &rhs);

    
    v3 lerp_v3(const v3 &f, float v, const v3 &t);

    /// convert a 3d vector to string, prefer fmt
    std::string string_from(const v3 &v);

    /// converts a 3d unit vector to string, prefer fmt
    std::string string_from(const n3 &v);

    /// component wise min value
    constexpr v3
    min(const v3 &lhs, const v3 &rhs)
    {
        return {
            eu::min(lhs.x, rhs.x),
            eu::min(lhs.y, rhs.y),
            eu::min(lhs.z, rhs.z)
        };
    }

    /// component wise max value
    constexpr v3
    max(const v3 &lhs, const v3 &rhs)
    {
        return {
            eu::max(lhs.x, rhs.x),
            eu::max(lhs.y, rhs.y),
            eu::max(lhs.z, rhs.z)
        };
    }

    /** @}*/

}

ADD_DEFAULT_FORMATTER(eu::v3, std::string, eu::string_from);
ADD_DEFAULT_FORMATTER(eu::n3, std::string, eu::string_from);


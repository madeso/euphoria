#pragma once

#include <tuple>

#include "assert/assert.h"

#include "base/numeric.h"

namespace eu
{
    ////////////////////////////////////////////////////////////////////////////////
    /// Forward declarations

    struct v3;
    struct n3;

    ////////////////////////////////////////////////////////////////////////////////

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

        explicit v3(const float *a);

        static v3 from_to(const v3 &from, const v3 &to);

        [[nodiscard]] float dot(const v3 &rhs) const;
        [[nodiscard]] v3 cross(const v3 &u) const;

        void operator+=(const v3 &rhs);
        void operator-=(const v3 &rhs);
        void operator/=(float rhs);
        void operator*=(float rhs);
        v3 operator-() const;

        float *get_data_ptr();
        [[nodiscard]] const float *get_data_ptr() const;

        [[nodiscard]] constexpr float get_length_squared() const
        {
            return x * x + y * y + z * z;
        }

        [[nodiscard]] float get_length() const;

        void normalize();
        [[nodiscard]] n3 get_normalized() const;

        // todo(Gustav): rename to assume_normalized
        [[nodiscard]] n3 as_normalized() const;

        bool operator==(const v3 &rhs) = delete;
    };

    constexpr v3 zero3f = v3{0.0f, 0.0f, 0.0f};

    ////////////////////////////////////////////////////////////////////////////////

    struct n3 : public v3
    {
        constexpr n3 operator-() const
        {
            return {-this->x, -this->y, -this->z};
        }

        [[nodiscard]] constexpr bool
        is_valid() const
        {
            return is_equal(get_length_squared(), 1.0f);
        }

        static n3 to_unit(float x, float y, float z);
        static n3 to_unit(const v3 &v);

        bool operator==(const n3 &rhs) = delete;

        constexpr n3(float a, float b, float c)
            : v3(a, b, c)
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

    ////////////////////////////////////////////////////////////////////////////////
    /// Math operators

    v3 operator+(const v3 &lhs, const v3 &rhs);
    v3 operator-(const v3 &lhs, const v3 &rhs);

    v3 operator*(float lhs, const v3 &rhs);
    v3 operator*(const v3 &lhs, float rhs);

    v3 operator/(const v3 &lhs, float rhs);
    v3 operator/(float lhs, const v3 &rhs);

    ////////////////////////////////////////////////////////////////////////////////
    /// Transformations

    v3 lerp_v3(const v3 &f, float v, const v3 &t);

    ////////////////////////////////////////////////////////////////////////////////
    /// Printing

    std::string to_string(const v3 &v);
    std::string to_string(const n3 &v);

    //////////////////////////
    // minmax
    constexpr v3
    min(const v3 &lhs, const v3 &rhs)
    {
#define M(var) eu::min(lhs.var, rhs.var)
        return v3{M(x), M(y), M(z)};
#undef M
    }

    constexpr v3
    max(const v3 &lhs, const v3 &rhs)
    {
#define M(var) eu::max(lhs.var, rhs.var)
        return v3{M(x), M(y), M(z)};
#undef M
    }
}

ADD_DEFAULT_FORMATTER(eu::v3, std::string, eu::to_string);
ADD_DEFAULT_FORMATTER(eu::n3, std::string, eu::to_string);

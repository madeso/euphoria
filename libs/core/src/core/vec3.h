#pragma once



#include <tuple>

#include "core/vec2.h"
#include "core/numeric.h"
#include "assert/assert.h"
#include "core/cint.h"


namespace euphoria::core
{
    ////////////////////////////////////////////////////////////////////////////////
    /// Forward declarations

    struct vec3f;
    struct unit3f;
    struct Scale3f;


    ////////////////////////////////////////////////////////////////////////////////


    struct Scale3f
    {
        float x;
        float y;
        float z;

        explicit Scale3f(float a);
        explicit Scale3f(const std::tuple<float, float, float>& a);
        Scale3f(float ax, float ay, float az);
        explicit Scale3f(const float* a);

        float* get_data_ptr();
        [[nodiscard]] const float* get_data_ptr() const;

        bool operator==(const Scale3f& rhs) = delete;
    };


    ////////////////////////////////////////////////////////////////////////////////


    struct vec3f
    {
        float x;
        float y;
        float z;

        explicit vec3f(float a);
        explicit vec3f(const std::tuple<float, float, float>& a);
        constexpr vec3f(float ax, float ay, float az)
            : x(ax)
            , y(ay)
            , z(az)
        {
        }

        explicit vec3f(const float* a);
        vec3f(const vec2f& a, float az);

        static vec3f from_to(const vec3f& from, const vec3f& to);
        
        [[nodiscard]] float dot(const vec3f& rhs) const;
        [[nodiscard]] vec3f cross(const vec3f& u) const;

        void operator+=(const vec3f& rhs);
        void operator-=(const vec3f& rhs);
        void operator/=(float rhs);
        void operator*=(float rhs);
        vec3f operator-() const;
        
        float* get_data_ptr();
        [[nodiscard]] const float* get_data_ptr() const;

        [[nodiscard]] constexpr float get_length_squared() const
        {
            return x * x + y * y + z * z;
        }

        [[nodiscard]] float get_length() const;

        void normalize();
        [[nodiscard]] unit3f get_normalized() const;

        // todo(Gustav): rename to assume_normalized
        [[nodiscard]] unit3f as_normalized() const;

        bool operator==(const vec3f& rhs) = delete;
    };

    constexpr vec3f zero3f = vec3f{ 0.0f, 0.0f, 0.0f };


    ////////////////////////////////////////////////////////////////////////////////


    struct unit3f : public vec3f
    {
        constexpr unit3f operator-() const
        {
            return unit3f(-this->x, -this->y, -this->z);
        }

        [[nodiscard]] constexpr bool
        is_valid() const
        {
            return is_equal(get_length_squared(), 1.0f);
        }

        static unit3f to_unit(float x, float y, float z);
        static unit3f to_unit(const vec3f& v);

        bool operator==(const unit3f& rhs) = delete;

        constexpr unit3f(float a, float b, float c)
            : vec3f(a, b, c)
        {
            ASSERT(is_valid());
        }
    };

    namespace common
    {
        constexpr unit3f x_axis = unit3f {1.0f, 0.0f, 0.0f};
        constexpr unit3f y_axis = unit3f {0.0f, 1.0f, 0.0f};
        constexpr unit3f z_axis = unit3f {0.0f, 0.0f, 1.0f};
        constexpr unit3f up = y_axis;
        constexpr unit3f down = -y_axis;
        constexpr unit3f right = x_axis;
        constexpr unit3f left = -x_axis;
        constexpr unit3f in = -z_axis;
        constexpr unit3f out = z_axis;
    }

    ////////////////////////////////////////////////////////////////////////////////
    /// Math operators

    vec3f operator+(const vec3f& lhs, const vec3f& rhs);
    vec3f operator-(const vec3f& lhs, const vec3f& rhs);

    vec3f operator*(float lhs, const vec3f& rhs);
    vec3f operator*(const vec3f& lhs, float rhs);

    vec3f operator/(const vec3f& lhs, float rhs);
    vec3f operator/(float lhs, const vec3f& rhs);


    ////////////////////////////////////////////////////////////////////////////////
    /// Transformations

    vec3f lerp_vec3f(const vec3f& f, float v, const vec3f& t);


    ////////////////////////////////////////////////////////////////////////////////
    /// Printing

    std::ostream& operator<<(std::ostream& stream, const vec3f& v);
    std::ostream& operator<<(std::ostream& stream, const unit3f& v);
    std::ostream& operator<<(std::ostream& stream, const Scale3f& v);
}

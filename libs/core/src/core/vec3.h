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

        explicit Scale3f(const float& a);
        explicit Scale3f(const std::tuple<float, float, float>& a);
        Scale3f(const float& ax, const float& ay, const float& az);
        explicit Scale3f(const float* a);

        float* get_data_ptr();
        [[nodiscard]] const float* get_data_ptr() const;

        [[nodiscard]] float get_component_sum() const;

        bool operator==(const Scale3f& rhs) = delete;
    };


    ////////////////////////////////////////////////////////////////////////////////


    struct vec3f
    {
        float x;
        float y;
        float z;

        explicit vec3f(const float& a);
        explicit vec3f(const std::tuple<float, float, float>& a);
        vec3f(const float& ax, const float& ay, const float& az);
        explicit vec3f(const float* a);
        vec3f(const vec2<float>& a, const float& az);

        static vec3f from_to(const vec3f& from, const vec3f& to);
        static vec3f zero();

        void operator+=(const vec3f& rhs);
        void operator-=(const vec3f& rhs);
        void operator/=(const float& rhs);
        void operator*=(const float& rhs);
        vec3f operator-() const;

        [[nodiscard]] float get_component_sum() const;
        
        float* get_data_ptr();
        [[nodiscard]] const float* get_data_ptr() const;

        float get_length_squared() const;
        [[nodiscard]] float get_length() const;

        void normalize();
        [[nodiscard]] unit3f get_normalized() const;

        // todo(Gustav): rename to assume_normalized
        [[nodiscard]] unit3f as_normalized() const;

        bool operator==(const vec3f& rhs) = delete;
    };


    ////////////////////////////////////////////////////////////////////////////////


    struct unit3f : public vec3f
    {
        unit3f operator-() const;

        [[nodiscard]] bool
        is_valid() const;

        static unit3f x_axis();
        static unit3f y_axis();
        static unit3f z_axis();
        
        static unit3f up();     static unit3f down();
        static unit3f right();  static unit3f left();
        static unit3f in();     static unit3f out();

        static unit3f to_unit(float x, float y, float z);
        static unit3f to_unit(const vec3f& v);

        bool operator==(const unit3f& rhs) = delete;

    private:
        explicit unit3f(float a, float b, float c);
    };


    ////////////////////////////////////////////////////////////////////////////////
    /// Math operators

    vec3f operator+(const vec3f& lhs, const vec3f& rhs);
    vec3f operator-(const vec3f& lhs, const vec3f& rhs);

    vec3f operator*(float lhs, const vec3f& rhs);
    vec3f operator*(const vec3f& lhs, float rhs);

    vec3f operator/(const vec3f& lhs, float rhs);
    vec3f operator/(float lhs, const vec3f& rhs);

    vec3f component_multiply(const vec3f& lhs, const vec3f& rhs);


    ////////////////////////////////////////////////////////////////////////////////
    /// Functions

    float dot(const vec3f& lhs, const vec3f& rhs);
    vec3f cross(const vec3f& v, const vec3f u);


    ////////////////////////////////////////////////////////////////////////////////
    /// Transformations

    // todo(Gustav): implement transformations


    ////////////////////////////////////////////////////////////////////////////////
    /// Printing

    std::ostream& operator<<(std::ostream& stream, const vec3f& v);
    std::ostream& operator<<(std::ostream& stream, const unit3f& v);
    std::ostream& operator<<(std::ostream& stream, const Scale3f& v);
}

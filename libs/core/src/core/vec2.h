#pragma once

#include <tuple>

#include "assert/assert.h"

#include "core/interpolate.default.h"
#include "core/angle.h"
#include "core/numeric.h"
#include "core/cint.h"
#include "core/vec.base.h"


namespace euphoria::core
{
    struct Random;

    ////////////////////////////////////////////////////////////////////////////////
    /// Forward declarations

    // a 2d vector
    struct vec2f; struct vec2i;


    // a 2d unit (vector)
    struct unit2f; struct unit2i;


    ////////////////////////////////////////////////////////////////////////////////


    struct vec2f
    {
        float x;
        float y;

        vec2f() = default;
        vec2f(float ax, float ay);
        explicit vec2f(const std::tuple<float, float>& a);
        explicit vec2f(const unit2f& u);

        [[nodiscard]] static vec2f zero();
        [[nodiscard]] static vec2f from_to(const vec2f& from, const vec2f& to);

        float* get_data_ptr();
        float normalize();

        [[nodiscard]] vec2f get_rotated(const angle& a) const;

        [[nodiscard]] const float* get_data_ptr() const;
        [[nodiscard]] vec2i toi() const;
        [[nodiscard]] vec2f get_flipped_y() const;
        [[nodiscard]] float get_length_squared() const;
        [[nodiscard]] float get_length() const;
        [[nodiscard]] NormalizedAndLength<unit2f, float> get_normalized_and_length() const;
        [[nodiscard]] unit2f get_normalized() const;

        void operator+=(const vec2f& rhs);
        void operator-=(const vec2f& rhs);
        vec2f operator-() const;
        void operator/=(float rhs);
        void operator*=(float rhs);
    };


    struct vec2i
    {
        int x;
        int y;

        vec2i() = default;
        vec2i(int ax, int ay);
        explicit vec2i(const std::tuple<int, int>& a);
        explicit vec2i(const unit2i& u);

        [[nodiscard]] static vec2i zero();
        [[nodiscard]] static vec2i from_to(const vec2i& from, const vec2i& to);

        int* get_data_ptr();
        [[nodiscard]] const int* get_data_ptr() const;

        [[nodiscard]] vec2f tof() const;
        [[nodiscard]] vec2i get_flipped_y() const;

        void operator+=(const vec2i& rhs);
        void operator-=(const vec2i& rhs);
        vec2i operator-() const;
        void operator*=(int rhs);
    };


    ////////////////////////////////////////////////////////////////////////////////

    
    struct unit2f
    {
        float x;
        float y;

        explicit unit2f(float ax, float ay);

        float* get_data_ptr();

        [[nodiscard]] unit2f get_rotated(const angle& a) const;

        [[nodiscard]] const float* get_data_ptr() const;
        [[nodiscard]] vec2f to_vec() const;
        [[nodiscard]] unit2f get_flipped_y() const;

        // todo(Gustav): remove this...
        [[nodiscard]] float get_length_squared() const;
        [[nodiscard]] bool is_valid() const;

        unit2f operator-() const;

    private:
        friend struct vec2f;

        explicit unit2f(const vec2f& v);
    };


    ////////////////////////////////////////////////////////////////////////////////

    
    struct Scale2f
    {
        float x;
        float y;

        Scale2f(float ax, float ay);
        explicit Scale2f(const std::tuple<float, float>& a);
        bool operator==(const Scale2f& rhs) = delete;

        float* get_data_ptr();
        
        [[nodiscard]] Scale2f get_rotated(const angle& a) const;

        [[nodiscard]] const float* get_data_ptr() const;
        [[nodiscard]] Scale2f get_flipped_y() const;

        Scale2f operator-() const;
    };


    ////////////////////////////////////////////////////////////////////////////////
    /// Math operators
    
    vec2f operator+(const vec2f& lhs, const vec2f& rhs);
    vec2f operator-(const vec2f& lhs, const vec2f& rhs);
    vec2f operator*(const vec2f& lhs, float rhs);
    vec2f operator*(float lhs, const vec2f& rhs);
    vec2f operator*(const unit2f& lhs, float rhs);
    vec2f operator*(float lhs, const unit2f& rhs);
    vec2f operator/(const vec2f& lhs, float rhs);

    vec2i operator+(const vec2i& lhs, const vec2i& rhs);
    vec2i operator-(const vec2i& lhs, const vec2i& rhs);
    vec2i operator*(const vec2i& lhs, int rhs);
    vec2i operator*(int lhs, const vec2i& rhs);

    ////////////////////////////////////////////////////////////////////////////////
    /// Equality operators

    bool operator==(const vec2i& lhs, const vec2i& rhs);
    bool operator!=(const vec2i& lhs, const vec2i& rhs);


    ////////////////////////////////////////////////////////////////////////////////
    /// Functions

    float dot(const vec2f& lhs, const vec2f& rhs);

    ////////////////////////////////////////////////////////////////////////////////
    /// Transform

    vec2f lerp_vec2f(const vec2f& from, float v, const vec2f& to);
    vec2i lerp_vec2i(const vec2i& from, float v, const vec2i& to);

    DEFAULT_INTERPOLATE(vec2f, lerp_vec2f);
    DEFAULT_INTERPOLATE(vec2i, lerp_vec2i);

    // todo(Gustav): add one lerp and one slerp unit transform?


    ////////////////////////////////////////////////////////////////////////////////
    /// Printing

    std::ostream& operator<<(std::ostream& s, const vec2f& v);
    std::ostream& operator<<(std::ostream& s, const unit2f& v);
    std::ostream& operator<<(std::ostream& s, const vec2i& v);
    
    // util functions
    unit2f create_random_unit(Random* random);
}


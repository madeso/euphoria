#pragma once

#include "base/vec3.h"
#include "base/vec4.h"
#include "base/angle.h"
#include "base/axisangle.h"
#include "base/mat3.h"



namespace eu
{
    struct mat4f
    {
        [[nodiscard]] static mat4f from_col_major
        (
            float t00, float t01, float t02, float t03,
            float t10, float t11, float t12, float t13,
            float t20, float t21, float t22, float t23,
            float t30, float t31, float t32, float t33
        );

        [[nodiscard]] constexpr static mat4f from_row_major
        (
            float t00, float t10, float t20, float t30,
            float t01, float t11, float t21, float t31,
            float t02, float t12, float t22, float t32,
            float t03, float t13, float t23, float t33
        )
        {
            return
            {
                t00, t01, t02, t03,
                t10, t11, t12, t13,
                t20, t21, t22, t23,
                t30, t31, t32, t33
            };
        }

        [[nodiscard]] static mat4f from_major(const vec4f& major);
        [[nodiscard]] static mat4f from_scale(const Scale3f& scale);
        [[nodiscard]] static mat4f from_translation(const vec3f& v);
        [[nodiscard]] static mat4f from_rot_x(const Angle& a);
        [[nodiscard]] static mat4f from_rot_y(const Angle& a);
        [[nodiscard]] static mat4f from_rot_z(const Angle& a);
        [[nodiscard]] static mat4f from_axis_angle(const AxisAngle& aa);
        [[nodiscard]] static mat4f create_ortho(float l, float r, float b, float t, float n, float f);
        [[nodiscard]] static mat4f create_perspective(const Angle& fov, float a, float near, float far);
        [[nodiscard]] constexpr static mat4f from_scalar(float scalar)
        {
            const float z = 0;
            return from_row_major
            (
                scalar, z, z, z,
                z, scalar, z, z,
                z, z, scalar, z,
                z, z, z, scalar
            );
        }

        float* get_data_ptr();
        float* get_column_major();

        bool invert();

        [[nodiscard]] float get(int row, int col) const;
        [[nodiscard]] vec4f get_transform(const vec4f& p) const;
        [[nodiscard]] vec3f get_transform(const vec3f& p, float w) const;
        [[nodiscard]] vec3f get_transform_point(const vec3f& p) const;
        [[nodiscard]] vec3f get_transform_vec(const vec3f& p) const;
        [[nodiscard]] unit3f get_transform_vec(const unit3f& p) const;
        [[nodiscard]] unit3f get_axis(int col) const;
        [[nodiscard]] mat4f get_translated(const vec3f& t) const;
        [[nodiscard]] mat4f get_rotated(const AxisAngle& aa) const;
        [[nodiscard]] mat4f get_scaled(const Scale3f& scale) const;
        [[nodiscard]] vec4f get_column(int c) const;
        [[nodiscard]] vec4f get_row(int r) const;

        [[nodiscard]] const float* get_column_major() const;
        [[nodiscard]] const float* get_data_ptr() const;

        [[nodiscard]] vec3f get_translation() const;
        [[nodiscard]] vec4f get_major() const;
        [[nodiscard]] unit3f get_x_axis() const;
        [[nodiscard]] unit3f get_y_axis() const;
        [[nodiscard]] unit3f get_z_axis() const;
        [[nodiscard]] mat4f get_transposed() const;
        [[nodiscard]] mat4f get_inverted() const;
        [[nodiscard]] mat3f get_mat3() const;

        void operator+=(const mat4f& rhs);
        void operator-=(const mat4f& rhs);
        
    private:
        float data[16]; // col-major

        mat4f() = default;

        constexpr mat4f
        (
            float t00, float t01, float t02, float t03,
            float t10, float t11, float t12, float t13,
            float t20, float t21, float t22, float t23,
            float t30, float t31, float t32, float t33
        )
            : data
            {
                t00, t01, t02, t03,
                t10, t11, t12, t13,
                t20, t21, t22, t23,
                t30, t31, t32, t33
            }
        {
        }
    };


    constexpr mat4f m4_identity = mat4f::from_scalar(1);

    
    std::string to_string(const mat4f& m);

    mat4f operator+(const mat4f& lhs, const mat4f& rhs);
    mat4f operator-(const mat4f& lhs, const mat4f& rhs);
    mat4f operator*(const mat4f& lhs, const mat4f& rhs);
    vec4f operator*(const mat4f& lhs, const vec4f& rhs);
}

ADD_DEFAULT_FORMATTER(eu::mat4f, std::string, eu::to_string);

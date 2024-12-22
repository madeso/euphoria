#pragma once

#include "base/vec3.h"
#include "base/vec4.h"
#include "base/angle.h"
#include "base/axisangle.h"

namespace eu
{
    /// 4x4 matrix
    struct m4
    {
        [[nodiscard]] static m4 from_col_major(
            float t00, float t01, float t02, float t03,
            float t10, float t11, float t12, float t13,
            float t20, float t21, float t22, float t23,
            float t30, float t31, float t32, float t33);

        [[nodiscard]] constexpr static m4 from_row_major(
            float t00, float t10, float t20, float t30,
            float t01, float t11, float t21, float t31,
            float t02, float t12, float t22, float t32,
            float t03, float t13, float t23, float t33)
        {
            return {
                t00, t01, t02, t03,
                t10, t11, t12, t13,
                t20, t21, t22, t23,
                t30, t31, t32, t33};
        }

        [[nodiscard]] static m4 from_major(const v4 &major);
        [[nodiscard]] static m4 from_translation(const v3 &v);
        [[nodiscard]] static m4 from_rot_x(const An &a);
        [[nodiscard]] static m4 from_rot_y(const An &a);
        [[nodiscard]] static m4 from_rot_z(const An &a);
        [[nodiscard]] static m4 from(const AA &aa);
        [[nodiscard]] static m4 create_ortho(float l, float r, float b, float t, float n, float f);
        [[nodiscard]] static m4 create_perspective(const An &fov, float a, float near, float far);
        [[nodiscard]] constexpr static m4 from_scalar(float scalar)
        {
            const float z = 0;
            return from_row_major(
                scalar, z, z, z,
                z, scalar, z, z,
                z, z, scalar, z,
                z, z, z, scalar);
        }

        float *get_data_ptr();
        float *get_column_major();

        bool invert();

        [[nodiscard]] float get(int row, int col) const;
        [[nodiscard]] v4 get_transform(const v4 &p) const;
        [[nodiscard]] v3 get_transform(const v3 &p, float w) const;
        [[nodiscard]] v3 get_transform_point(const v3 &p) const;
        [[nodiscard]] v3 get_transform_vec(const v3 &p) const;
        [[nodiscard]] n3 get_transform_vec(const n3 &p) const;
        [[nodiscard]] n3 get_axis(int col) const;
        [[nodiscard]] m4 get_translated(const v3 &t) const;
        [[nodiscard]] m4 get_rotated(const AA &aa) const;
        [[nodiscard]] v4 get_column(int c) const;
        [[nodiscard]] v4 get_row(int r) const;

        [[nodiscard]] const float *get_column_major() const;
        [[nodiscard]] const float *get_data_ptr() const;

        [[nodiscard]] v3 get_translation() const;
        [[nodiscard]] v4 get_major() const;
        [[nodiscard]] n3 get_x_axis() const;
        [[nodiscard]] n3 get_y_axis() const;
        [[nodiscard]] n3 get_z_axis() const;
        [[nodiscard]] m4 get_transposed() const;
        [[nodiscard]] m4 get_inverted() const;

        void operator+=(const m4 &rhs);
        void operator-=(const m4 &rhs);

    private:
        float data[16]; // col-major

        m4() = default;

        constexpr m4(
            float t00, float t01, float t02, float t03,
            float t10, float t11, float t12, float t13,
            float t20, float t21, float t22, float t23,
            float t30, float t31, float t32, float t33)
            : data{
                  t00, t01, t02, t03,
                  t10, t11, t12, t13,
                  t20, t21, t22, t23,
                  t30, t31, t32, t33}
        {
        }
    };

    constexpr m4 m4_identity = m4::from_scalar(1);

    std::string string_from(const m4 &m);

    m4 operator+(const m4 &lhs, const m4 &rhs);
    m4 operator-(const m4 &lhs, const m4 &rhs);
    m4 operator*(const m4 &lhs, const m4 &rhs);
    v4 operator*(const m4 &lhs, const v4 &rhs);
}

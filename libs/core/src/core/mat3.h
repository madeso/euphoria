#pragma once

#include "core/vec2.h"
#include "core/vec3.h"
#include "core/mat2.h"
#include "core/angle.h"
#include "core/axisangle.h"

namespace euphoria::core
{
    struct mat3f
    {
    private:
        float data[9]; // col-major

        mat3f() = default;

        mat3f
        (
            float t00, float t01, float t02,
            float t10, float t11, float t12,
            float t20, float t21, float t22
        );

    public:
        explicit mat3f(const mat2f& mat);
        [[nodiscard]] static mat3f from_major(const Scale3f& major);
        [[nodiscard]] static mat3f from_scale(const Scale3f& scale);
        [[nodiscard]] static mat3f from_translation2d(const vec2f& t);
        [[nodiscard]] static mat3f from_scalar(float scalar);
        [[nodiscard]] static mat3f from_rot_x(const angle& a);
        [[nodiscard]] static mat3f from_rot_y(const angle& a);
        [[nodiscard]] static mat3f from_rot_z(const angle& a);
        [[nodiscard]] static mat3f from_axis_angle(const AxisAngle& aa);
        [[nodiscard]] static mat3f identity();

        [[nodiscard]] static mat3f from_col_major
        (
            float t00, float t01, float t02,
            float t10, float t11, float t12,
            float t20, float t21, float t22
        );

        [[nodiscard]] static mat3f from_row_major
        (
            float t00, float t10, float t20,
            float t01, float t11, float t21,
            float t02, float t12, float t22
        );

        [[nodiscard]] vec3f get_major() const;
        [[nodiscard]] vec3f get_axis(int col) const;
        [[nodiscard]] vec3f get_x_axis() const;
        [[nodiscard]] vec3f get_y_axis() const;
        [[nodiscard]] vec3f get_z_axis() const;
        [[nodiscard]] mat3f get_transposed() const;

        void operator+=(const mat3f& rhs);
        void operator-=(const mat3f& rhs);

        // todo(Gustav): rename to get_...d (get_rotated etc.)
        [[nodiscard]] mat3f rotate(const AxisAngle& aa) const;
        [[nodiscard]] mat3f scale(const Scale3f& scale) const;

        [[nodiscard]] const float* get_data_ptr() const;
        float* get_data_ptr();

        // index operator use () as [] only expects one argument, fix in c++23
        float& operator()(int row, int col);
        float operator()(int row, int col) const;

        [[nodiscard]] float get(int row, int col) const;
        [[nodiscard]] vec3f get_column(int c) const;
        [[nodiscard]] vec3f get_row(int r) const;

        bool operator==(const mat3f& rhs) = delete;
    };

    std::ostream& operator<<(std::ostream& stream, const mat3f& m);
    mat3f operator+(const mat3f& lhs, const mat3f& rhs);
    mat3f operator-(const mat3f& lhs, const mat3f& rhs);
    mat3f operator*(const mat3f& lhs, const mat3f& rhs);
    vec3f operator*(const mat3f& lhs, const vec3f& rhs);
}

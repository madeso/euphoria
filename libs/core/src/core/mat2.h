#pragma once

#include "core/vec2.h"
#include "core/angle.h"


namespace euphoria::core
{
    struct mat2f
    {
        bool operator==(const mat2f& rhs) = delete;

        [[nodiscard]] static mat2f from_col_major
        (
            float t00, float t01,
            float t10, float t11
        );

        [[nodiscard]] constexpr static mat2f from_row_major
        (
            float t00, float t10,
            float t01, float t11
        )
        {
            return mat2f
            (
                t00, t01,
                t10, t11
            );
        }

        [[nodiscard]] constexpr static mat2f from_scalar(float scalar)
        {
            const float z = 0;
            return from_row_major
            (
                scalar, z,
                z, scalar
            );
        }
        [[nodiscard]] static mat2f from_stretch_x(float k);
        [[nodiscard]] static mat2f from_stretch_y(float k);
        [[nodiscard]] static mat2f from_rotation(const angle& a);

        float* get_data_ptr();

        [[nodiscard]] float get(int row, int col) const;
        [[nodiscard]] vec2f get_column(int c) const;
        [[nodiscard]] vec2f get_row(int r) const;

        [[nodiscard]] const float* get_data_ptr() const;

        void operator+=(const mat2f& rhs);
        void operator-=(const mat2f& rhs);
        // index operator use () as [] only expects one argument
        float& operator()(int row, int col);
        float operator()(int row, int col) const;

    private:
        float data[4];

        mat2f() = default;

        constexpr mat2f
        (
            float t00, float t01,
            float t10, float t11
        )
            : data
            {
                t00, t01,
                t10, t11
            }
        {
        }
    };

    constexpr mat2f m2_identity = mat2f::from_scalar(1);


    std::string to_string(const mat2f& m);

    mat2f operator+(const mat2f& lhs, const mat2f& rhs);
    mat2f operator-(const mat2f& lhs, const mat2f& rhs);
    mat2f operator*(const mat2f& lhs, const mat2f& rhs);
    vec2f operator*(const mat2f& lhs, const vec2f& rhs);
}

// todo(Gustav): add a multi line format option?
ADD_DEFAULT_FORMATTER(euphoria::core::mat2f, std::string, euphoria::core::to_string);

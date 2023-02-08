#pragma once

#include "core/vec2.h"
#include "core/angle.h"


namespace euphoria::core
{
    struct mat2f
    {
    private:
        float data[4];

        mat2f() = default;

        mat2f
        (
            float t00, float t01,
            float t10, float t11
        );

    public:

        [[nodiscard]] static mat2f from_col_major
        (
            float t00, float t01,
            float t10, float t11
        );

        [[nodiscard]] static mat2f from_row_major
        (
            float t00, float t10,
            float t01, float t11
        );

        [[nodiscard]] static mat2f from_scalar(float scalar);
        [[nodiscard]] static mat2f from_stretch_x(float k);
        [[nodiscard]] static mat2f from_stretch_y(float k);
        [[nodiscard]] static mat2f from_rotation(const angle& a);
        [[nodiscard]] static mat2f identity();

        void operator+=(const mat2f rhs);
        void operator-=(const mat2f rhs);

        const float* get_data_ptr() const;
        float* get_data_ptr();

        // index operator use () as [] only expects one argument
        float& operator()(int row, int col);
        float operator()(int row, int col) const;
        float get(int row, int col) const;

        vec2f get_column(int c) const;
        vec2f get_row(int r) const;

        bool operator==(const mat2f& rhs) = delete;
    };


    std::ostream&
    operator<<(std::ostream& stream, const mat2f& m);

    mat2f operator+(const mat2f& lhs, const mat2f rhs);
    mat2f operator-(const mat2f& lhs, const mat2f rhs);
    mat2f operator*(const mat2f& lhs, const mat2f rhs);
    vec2f operator*(const mat2f& lhs, const vec2f rhs);
}

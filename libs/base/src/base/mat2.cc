#include "base/mat2.h"



namespace eu
{   
    [[nodiscard]] mat2f
    mat2f::from_col_major
    (
        float t00, float t01,
        float t10, float t11
    )
    {
        return {t00, t01, t10, t11};
    }


    [[nodiscard]] mat2f
    mat2f::from_stretch_x(float k)
    {
        return from_row_major
        (
            k, 0,
            0, 1
        );
    }


    [[nodiscard]] mat2f
    mat2f::from_stretch_y(float k)
    {
        return from_row_major
        (
            1, 0,
            0, k
        );
    }

    [[nodiscard]] mat2f
    mat2f::from_rotation(const Angle& a)
    {
        const auto s = sin(a);
        const auto c = cos(a);
        return from_row_major
        (
            c, s,
            -s, c
        );
    }


    void
    mat2f::operator+=(const mat2f& rhs)
    {
#define OP(i) data[i] += rhs.data[i]
        OP(0); OP(1);
        OP(2); OP(3);
#undef OP
    }


    void
    mat2f::operator-=(const mat2f& rhs)
    {
#define OP(i) data[i] -= rhs.data[i]
        OP(0); OP(1);
        OP(2); OP(3);
#undef OP
    }


    const float*
    mat2f::get_data_ptr() const
    {
        return data;
    }


    float*
    mat2f::get_data_ptr()
    {
        return data;
    }


    float
    mat2f::get(int row, int col) const
    {
        return data[col * 2 + row];
    }


    vec2f
    mat2f::get_column(int c) const
    {
        return {get(0, c), get(1, c)};
    }


    vec2f
    mat2f::get_row(int r) const
    {
        return {get(r, 0), get(r, 1)};
    }


    std::string to_string(const mat2f& m)
    {
        return fmt::format("({}, {})", m.get_row(0), m.get_row(1));
    }


    mat2f
    operator+(const mat2f& lhs, const mat2f& rhs)
    {
        mat2f t = lhs;
        t += rhs;
        return t;
    }


    mat2f
    operator-(const mat2f& lhs, const mat2f& rhs)
    {
        mat2f t = lhs;
        t -= rhs;
        return t;
    }

    float
    get_component_multiply_sum(const vec2f& lhs, const vec2f& rhs)
    {
        const auto x = lhs.x * rhs.x;
        const auto y = lhs.y * rhs.y;
        return x + y;
    }

    mat2f operator*(const mat2f& lhs, const mat2f& rhs)
    {
#define OP(r, c) \
    get_component_multiply_sum(lhs.get_row(r), rhs.get_column(c))
        return mat2f::from_row_major
        (
            OP(0, 0), OP(0, 1),
            OP(1, 0), OP(1, 1)
        );
#undef OP
    }


    vec2f operator*(const mat2f& lhs, const vec2f& rhs)
    {
#define OP(r) get_component_multiply_sum(lhs.get_row(r), rhs)
        return {OP(0), OP(1)};
#undef OP
    }

 
}



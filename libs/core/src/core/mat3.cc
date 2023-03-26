#include "core/mat3.h"



namespace euphoria::core
{
    mat3f::mat3f(const mat2f& mat)
        : data
        {
            mat(0,0), mat(0,1), 0,
            mat(1,0), mat(1,1), 0,
            0,        0,        1
        }
    {
    }


    [[nodiscard]]
    mat3f
    mat3f::from_col_major
    (
        float t00, float t01, float t02,
        float t10, float t11, float t12,
        float t20, float t21, float t22
    )
    {
        return mat3f
        (
            t00, t01, t02,
            t10, t11, t12,
            t20, t21, t22
        );
    }


    [[nodiscard]]
    mat3f
    mat3f::from_major(const Scale3f& major)
    {
        const float zero = 0;
        return from_row_major
        (
            major.x, zero, zero,
            zero, major.y, zero,
            zero, zero, major.z
        );
    }


    [[nodiscard]]
    mat3f
    mat3f::from_scale(const Scale3f& scale)
    {
        return from_major(scale);
    }


    [[nodiscard]]
    mat3f
    mat3f::from_translation2d(const vec2f& t)
    {
        return from_row_major
        (
            1, 0, t.x,
            0, 1, t.y,
            0, 0, 1
        );
    }


    [[nodiscard]]
    mat3f
    mat3f::from_rot_x(const angle& a)
    {
        const auto c = cos(a);
        const auto s = sin(a);
        return from_row_major
        (
            1, 0, 0,
            0, c, -s,
            0, s, c
        );
    }


    [[nodiscard]]
    mat3f
    mat3f::from_rot_y(const angle& a)
    {
        const auto c = cos(a);
        const auto s = sin(a);
        return from_row_major
        (
            c, 0, s,
            0, 1, 0,
            -s, 0, c
        );
    }


    [[nodiscard]]
    mat3f
    mat3f::from_rot_z(const angle& a)
    {
        const auto c = cos(a);
        const auto s = sin(a);
        return from_row_major(c, -s, 0, s, c, 0, 0, 0, 1);
    }


    [[nodiscard]]
    mat3f
    mat3f::from_axis_angle(const AxisAngle& aa)
    {
        const float rcos = cos(aa.angle);
        const float rsin = sin(aa.angle);

        const auto u = aa.axis.x;
        const auto v = aa.axis.y;
        const auto w = aa.axis.z;

        return mat3f::from_col_major
        (
            rcos + u * u * (1 - rcos),
            w * rsin + v * u * (1 - rcos),
            -v * rsin + w * u * (1 - rcos),

            -w * rsin + u * v * (1 - rcos),
            rcos + v * v * (1 - rcos),
            u * rsin + w * v * (1 - rcos),

            v * rsin + u * w * (1 - rcos),
            -u * rsin + v * w * (1 - rcos),
            rcos + w * w * (1 - rcos)
        );
    }


    vec3f
    mat3f::get_major() const
    {
        const mat3f& self = *this;
        return vec3f(self(0, 0), self(1, 1), self(2, 2));
    }


    vec3f
    mat3f::get_axis(int col) const
    {
        return get_column(col);
    }


    vec3f
    mat3f::get_x_axis() const
    {
        return get_axis(0);
    }


    vec3f
    mat3f::get_y_axis() const
    {
        return get_axis(1);
    }


    vec3f
    mat3f::get_z_axis() const
    {
        return get_axis(2);
    }


    mat3f
    mat3f::get_transposed() const
    {
        const mat3f& self = *this;
        return from_col_major
        (
            self(0, 0), self(0, 1), self(0, 2),
            self(1, 0), self(1, 1), self(1, 2),
            self(2, 0), self(2, 1), self(2, 2)
        );
    }


    void
    mat3f::operator+=(const mat3f& rhs)
    {
#define OP(i) data[i] += rhs.data[i]
        OP(0); OP(1); OP(2);
        OP(3); OP(4); OP(5);
        OP(6); OP(7); OP(8);
#undef OP
    }


    void
    mat3f::operator-=(const mat3f& rhs)
    {
#define OP(i) data[i] -= rhs.data[i]
        OP(0); OP(1); OP(2);
        OP(3); OP(4); OP(5);
        OP(6); OP(7); OP(8);
#undef OP
    }


    mat3f
    mat3f::get_rotated(const AxisAngle& aa) const
    {
        return *this * from_axis_angle(aa);
    }


    mat3f
    mat3f::get_scaled(const Scale3f& scale) const
    {
        return *this * from_scale(scale);
    }


    const float*
    mat3f::get_data_ptr() const
    {
        return data;
    }


    float*
    mat3f::get_data_ptr()
    {
        return data;
    }


    float&
    mat3f::operator()(int row, int col)
    {
        return data[col * 3 + row];
    }


    float
    mat3f::operator()(int row, int col) const
    {
        return get(row, col);
    }


    float
    mat3f::get(int row, int col) const
    {
        return data[col * 3 + row];
    }


    vec3f
    mat3f::get_column(int c) const
    {
        return vec3f(get(0, c), get(1, c), get(2, c));
    }


    vec3f
    mat3f::get_row(int r) const
    {
        return vec3f(get(r, 0), get(r, 1), get(r, 2));
    }


    std::string to_string(const mat3f& m)
    {
        return fmt::format("({}, {}, {})", m.get_row(0), m.get_row(1), m.get_row(2));
    }


    mat3f
    operator+(const mat3f& lhs, const mat3f& rhs)
    {
        mat3f t = lhs;
        t += rhs;
        return t;
    }


    mat3f
    operator-(const mat3f& lhs, const mat3f& rhs)
    {
        mat3f t = lhs;
        t -= rhs;
        return t;
    }

    float
    get_component_multiply_sum(const vec3f& lhs, const vec3f& rhs)
    {
        const auto x = lhs.x * rhs.x;
        const auto y = lhs.y * rhs.y;
        const auto z = lhs.z* rhs.z;
        return x + y + z;
    }


    mat3f operator*(const mat3f& lhs, const mat3f& rhs)
    {
#define OP(r, c) \
    get_component_multiply_sum(vec3f{lhs.get_row(r)}, vec3f{rhs.get_column(c)})
        return mat3f::from_row_major
        (
            OP(0, 0), OP(0, 1), OP(0, 2),
            OP(1, 0), OP(1, 1), OP(1, 2),
            OP(2, 0), OP(2, 1), OP(2, 2)
        );
#undef OP
    }


    vec3f operator*(const mat3f& lhs, const vec3f& rhs)
    {
#define OP(r) get_component_multiply_sum(vec3f{lhs.get_row(r)}, rhs)
        return vec3f(OP(0), OP(1), OP(2));
#undef OP
    }


}

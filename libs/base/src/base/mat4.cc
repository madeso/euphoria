#include "base/mat4.h"

#include "base/quat.h"

namespace eu
{
    [[nodiscard]]
    m4
    m4::from_col_major
    (
        float t00, float t01, float t02, float t03,
        float t10, float t11, float t12, float t13,
        float t20, float t21, float t22, float t23,
        float t30, float t31, float t32, float t33
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

    [[nodiscard]]
    m4
    m4::from_major(const v4& major)
    {
        const float zero = 0;
        return from_row_major
        (
            major.x, zero, zero, zero,
            zero, major.y, zero, zero,
            zero, zero, major.z, zero,
            zero, zero, zero, major.w
        );
    }

    [[nodiscard]]
    m4
    m4::from_translation(const v3& v)
    {
        const float one = 1;
        const float z = 0;
        return from_row_major
        (
            one, z, z, v.x,
            z, one, z, v.y,
            z, z, one, v.z,
            z, z, z, one
        );
    }

    v4
    m4::get_transformed(const v4& p) const
    {
        return *this * p;
    }

    v3
    m4::get_transformed_point(const v3& p) const
    {
        return get_transformed(v4{p, 1.0f}).to_vec3_persp_div();
    }

    v3
    m4::get_transformed_vec(const v3& p) const
    {
        return get_transformed(v4{p, 0.0f}).to_vec3(0.0f);
    }

    n3
    m4::get_transformed_vec(const n3& p) const
    {
        return n3{ get_transformed_vec(static_cast<v3>(p))};
    }

    v3
    m4::get_translation() const
    {
        return {get(0, 3), get(1, 3), get(2, 3)};
    }

    [[nodiscard]]
    m4
    m4::from_rot_x(const An& a)
    {
        const auto c = cos(a);
        const auto s = sin(a);
        return from_row_major
        (
            1, 0, 0, 0,
            0, c, -s, 0,
            0, s, c, 0,
            0, 0, 0, 1
        );
    }

    m4
    m4::from_rot_y(const An& a)
    {
        const auto c = cos(a);
        const auto s = sin(a);
        return from_row_major
        (
            c, 0, s, 0,
            0, 1, 0, 0,
            -s, 0, c, 0,
            0, 0, 0, 1
        );
    }

    [[nodiscard]]
    m4
    m4::from_rot_z(const An& a)
    {
        const auto c = cos(a);
        const auto s = sin(a);
        return from_row_major
        (
            c, -s, 0, 0,
            s, c, 0, 0,
            0, 0, 1, 0,
            0, 0, 0, 1
        );
    }

    [[nodiscard]]
    m4
    m4::from(const AA& aa)
    {
        const float rcos = cos(aa.angle);
        const float rsin = sin(aa.angle);

        const auto u = aa.axis.x;
        const auto v = aa.axis.y;
        const auto w = aa.axis.z;

        return m4::from_col_major
        (
            rcos + u * u * (1 - rcos),
            w * rsin + v * u * (1 - rcos),
            -v * rsin + w * u * (1 - rcos),
            0,

            -w * rsin + u * v * (1 - rcos),
            rcos + v * v * (1 - rcos),
            u * rsin + w * v * (1 - rcos),
            0,

            v * rsin + u * w * (1 - rcos),
            -u * rsin + v * w * (1 - rcos),
            rcos + w * w * (1 - rcos),
            0,

            0,
            0,
            0,
            1
        );
    }


    [[nodiscard]] std::optional<m4>
    m4::from(const Q& q)
    {
        const auto aa = AA::from(q);
        if (!aa)
            { return std::nullopt; }
        return m4::from(*aa);
    }

    
    v4
    m4::get_major() const
    {
        return {get(0, 0), get(1, 1), get(2, 2), get(3, 3)};
    }

    n3
    get_axis(const m4& m, int col)
    {
        return n3{m.get_column(col).to_vec3(0.0f)};
    }

    n3
    m4::get_x_axis() const
    {
        return get_axis(*this, 0);
    }

    n3
    m4::get_y_axis() const
    {
        return get_axis(*this, 1);
    }

    n3
    m4::get_z_axis() const
    {
        return get_axis(*this, 2);
    }

    m4
    m4::get_transposed() const
    {
        // https://www.j3d.org/matrix_faq/matrfaq_latest.html
        return from_col_major
        (
            get(0, 0), get(0, 1), get(0, 2), get(0, 3),
            get(1, 0), get(1, 1), get(1, 2), get(1, 3),
            get(2, 0), get(2, 1), get(2, 2), get(2, 3),
            get(3, 0), get(3, 1), get(3, 2), get(3, 3)
        );
    }

    bool
    m4::invert()
    {
        // from the glu source?
        // https://stackoverflow.com/questions/1148309/inverting-a-4x4-matrix

        float inv[16];

        float* m = data;

        inv[0] =
            m[5] * m[10] * m[15] - m[5] * m[11] * m[14] -
            m[9] * m[6] * m[15] + m[9] * m[7] * m[14] +
            m[13] * m[6] * m[11] - m[13] * m[7] * m[10]
            ;

        inv[4] =
            -m[4] * m[10] * m[15] + m[4] * m[11] * m[14] +
            m[8] * m[6] * m[15] - m[8] * m[7] * m[14] -
            m[12] * m[6] * m[11] + m[12] * m[7] * m[10]
            ;

        inv[8] =
            m[4] * m[9] * m[15] - m[4] * m[11] * m[13] -
            m[8] * m[5] * m[15] + m[8] * m[7] * m[13] +
            m[12] * m[5] * m[11] - m[12] * m[7] * m[9]
            ;

        inv[12] =
            -m[4] * m[9] * m[14] + m[4] * m[10] * m[13] +
            m[8] * m[5] * m[14] - m[8] * m[6] * m[13] -
            m[12] * m[5] * m[10] + m[12] * m[6] * m[9]
            ;

        inv[1] =
            -m[1] * m[10] * m[15] + m[1] * m[11] * m[14] +
            m[9] * m[2] * m[15] - m[9] * m[3] * m[14] -
            m[13] * m[2] * m[11] + m[13] * m[3] * m[10]
            ;

        inv[5] =
            m[0] * m[10] * m[15] - m[0] * m[11] * m[14] -
            m[8] * m[2] * m[15] + m[8] * m[3] * m[14] +
            m[12] * m[2] * m[11] - m[12] * m[3] * m[10]
            ;

        inv[9] =
            -m[0] * m[9] * m[15] + m[0] * m[11] * m[13] +
            m[8] * m[1] * m[15] - m[8] * m[3] * m[13] -
            m[12] * m[1] * m[11] + m[12] * m[3] * m[9]
            ;

        inv[13] =
            m[0] * m[9] * m[14] - m[0] * m[10] * m[13] -
            m[8] * m[1] * m[14] + m[8] * m[2] * m[13] +
            m[12] * m[1] * m[10] - m[12] * m[2] * m[9]
            ;

        inv[2] =
            m[1] * m[6] * m[15] - m[1] * m[7] * m[14] -
            m[5] * m[2] * m[15] + m[5] * m[3] * m[14] +
            m[13] * m[2] * m[7] - m[13] * m[3] * m[6]
            ;

        inv[6] =
            -m[0] * m[6] * m[15] + m[0] * m[7] * m[14] +
            m[4] * m[2] * m[15] - m[4] * m[3] * m[14] -
            m[12] * m[2] * m[7] + m[12] * m[3] * m[6]
            ;

        inv[10] =
            m[0] * m[5] * m[15] - m[0] * m[7] * m[13] -
            m[4] * m[1] * m[15] + m[4] * m[3] * m[13] +
            m[12] * m[1] * m[7] - m[12] * m[3] * m[5]
            ;

        inv[14] =
            -m[0] * m[5] * m[14] + m[0] * m[6] * m[13] +
            m[4] * m[1] * m[14] - m[4] * m[2] * m[13] -
            m[12] * m[1] * m[6] + m[12] * m[2] * m[5]
            ;

        inv[3] =
            -m[1] * m[6] * m[11] + m[1] * m[7] * m[10] +
            m[5] * m[2] * m[11] - m[5] * m[3] * m[10] -
            m[9] * m[2] * m[7] + m[9] * m[3] * m[6]
            ;

        inv[7] =
            m[0] * m[6] * m[11] - m[0] * m[7] * m[10] -
            m[4] * m[2] * m[11] + m[4] * m[3] * m[10] +
            m[8] * m[2] * m[7] - m[8] * m[3] * m[6]
            ;

        inv[11] =
            -m[0] * m[5] * m[11] + m[0] * m[7] * m[9] +
            m[4] * m[1] * m[11] - m[4] * m[3] * m[9] -
            m[8] * m[1] * m[7] + m[8] * m[3] * m[5]
            ;

        inv[15] =
            m[0] * m[5] * m[10] - m[0] * m[6] * m[9] -
            m[4] * m[1] * m[10] + m[4] * m[2] * m[9] +
            m[8] * m[1] * m[6] - m[8] * m[2] * m[5]
            ;

        const auto inv_det =
            m[0] * inv[0] + m[1] * inv[4] + m[2] * inv[8] +
            m[3] * inv[12];

        if(inv_det == 0)
        {
            return false;
        }

        const auto det = 1.0f / inv_det;

        for(int index = 0; index < 16; index += 1)
        {
            data[index] = inv[index] * det;
        }

        return true;
    }

    m4
    m4::get_inverted() const
    {
        auto r = *this;
        const auto was_inverted = r.invert();
        ASSERT(was_inverted);
        return r;
    }

    void
    m4::operator+=(const m4& rhs)
    {
#define OP(i) data[i] += rhs.data[i]
        OP(0); OP(1); OP(2); OP(3);
        OP(4); OP(5); OP(6); OP(7);
        OP(8); OP(9); OP(10); OP(11);
        OP(12); OP(13); OP(14); OP(15);
#undef OP
    }

    void
    m4::operator-=(const m4& rhs)
    {
#define OP(i) data[i] -= rhs.data[i]
        OP(0); OP(1); OP(2); OP(3);
        OP(4); OP(5); OP(6); OP(7);
        OP(8); OP(9); OP(10); OP(11);
        OP(12); OP(13); OP(14); OP(15);
#undef OP
    }

    [[nodiscard]]
    m4
    m4::create_ortho_lrud(float l, float r, float t, float b, float n, float f)
    {
        // http://www.songho.ca/opengl/gl_projectionmatrix.html
        return from_row_major
        (
            2 / (r - l), 0, 0, -(r + l) / (r - l),
            0, 2 / (t - b), 0, -(t + b) / (t - b),
            0, 0, -2 / (f - n), -(f + n) / (f - n),
            0, 0, 0, 1
        );
    }

    [[nodiscard]]
    m4
    m4::create_perspective(const An& fov, float a, float near, float far)
    {
        const float t = 1 / tan(fov / 2);
        const float zm = far - near;
        const float zp = far + near;
        return from_row_major
        (
            t / a, 0, 0, 0,
            0, t, 0, 0,
            0, 0, -zp / zm, (-2 * far * near) / zm,
            0, 0, -1, 0
        );
    }

    m4
    m4::get_translated(const v3& t) const
    {
        return *this * from_translation(t);
    }

    m4
    m4::get_rotated(const AA& aa) const
    {
        return *this * from(aa);
    }

    const float*
    m4::get_column_major_data_ptr() const
    {
        return data;
    }

    float*
    m4::get_column_major_data_ptr()
    {
        return data;
    }


    float
    m4::get(int row, int col) const
    {
        return data[col * 4 + row];
    }

    v4
    m4::get_column(int col) const
    {
        const auto base = col * 4;
        return
        {
            data[base + 0],
            data[base + 1],
            data[base + 2],
            data[base + 3]
        };
    }

    v4
    m4::get_row(int r) const
    {
        return {get(r, 0), get(r, 1), get(r, 2), get(r, 3)};
    }

    
    std::string string_from(const m4& m)
    {
        return fmt::format("({}, {}. {}. {})", m.get_row(0), m.get_row(1), m.get_row(2), m.get_row(3));
    }

    
    m4
    operator+(const m4& lhs, const m4& rhs)
    {
        m4 t = lhs;
        t += rhs;
        return t;
    }

    
    m4
    operator-(const m4& lhs, const m4& rhs)
    {
        m4 t = lhs;
        t -= rhs;
        return t;
    }

    float
    get_component_multiply_sum(const v4& lhs, const v4& rhs)
    {
        const auto x = lhs.x * rhs.x;
        const auto y = lhs.y * rhs.y;
        const auto z = lhs.z * rhs.z;
        const auto w  = lhs.w* rhs.w;
        return x + y + z + w;
    }
    
    m4 operator*(const m4& lhs, const m4& rhs)
    {
        const auto op = [&lhs, &rhs](int r, int c) -> float
        {
            return get_component_multiply_sum(lhs.get_row(r), rhs.get_column(c));
        };
        return m4::from_row_major
        (
            op(0, 0), op(0, 1), op(0, 2), op(0, 3),
            op(1, 0), op(1, 1), op(1, 2), op(1, 3),
            op(2, 0), op(2, 1), op(2, 2), op(2, 3),
            op(3, 0), op(3, 1), op(3, 2), op(3, 3)
        );
    }

    
    v4 operator*(const m4& lhs, const v4& rhs)
    {
        const auto op = [&lhs, &rhs](int r) -> float
        {
            return get_component_multiply_sum(lhs.get_row(r), rhs);
        };

        return {op(0), op(1), op(2), op(3)};
    }

    ADD_CATCH_FORMATTER_IMPL(m4)
}

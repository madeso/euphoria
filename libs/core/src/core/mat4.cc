#include "core/mat4.h"


namespace euphoria::core
{
    float* mat4f::get_column_major() { return data; }
    const float* mat4f::get_column_major() const { return data; }

    [[nodiscard]]
    mat4f
    mat4f::from_col_major
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
    mat4f
    mat4f::from_major(const vec4f& major)
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
    mat4f
    mat4f::from_scale(const Scale3f& scale)
    {
        return from_major(vec4f(scale));
    }

    [[nodiscard]]
    mat4f
    mat4f::from_translation(const vec3f& v)
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

    vec4f
    mat4f::get_transform(const vec4f& p) const
    {
        return *this * p;
    }

    vec3f
    mat4f::get_transform(const vec3f& p, float w) const
    {
        return get_transform(vec4f{p, w}).to_vec3(w);
    }

    vec3f
    mat4f::get_transform_point(const vec3f& p) const
    {
        return get_transform(p, 1);
    }

    vec3f
    mat4f::get_transform_vec(const vec3f& p) const
    {
        return get_transform(p, 0);
    }

    unit3f
    mat4f::get_transform_vec(const unit3f& p) const
    {
        return unit3f::to_unit(get_transform_vec(static_cast<vec3f>(p)));
    }

    vec3f
    mat4f::get_translation() const
    {
        return {get(0, 3), get(1, 3), get(2, 3)};
    }

    [[nodiscard]]
    mat4f
    mat4f::from_rot_x(const Angle& a)
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

    mat4f
    mat4f::from_rot_y(const Angle& a)
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
    mat4f
    mat4f::from_rot_z(const Angle& a)
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
    mat4f
    mat4f::from_axis_angle(const AxisAngle& aa)
    {
        const float rcos = cos(aa.angle);
        const float rsin = sin(aa.angle);

        const auto u = aa.axis.x;
        const auto v = aa.axis.y;
        const auto w = aa.axis.z;

        return mat4f::from_col_major
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

    
    vec4f
    mat4f::get_major() const
    {
        return {get(0, 0), get(1, 1), get(2, 2), get(3, 3)};
    }

    unit3f
    mat4f::get_axis(int col) const
    {
        return unit3f::to_unit(get_column(col).to_vec3());
    }

    unit3f
    mat4f::get_x_axis() const
    {
        return get_axis(0);
    }

    unit3f
    mat4f::get_y_axis() const
    {
        return get_axis(1);
    }

    unit3f
    mat4f::get_z_axis() const
    {
        return get_axis(2);
    }

    mat4f
    mat4f::get_transposed() const
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
    mat4f::invert()
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

    mat4f
    mat4f::get_inverted() const
    {
        auto r = *this;
        const auto was_inverted = r.invert();
        ASSERT(was_inverted);
        return r;
    }

    mat3f
    mat4f::get_mat3() const
    {
        return mat3f::from_row_major
        (
            get(0, 0), get(0, 1), get(0, 2),
            get(1, 0), get(1, 1), get(1, 2),
            get(2, 0), get(2, 1), get(2, 2)
        );
    }

    void
    mat4f::operator+=(const mat4f& rhs)
    {
#define OP(i) data[i] += rhs.data[i]
        OP(0); OP(1); OP(2); OP(3);
        OP(4); OP(5); OP(6); OP(7);
        OP(8); OP(9); OP(10); OP(11);
        OP(12); OP(13); OP(14); OP(15);
#undef OP
    }

    void
    mat4f::operator-=(const mat4f& rhs)
    {
#define OP(i) data[i] -= rhs.data[i]
        OP(0); OP(1); OP(2); OP(3);
        OP(4); OP(5); OP(6); OP(7);
        OP(8); OP(9); OP(10); OP(11);
        OP(12); OP(13); OP(14); OP(15);
#undef OP
    }

    [[nodiscard]]
    mat4f
    mat4f::create_ortho(float l, float r, float b, float t, float n, float f)
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
    mat4f
    mat4f::create_perspective(const Angle& fov, float a, float near, float far)
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

    mat4f
    mat4f::get_translated(const vec3f& t) const
    {
        return *this * from_translation(t);
    }

    mat4f
    mat4f::get_rotated(const AxisAngle& aa) const
    {
        return *this * from_axis_angle(aa);
    }

    mat4f
    mat4f::get_scaled(const Scale3f& scale) const
    {
        return *this * from_scale(scale);
    }

    const float*
    mat4f::get_data_ptr() const
    {
        return data;
    }

    float*
    mat4f::get_data_ptr()
    {
        return data;
    }


    float
    mat4f::get(int row, int col) const
    {
        return data[col * 4 + row];
    }

    vec4f
    mat4f::get_column(int col) const
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

    vec4f
    mat4f::get_row(int r) const
    {
        return {get(r, 0), get(r, 1), get(r, 2), get(r, 3)};
    }

    
    std::string to_string(const mat4f& m)
    {
        return fmt::format("({}, {}. {}. {})", m.get_row(0), m.get_row(1), m.get_row(2), m.get_row(3));
    }

    
    mat4f
    operator+(const mat4f& lhs, const mat4f& rhs)
    {
        mat4f t = lhs;
        t += rhs;
        return t;
    }

    
    mat4f
    operator-(const mat4f& lhs, const mat4f& rhs)
    {
        mat4f t = lhs;
        t -= rhs;
        return t;
    }

    float
    get_component_multiply_sum(const vec4f& lhs, const vec4f& rhs)
    {
        const auto x = lhs.x * rhs.x;
        const auto y = lhs.y * rhs.y;
        const auto z = lhs.z * rhs.z;
        const auto w  = lhs.w* rhs.w;
        return x + y + z + w;
    }
    
    mat4f operator*(const mat4f& lhs, const mat4f& rhs)
    {
        const auto op = [&lhs, &rhs](int r, int c) -> float
        {
            return get_component_multiply_sum(lhs.get_row(r), rhs.get_column(c));
        };
        return mat4f::from_row_major
        (
            op(0, 0), op(0, 1), op(0, 2), op(0, 3),
            op(1, 0), op(1, 1), op(1, 2), op(1, 3),
            op(2, 0), op(2, 1), op(2, 2), op(2, 3),
            op(3, 0), op(3, 1), op(3, 2), op(3, 3)
        );
    }

    
    vec4f operator*(const mat4f& lhs, const vec4f& rhs)
    {
        const auto op = [&lhs, &rhs](int r) -> float
        {
            return get_component_multiply_sum(lhs.get_row(r), rhs);
        };

        return {op(0), op(1), op(2), op(3)};
    }
}

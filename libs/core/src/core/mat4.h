#ifndef CORE_MAT4_H
#define CORE_MAT4_H

#include "core/vec3.h"
#include "core/vec4.h"
#include "core/angle.h"
#include "core/axisangle.h"
#include "core/mat3.h"

#include <vector>

namespace euphoria::core
{
    template <typename T>
    struct mat4
    {
    private:
        T data[16];  // col-major

        mat4() = default;

        mat4
        (
            T t00, T t01, T t02, T t03,
            T t10, T t11, T t12, T t13,
            T t20, T t21, T t22, T t23,
            T t30, T t31, T t32, T t33
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

    public:
        [[nodiscard]]
        static
        mat4<T>
        from_col_major
        (
            T t00, T t01, T t02, T t03,
            T t10, T t11, T t12, T t13,
            T t20, T t21, T t22, T t23,
            T t30, T t31, T t32, T t33
        )
        {
            return mat4<T>
            (
                t00, t01, t02, t03,
                t10, t11, t12, t13,
                t20, t21, t22, t23,
                t30, t31, t32, t33
            );
        }

        [[nodiscard]]
        static
        mat4<T>
        from_row_major
        (
            T t00, T t10, T t20, T t30,
            T t01, T t11, T t21, T t31,
            T t02, T t12, T t22, T t32,
            T t03, T t13, T t23, T t33
        )
        {
            return mat4<T>
            (
                t00, t01, t02, t03,
                t10, t11, t12, t13,
                t20, t21, t22, t23,
                t30, t31, t32, t33
            );
        }

        [[nodiscard]]
        static
        mat4<T>
        from_major(const vec4<T>& major)
        {
            const T zero = 0;
            return from_row_major
            (
                major.x, zero, zero, zero,
                zero, major.y, zero, zero,
                zero, zero, major.z, zero,
                zero, zero, zero, major.w
            );
        }

        [[nodiscard]]
        static
        mat4<T>
        from_scale(const scale3<T>& scale)
        {
            return from_major(vec4<T>(scale));
        }

        [[nodiscard]]
        static
        mat4<T>
        from_translation(const vec3<T>& v)
        {
            const T one = 1;
            const T z   = 0;
            return from_row_major
            (
                one, z, z, v.x,
                z, one, z, v.y,
                z, z, one, v.z,
                z, z, z, one
            );
        }

        vec4<T>
        get_transform(const vec4<T>& p) const
        {
            return *this * vec4<T>(p);
        }

        vec3<T>
        get_transform(const vec3<T>& p, T w) const
        {
            return get_transform(vec4<T>(p, w)).to_vec3(w);
        }

        vec3<T>
        get_transform_point(const vec3<T>& p) const
        {
            return get_transform(p, 1);
        }

        vec3<T>
        get_transform_vec(const vec3<T>& p) const
        {
            return get_transform(p, 0);
        }

        vec3<T>
        get_translation() const
        {
            return vec3<T>(get(0, 3), get(1, 3), get(2, 3));
        }

        [[nodiscard]] static mat4<T>
        from_scalar(T scalar)
        {
            const T z = 0;
            return from_row_major
            (
                scalar, z, z, z,
                z, scalar, z, z,
                z, z, scalar, z,
                z, z, z, scalar
            );
        }

        [[nodiscard]]
        static
        mat4<T>
        from_rot_x(const angle& a)
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

        [[nodiscard]]
        static
        mat4<T>
        from_rot_y(const angle& a);

        [[nodiscard]]
        static
        mat4<T>
        from_rot_z(const angle& a)
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
        static
        mat4<T>
        from_axis_angle(const axis_angle aa)
        {
            const T rcos = cos(aa.angle);
            const T rsin = sin(aa.angle);

            const auto u = aa.axis.x;
            const auto v = aa.axis.y;
            const auto w = aa.axis.z;

            return mat4<T>::from_col_major
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

        [[nodiscard]]
        static
        mat4<T>
        identity()
        {
            return from_scalar(1);
        }

        vec4<T>
        get_major() const
        {
            const mat4<T>& self = *this;
            return vec4<T>(self(0, 0), self(1, 1), self(2, 2), self(3, 3));
        }

        vec3<T>
        get_axis(int col) const
        {
            return get_column(col).to_vec3();
        }

        vec3<T>
        get_x_axis() const
        {
            return get_axis(0);
        }

        vec3<T>
        get_y_axis() const
        {
            return get_axis(1);
        }

        vec3<T>
        get_z_axis() const
        {
            return get_axis(2);
        }

        mat4<T>
        get_transposed() const
        {
            // https://www.j3d.org/matrix_faq/matrfaq_latest.html
            const mat4<T>& self = *this;
            return from_col_major
            (
                self(0, 0), self(0, 1), self(0, 2), self(0, 3),
                self(1, 0), self(1, 1), self(1, 2), self(1, 3),
                self(2, 0), self(2, 1), self(2, 2), self(2, 3),
                self(3, 0), self(3, 1), self(3, 2), self(3, 3)
            );
        }

        bool
        invert()
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

            for(int i = 0; i < 16; i++)
            {
                data[i] = inv[i] * det;
            }

            return true;
        }

        mat3<T>
        get_mat3() const
        {
            const mat4<T>& m = *this;
            return mat3<T>::from_row_major
            (
                m(0, 0), m(0, 1), m(0, 2),
                m(1, 0), m(1, 1), m(1, 2),
                m(2, 0), m(2, 1), m(2, 2)
            );
        }

        void
        operator+=(const mat4<T> rhs)
        {
#define OP(i) data[i] += rhs.data[i]
            OP(0); OP(1); OP(2); OP(3);
            OP(4); OP(5); OP(6); OP(7);
            OP(8); OP(9); OP(10); OP(11);
            OP(12); OP(13); OP(14); OP(15);
#undef OP
        }

        void
        operator-=(const mat4<T> rhs)
        {
#define OP(i) data[i] -= rhs.data[i]
            OP(0); OP(1); OP(2); OP(3);
            OP(4); OP(5); OP(6); OP(7);
            OP(8); OP(9); OP(10); OP(11);
            OP(12); OP(13); OP(14); OP(15);
#undef OP
        }

        [[nodiscard]]
        static
        mat4<T>
        create_ortho(T l, T r, T b, T t, T n, T f)
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
        static
        mat4<T>
        create_perspective(const angle& fov, T a, T near, T far)
        {
            const T t  = 1 / tan(fov / 2);
            const T zm = far - near;
            const T zp = far + near;
            return from_row_major
            (
                t / a, 0, 0, 0,
                0, t, 0, 0,
                0, 0, -zp / zm, (-2 * far * near) / zm,
                0, 0, -1, 0
            );
        }

        mat4<T>
        translate(const vec3<T>& t) const
        {
            return *this * from_translation(t);
        }

        mat4<T>
        rotate(const axis_angle& aa) const
        {
            return *this * from_axis_angle(aa);
        }

        mat4<T>
        scale(const scale3<T>& scale) const
        {
            return *this * from_scale(scale);
        }

        const T*
        get_data_ptr() const
        {
            return data;
        }

        T*
        get_data_ptr()
        {
            return data;
        }

        // index operator use () as [] only expects one argument
        T&
        operator()(int row, int col)
        {
            return data[col * 4 + row];
        }

        T
        operator()(int row, int col) const
        {
            return data[col * 4 + row];
        }

        T
        get(int row, int col) const
        {
            return data[col * 4 + row];
        }

        vec4<T>
        get_column(int c) const
        {
            return vec4<T>(&data[c * 4]);
        }

        vec4<T>
        get_row(int r) const
        {
            const mat4<T>& self = *this;
            return vec4<T>(self(r, 0), self(r, 1), self(r, 2), self(r, 3));
        }
    };

    template <typename T>
    mat4<T> mat4<T>::from_rot_y(const angle& a)
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

    template <typename T>
    bool
    operator==(const mat4<T>& lhs, const mat4<T>& rhs)
    {
        return
            lhs.get_column(0) == rhs.get_column(0) &&
            lhs.get_column(1) == rhs.get_column(1) &&
            lhs.get_column(2) == rhs.get_column(2) &&
            lhs.get_column(3) == rhs.get_column(3)
            ;
    }

    template <typename T>
    std::ostream&
    operator<<(std::ostream& stream, const mat4<T>& m)
    {
        return stream
            << "("
            << m.get_row(0) << ", "
            << m.get_row(1) << ", "
            << m.get_row(2) << ", "
            << m.get_row(3)
            << ")";
    }

    template<typename T>
    mat4<T>
    operator+(const mat4<T>& lhs, const mat4<T> rhs)
    {
        mat4<T> t = lhs;
        t += rhs;
        return t;
    }

    template<typename T>
    mat4<T>
    operator-(const mat4<T>& lhs, const mat4<T> rhs)
    {
        mat4<T> t = lhs;
        t -= rhs;
        return t;
    }

    template <typename T>
    mat4<T> operator*(const mat4<T>& lhs, const mat4<T> rhs)
    {
        const auto OP = [&lhs, &rhs](int r, int c) -> T
        {
            return component_multiply(lhs.get_row(r), rhs.get_column(c)).get_component_sum();
        };
        return mat4<T>::from_row_major
        (
            OP(0, 0), OP(0, 1), OP(0, 2), OP(0, 3),
            OP(1, 0), OP(1, 1), OP(1, 2), OP(1, 3),
            OP(2, 0), OP(2, 1), OP(2, 2), OP(2, 3),
            OP(3, 0), OP(3, 1), OP(3, 2), OP(3, 3)
        );
    }

    template <typename T>
    vec4<T> operator*(const mat4<T>& lhs, const vec4<T> rhs)
    {
        const auto OP = [&lhs, &rhs](int r) -> T
        {
            return component_multiply(lhs.get_row(r), rhs).get_component_sum();
        };

        return vec4<T>(OP(0), OP(1), OP(2), OP(3));
    }

    using mat4f = mat4<float>;
    using mat4i = mat4<int>;

}  // namespace euphoria::core

#endif  // CORE_MAT4_H


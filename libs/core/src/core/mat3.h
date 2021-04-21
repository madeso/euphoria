#ifndef CORE_MAT3_H
#define CORE_MAT3_H

#include <vector>
#include <tuple>

#include "core/vec2.h"
#include "core/vec3.h"
#include "core/mat2.h"
#include "core/angle.h"
#include "core/axisangle.h"

namespace euphoria::core
{
    template <typename T>
    struct mat3
    {
    private:
        using tuple3 = std::tuple<T, T, T>;

        T data[9];  // col-major

        mat3() = default;

        mat3
        (
            T t00, T t01, T t02,
            T t10, T t11, T t12,
            T t20, T t21, T t22
        )
            : data
            {
                t00, t01, t02,
                t10, t11, t12,
                t20, t21, t22
            }
        {
        }

    public:
        explicit
        mat3(const mat2<T>& mat)
            : data
            {
                mat(0,0), mat(0,1), 0,
                mat(1,0), mat(1,1), 0,
                0,        0,        1
            }
        {}


        [[nodiscard]]
        static
        mat3<T>
        from_col_major
        (
            T t00, T t01, T t02,
            T t10, T t11, T t12,
            T t20, T t21, T t22
        )
        {
            return mat3<T>
            (
                t00, t01, t02,
                t10, t11, t12,
                t20, t21, t22
            );
        }

        [[nodiscard]]
        static
        mat3<T>
        from_row_major
        (
            T t00, T t10, T t20,
            T t01, T t11, T t21,
            T t02, T t12, T t22
        )
        {
            return mat3<T>
            (
                t00, t01, t02,
                t10, t11, t12,
                t20, t21, t22
            );
        }

        [[nodiscard]]
        static
        mat3<T>
        from_major(const scale3<T>& major)
        {
            const T zero = 0;
            return from_row_major
            (
                major.x, zero, zero,
                zero, major.y, zero,
                zero, zero, major.z
            );
        }

        [[nodiscard]]
        static
        mat3<T>
        from_scale(const scale3<T>& scale)
        {
            return from_major(scale);
        }

        [[nodiscard]]
        static
        mat3<T>
        from_translation2d(const vec2<T>& t)
        {
            return from_row_major
            (
                1, 0, t.x,
                0, 1, t.y,
                0, 0, 1
            );
        }

        [[nodiscard]]
        static
        mat3<T>
        from_scalar(T scalar)
        {
            const T z = 0;
            return from_row_major
            (
                scalar, z, z,
                z, scalar, z,
                z, z, scalar
            );
        }

        [[nodiscard]]
        static
        mat3<T>
        from_rot_x(const angle& a)
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
        static
        mat3<T>
        from_rot_y(const angle& a)
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
        static
        mat3<T>
        from_rot_z(const angle& a)
        {
            const auto c = cos(a);
            const auto s = sin(a);
            return from_row_major(c, -s, 0, s, c, 0, 0, 0, 1);
        }

        [[nodiscard]]
        static
        mat3<T>
        from_axis_angle(const axis_angle aa)
        {
            const T rcos = cos(aa.angle);
            const T rsin = sin(aa.angle);

            const auto u = aa.axis.x;
            const auto v = aa.axis.y;
            const auto w = aa.axis.z;

            return mat3<T>::from_col_major
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

        [[nodiscard]]
        static
        mat3<T>
        identity()
        {
            return from_scalar(1);
        }

        tuple3
        get_major() const
        {
            const mat3<T>& self = *this;
            return tuple3(self(0, 0), self(1, 1), self(2, 2));
        }

        vec3<T>
        get_axis(int col) const
        {
            return static_cast<vec3<T>>(get_column(col));
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

        mat3<T>
        get_transposed() const
        {
            const mat3<T>& self = *this;
            return from_col_major
            (
                self(0, 0), self(0, 1), self(0, 2),
                self(1, 0), self(1, 1), self(1, 2),
                self(2, 0), self(2, 1), self(2, 2)
            );
        }

        void
        operator+=(const mat3<T> rhs)
        {
#define OP(i) data[i] += rhs.data[i]
            OP(0); OP(1); OP(2);
            OP(3); OP(4); OP(5);
            OP(6); OP(7); OP(8);
#undef OP
        }

        void
        operator-=(const mat3<T> rhs)
        {
#define OP(i) data[i] -= rhs.data[i]
            OP(0); OP(1); OP(2);
            OP(3); OP(4); OP(5);
            OP(6); OP(7); OP(8);
#undef OP
        }

        mat3<T>
        rotate(const axis_angle& aa) const
        {
            return *this * from_axis_angle(aa);
        }

        mat3<T>
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
            return data[col * 3 + row];
        }

        T
        operator()(int row, int col) const
        {
            return get(row, col);
        }

        T
        get(int row, int col) const
        {
            return data[col * 3 + row];
        }

        tuple3
        get_column(int c) const
        {
            return tuple3(get(0, c), get(1, c), get(2, c));
        }

        tuple3
        get_row(int r) const
        {
            return tuple3(get(r, 0), get(r, 1), get(r, 2));
        }
    };

    template <typename T>
    bool
    operator==(const mat3<T>& lhs, const mat3<T>& rhs)
    {
        return
            lhs.get_column(0) == rhs.get_column(0) &&
            lhs.get_column(1) == rhs.get_column(1) &&
            lhs.get_column(2) == rhs.get_column(2)
            ;
    }

    template <typename T>
    std::ostream&
    operator<<(std::ostream& stream, const mat3<T>& m)
    {
        return stream
            << "("
            << m.get_row(0) << ", " << m.get_row(1) << ", " << m.get_row(2)
            << ")"
            ;
    }

    template <typename T>
    mat3<T>
    operator+(const mat3<T>& lhs, const mat3<T> rhs)
    {
        mat3<T> t = lhs;
        t += rhs;
        return t;
    }

    template <typename T>
    mat3<T>
    operator-(const mat3<T>& lhs, const mat3<T> rhs)
    {
        mat3<T> t = lhs;
        t -= rhs;
        return t;
    }

    template <typename T>
    mat3<T> operator*(const mat3<T>& lhs, const mat3<T> rhs)
    {
#define OP(r, c)                                                               \
    component_multiply(vec3{lhs.get_row(r)}, vec3{rhs.get_column(c)}).get_component_sum()
        return mat3<T>::from_row_major
        (
            OP(0, 0), OP(0, 1), OP(0, 2),
            OP(1, 0), OP(1, 1), OP(1, 2),
            OP(2, 0), OP(2, 1), OP(2, 2)
        );
#undef OP
    }

    template <typename T>
    vec3<T> operator*(const mat3<T>& lhs, const vec3<T> rhs)
    {
#define OP(r) component_multiply(vec3{lhs.get_row(r)}, rhs).get_component_sum()
        return vec3<T>(OP(0), OP(1), OP(2));
#undef OP
    }

    using mat3f = mat3<float>;
    using mat3i = mat3<int>;

}  // namespace euphoria::core

#endif  // CORE_MAT3_H

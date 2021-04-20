#ifndef EUPHORIA_CORE_MAT2_H
#define EUPHORIA_CORE_MAT2_H

#include "core/vec2.h"
#include "core/angle.h"

#include <tuple>

namespace euphoria::core
{
    template<typename T>
    struct mat2
    {
    private:
        using tuple2 = std::tuple<T, T>;

        T data[4];

        mat2() = default;

        mat2
        (
            T t00, T t01,
            T t10, T t11
        )
            : data
            {
                t00, t01,
                t10, t11
            }
        {
        }

    public:

        [[nodiscard]]
        static
        mat2<T>
        from_col_major
        (
            T t00, T t01,
            T t10, T t11
        )
        {
            return mat2<T>(t00, t01, t10, t11);
        }

        [[nodiscard]]
        static
        mat2<T>
        from_row_major
        (
            T t00, T t10,
            T t01, T t11
        )
        {
            return mat2<T>
            (
                t00, t01,
                t10, t11
            );
        }

        [[nodiscard]]
        static
        mat2<T>
        from_scalar(T scalar)
        {
            const T z = 0;
            return from_row_major
            (
                scalar, z,
                z, scalar
            );
        }

        [[nodiscard]]
        static
        mat2<T>
        from_stretch_x(T k)
        {
            return from_row_major
            (
                k, 0,
                0, 1
            );
        }

        [[nodiscard]]
        static
        mat2<T>
        from_stretch_y(T k)
        {
            return from_row_major
            (
                1, 0,
                0, k
            );
        }

        [[nodiscard]]
        static
        mat2<T>
        from_rotation(const angle& a)
        {
            const auto s = sin(a);
            const auto c = cos(a);
            return from_row_major
            (
                c, s,
                -s, c
            );
        }

        [[nodiscard]]
        static
        mat2<T>
        identity()
        {
            return from_scalar(1);
        }


        void
        operator+=(const mat2<T> rhs)
        {
#define OP(i) data[i] += rhs.data[i]
            OP(0); OP(1);
            OP(2); OP(3);
#undef OP
        }

        void
        operator-=(const mat2<T> rhs)
        {
#define OP(i) data[i] -= rhs.data[i]
            OP(0); OP(1);
            OP(2); OP(3);
#undef OP
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
            return data[col * 2 + row];
        }

        T
        operator()(int row, int col) const
        {
            return get(row, col);
        }

        T
        get(int row, int col) const
        {
            return data[col * 2 + row];
        }

        tuple2
        get_column(int c) const
        {
            return tuple2(get(0, c), get(1, c));
        }

        tuple2
        get_row(int r) const
        {
            return tuple2(get(r, 0), get(r, 1));
        }
    };


    template <typename T>
    bool
    operator==(const mat2<T>& lhs, const mat2<T>& rhs)
    {
        return
            lhs.GetColumn(0) == rhs.GetColumn(0) &&
            lhs.GetColumn(1) == rhs.GetColumn(1) &&
            lhs.GetColumn(2) == rhs.GetColumn(2)
            ;
    }

    template <typename T>
    std::ostream&
    operator<<(std::ostream& stream, const mat2<T>& m)
    {
        return stream
            << "("
            << m.GetRow(0) << ", " << m.GetRow(1)
            << ")"
            ;
    }

    template <typename T>
    mat2<T>
    operator+(const mat2<T>& lhs, const mat2<T> rhs)
    {
        mat2<T> t = lhs;
        t += rhs;
        return t;
    }

    template <typename T>
    mat2<T>
    operator-(const mat2<T>& lhs, const mat2<T> rhs)
    {
        mat2<T> t = lhs;
        t -= rhs;
        return t;
    }

    template <typename T>
    mat2<T> operator*(const mat2<T>& lhs, const mat2<T> rhs)
    {
#define OP(r, c) \
    ComponentMultiply(vec2<T>(lhs.GetRow(r)), vec2<T>(rhs.GetColumn(c))).GetComponentSum()
        return mat2<T>::FromRowMajor
        (
            OP(0, 0), OP(0, 1),
            OP(1, 0), OP(1, 1)
        );
#undef OP
    }

    template <typename T>
    vec2<T> operator*(const mat2<T>& lhs, const vec2<T> rhs)
    {
#define OP(r) ComponentMultiply(vec2<T>(lhs.GetRow(r)), rhs).GetComponentSum()
        return vec2<T>(OP(0), OP(1));
#undef OP
    }

    using mat2f = mat2<float>;
    using mat2i = mat2<int>;
}

#endif // EUPHORIA_CORE_MAT2_H


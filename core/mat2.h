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

        mat2() {}

        mat2(T t00, T t01, T t10, T t11)
            : data {t00, t01, t10, t11}
        {}

    public:

        [[nodiscard]] static mat2<T>
        FromColMajor(
                T t00,
                T t01,
                T t10,
                T t11)
        {
            return mat2<T>(t00, t01, t10, t11);
        }

        [[nodiscard]] static mat2<T>
        FromRowMajor(
                T t00,
                T t10,
                T t01,
                T t11)
        {
            return mat2<T>(t00, t01, t10, t11);
        }

        [[nodiscard]] static mat2<T>
        FromScalar(T scalar)
        {
            const T z = 0;
            return FromRowMajor(scalar, z, z, scalar);
        }

        [[nodiscard]] static mat2<T>
        FromStretchX(T k)
        {
            return FromRowMajor(k, 0, 0, 1);
        }

        [[nodiscard]] static mat2<T>
        FromStretchY(T k)
        {
            return FromRowMajor(1, 0, 0, k);
        }

        [[nodiscard]] static mat2<T>
        FromRotation(const Angle& a)
        {
            const auto s = Sin(a);
            const auto c = Cos(a);
            return FromRowMajor(c, s, -s, c);
        }

        [[nodiscard]] static mat2<T>
        Identity()
        {
            return FromScalar(1);
        }


        void
        operator+=(const mat2<T> rhs)
        {
#define OP(i) data[i] += rhs.data[i]
            OP(0);
            OP(1);

            OP(2);
            OP(3);
#undef OP
        }

        void
        operator-=(const mat2<T> rhs)
        {
#define OP(i) data[i] -= rhs.data[i]
            OP(0);
            OP(1);

            OP(2);
            OP(3);
#undef OP
        }

        T
        Get(int row, int col) const
        {
            return data[col * 2 + row];
        }

        tuple2
        GetColumn(int c) const
        {
            return tuple2(Get(0, c), Get(1, c));
        }

        tuple2
        GetRow(int r) const
        {
            return tuple2(Get(r, 0), Get(r, 1));
        }
    };

    template <typename T>
    bool
    operator==(const mat2<T>& lhs, const mat2<T>& rhs)
    {
        return lhs.GetColumn(0) == rhs.GetColumn(0)
               && lhs.GetColumn(1) == rhs.GetColumn(1)
               && lhs.GetColumn(2) == rhs.GetColumn(2);
    }

    template <typename T>
    std::ostream&
    operator<<(std::ostream& stream, const mat2<T>& m)
    {
        return stream << "(" << m.GetRow(0) << ", " << m.GetRow(1) << ", "
                      << m.GetRow(2) << ")";
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
#define OP(r, c)                                                               \
    ComponentMultiply(vec2<T>(lhs.GetRow(r)), vec2<T>(rhs.GetColumn(c))).GetComponentSum()
        return mat2<T>::FromRowMajor(
                OP(0, 0),
                OP(0, 1),
                OP(1, 0),
                OP(1, 1));
#undef OP
    }

    template <typename T>
    vec2<T> operator*(const mat2<T>& lhs, const vec2<T> rhs)
    {
#define OP(r) ComponentMultiply(vec2<T>(lhs.GetRow(r)), rhs).GetComponentSum()
        return vec2<T>(OP(0), OP(1));
#undef OP
    }

    typedef mat2<float> mat2f;
    typedef mat2<int>   mat2i;
}

#endif // EUPHORIA_CORE_MAT2_H


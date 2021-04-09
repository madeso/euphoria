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
        FromColMajor
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
        FromRowMajor
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
        FromMajor(const scale3<T>& major)
        {
            const T zero = 0;
            return FromRowMajor
            (
                major.x, zero, zero,
                zero, major.y, zero,
                zero, zero, major.z
            );
        }

        [[nodiscard]]
        static
        mat3<T>
        FromScale(const scale3<T>& scale)
        {
            return FromMajor(scale);
        }

        [[nodiscard]]
        static
        mat3<T>
        FromTranslation2d(const vec2<T>& t)
        {
            return FromRowMajor
            (
                1, 0, t.x,
                0, 1, t.y,
                0, 0, 1
            );
        }

        [[nodiscard]]
        static
        mat3<T>
        FromScalar(T scalar)
        {
            const T z = 0;
            return FromRowMajor
            (
                scalar, z, z,
                z, scalar, z,
                z, z, scalar
            );
        }

        [[nodiscard]]
        static
        mat3<T>
        FromRotX(const Angle& a)
        {
            const auto c = Cos(a);
            const auto s = Sin(a);
            return FromRowMajor
            (
                1, 0, 0,
                0, c, -s,
                0, s, c
            );
        }

        [[nodiscard]]
        static
        mat3<T>
        FromRotY(const Angle& a)
        {
            const auto c = Cos(a);
            const auto s = Sin(a);
            return FromRowMajor
            (
                c, 0, s,
                0, 1, 0,
                -s, 0, c
            );
        }

        [[nodiscard]]
        static
        mat3<T>
        FromRotZ(const Angle& a)
        {
            const auto c = Cos(a);
            const auto s = Sin(a);
            return FromRowMajor(c, -s, 0, s, c, 0, 0, 0, 1);
        }

        [[nodiscard]]
        static
        mat3<T>
        FromAxisAngle(const AxisAngle aa)
        {
            const T rcos = Cos(aa.angle);
            const T rsin = Sin(aa.angle);

            const auto u = aa.axis.x;
            const auto v = aa.axis.y;
            const auto w = aa.axis.z;

            return mat3<T>::FromColMajor
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
        Identity()
        {
            return FromScalar(1);
        }

        tuple3
        GetMajor() const
        {
            const mat3<T>& self = *this;
            return tuple3(self(0, 0), self(1, 1), self(2, 2));
        }

        vec3<T>
        GetAxis(int col) const
        {
            return static_cast<vec3<T>>(GetColumn(col));
        }

        vec3<T>
        GetXAxis() const
        {
            return GetAxis(0);
        }

        vec3<T>
        GetYAxis() const
        {
            return GetAxis(1);
        }

        vec3<T>
        GetZAxis() const
        {
            return GetAxis(2);
        }

        mat3<T>
        GetTransposed() const
        {
            const mat3<T>& self = *this;
            return FromColMajor
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
        Rotate(const AxisAngle& aa) const
        {
            return *this * FromAxisAngle(aa);
        }

        mat3<T>
        Scale(const scale3<T>& scale) const
        {
            return *this * FromScale(scale);
        }

        const T*
        GetDataPtr() const
        {
            return data;
        }

        T*
        GetDataPtr()
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
            return Get(row, col);
        }

        T
        Get(int row, int col) const
        {
            return data[col * 3 + row];
        }

        tuple3
        GetColumn(int c) const
        {
            return tuple3(Get(0, c), Get(1, c), Get(2, c));
        }

        tuple3
        GetRow(int r) const
        {
            return tuple3(Get(r, 0), Get(r, 1), Get(r, 2));
        }
    };

    template <typename T>
    bool
    operator==(const mat3<T>& lhs, const mat3<T>& rhs)
    {
        return
            lhs.GetColumn(0) == rhs.GetColumn(0) &&
            lhs.GetColumn(1) == rhs.GetColumn(1) &&
            lhs.GetColumn(2) == rhs.GetColumn(2)
            ;
    }

    template <typename T>
    std::ostream&
    operator<<(std::ostream& stream, const mat3<T>& m)
    {
        return stream
            << "("
            << m.GetRow(0) << ", " << m.GetRow(1) << ", " << m.GetRow(2)
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
    ComponentMultiply(vec3{lhs.GetRow(r)}, vec3{rhs.GetColumn(c)}).GetComponentSum()
        return mat3<T>::FromRowMajor
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
#define OP(r) ComponentMultiply(vec3{lhs.GetRow(r)}, rhs).GetComponentSum()
        return vec3<T>(OP(0), OP(1), OP(2));
#undef OP
    }

    using mat3f = mat3<float>;
    using mat3i = mat3<int>;

}  // namespace euphoria::core

#endif  // CORE_MAT3_H

#pragma once

#include "core/vec3.h"
#include "core/numeric.h"
#include "core/axisangle.h"
#include "core/angle.h"
#include "core/mat4.h"
#include "core/polarcoord.h"

namespace euphoria::core
{
    template <typename T>
    struct quat
    {
        using Q = quat<T>;
        using Vec = vec3<T>;
        using Point = vec3<T>;
        using Unit = unit3<T>;

        T w;
        T x;
        T y;
        T z;

        Vec
        vec() const
        {
            return Vec(x, y, z);
        }


        quat(T w, const Vec& v) : w(w), x(v.x), y(v.y), z(v.z) {}


        [[nodiscard]] static Q
        FromAxisAngle(const axis_angle& aa)
        {
            const T sin_a = sin(aa.angle / 2);
            const T cos_a = cos(aa.angle / 2);
            Q r(cos_a, aa.axis * sin_a);
            r.Normalize();
            return r;
        }


        [[nodiscard]] static Q
        FromRandom(Random* random)
        {
            const auto axis = RandomUnit3(random);
            const auto angle = angle::Random(random);

            return Q::FromAxisAngle(axis_angle::right_hand_around(axis, angle));
        }


        mat4<T>
        ToMat4() const
        {
            return mat4<T>::from_axis_angle(ToAxisAngle());
        }


        [[nodiscard]] axis_angle
        ToAxisAngle() const
        {
            const T cos_a = w;
            const auto angle = acos(cos_a) * 2;
            const T sin_a = DefaultIfCloseToZero<T>(
                    Sqrt(1.0f - cos_a * cos_a), 1, 0.0005f);
            // todo(Gustav): do we need to normalize here?
            return axis_angle::right_hand_around(
                    (vec() / sin_a).GetNormalized(), angle);
        }


        // static Q FromAngles(T x, T y, T z);

        [[nodiscard]] static Q
        Identity()
        {
            return Q(1, Vec(0, 0, 0));
        }


        [[nodiscard]] static Q
        LookAt(const Point& from, const Point& to, const typename Vec::Unit up)
        {
            return LookInDirection(Vec::FromTo(from, to).GetNormalized(), up);
        }


        [[nodiscard]] static Q
        LookInDirection(const Unit& dir, const Unit& up)
        {
            const Vec in = Unit::In();
            float dot_value = dot(in, dir);

            if(Abs(dot_value - (-1.0f)) < 0.000001f)
            {
                return Q(3.1415926535897932f, up);
            }
            if(Abs(dot_value - (1.0f)) < 0.000001f)
            {
                return Identity();
            }

            const auto rotAngle = acos(dot_value);
            const auto rotAxis = cross(in, dir).GetNormalized();
            return Q::FromAxisAngle(
                    axis_angle::right_hand_around(rotAxis, rotAngle));
        }


        Q
        Rotate(const Q& q) const
        {
            return q * *this;
        }


        Q
        GetConjugate() const
        {
            return quat(w, -vec());
        }


        // the inverse represent the same rotation
        Q
        GetInverse() const
        {
            return quat(-w, -vec());
        }


        Q
        GetIdentity() const
        {
            const T l2 = GetLengthSquared();
            if(IsEqual(l2, 0)) { return Identity(); }
            else if(IsEqual(l2, 1)) { return GetConjugate(); }
            else { return quat(w / Sqrt(l2), -vec()); }
        }


        T
        GetLength() const
        {
            return Sqrt(GetLengthSquared());
        }


        T
        GetLengthSquared() const
        {
            return x * x + y * y + z * z + w * w;
        }


        void
        Normalize()
        {
            const T l = GetLength();
            if(IsZero(l))
            {
                *this = Identity();
            }
            else
            {
                x /= l;
                y /= l;
                z /= l;
                w /= l;
            }
        }


        Q
        GetNormalized() const
        {
            Q r = *this;
            r.Normalize();
            return r;
        }


        Unit
        In() const
        {
            return RotateAroundOrigo(-Unit::ZAxis());
        }


        Unit
        Out() const
        {
            return RotateAroundOrigo(Unit::ZAxis());
        }


        Unit
        Right() const
        {
            return RotateAroundOrigo(Unit::XAxis());
        }


        Unit
        Left() const
        {
            return RotateAroundOrigo(-Unit::XAxis());
        }


        Unit
        Up() const
        {
            return RotateAroundOrigo(Unit::YAxis());
        }


        Unit
        Down() const
        {
            return RotateAroundOrigo(-Unit::YAxis());
        }


        // In*Z + Right*X + Up*Y
        Vec
        RightUpIn(const Vec v) const
        {
            return In() * v.z + Right() * v.x + Up() * v.y;
        }


        Unit
        RotateAroundOrigo(const Unit& v) const
        {
            // http://gamedev.stackexchange.com/questions/28395/rotating-vector3-by-a-quaternion
            const Q pure(0, v);
            const Q a = *this * pure;
            const Q ret = a * GetConjugate();
            return ret.vec().GetNormalized();
        }


        [[nodiscard]] static Q
        Lerp(const Q& f, const T scale, const Q& t)
        {
            return f * (1 - scale) + t * scale;
        }


        [[nodiscard]] static Q
        Slerp(const Q& qa, const float t, const Q& qb)
        {
            // from:
            // http://www.euclideanspace.com/maths/algebra/realNormedAlgebra/quaternions/slerp/
            // Calculate angle between them.
            const float cosHalfTheta = qa.w * qb.w + qa.x * qb.x + qa.y * qb.y + qa.z * qb.z;
            // if qa=qb or qa=-qb then theta = 0 and we can return qa
            if(Abs(cosHalfTheta) >= 1.0)
            {
                return qa;
            }
            // Calculate temporary values.
            const auto halfTheta = acos(cosHalfTheta);
            const auto sinHalfTheta = sqrt(1.0f - cosHalfTheta * cosHalfTheta);
            if(Abs(sinHalfTheta) < 0.001f)
            {
                // if theta = 180 degrees then result is not fully defined
                // we could rotate around any axis normal to qa or qb
                const Q qt = qa + qb;
                return Q
                {
                    qt.w * 0.5f,
                    Vec
                    {
                        qt.x * 0.5f,
                        qt.y * 0.5f,
                        qt.z * 0.5f
                    }
                };
            }
            const float ratioA = sin((1 - t) * halfTheta) / sinHalfTheta;
            const float ratioB = sin(t * halfTheta) / sinHalfTheta;
            return qa * ratioA + qb * ratioB;
        }


        [[nodiscard]] static Q
        SlerpShortway(const Q& f, const T scale, const Q& t)
        {
            if(dot(f, t) < 0)
            {
                return Slerp(f.GetInverse(), scale, t);
            }
            else
            {
                return Slerp(f, scale, t);
            }
        }


        void
        operator+=(const Q& rhs)
        {
            x += rhs.x;
            y += rhs.y;
            z += rhs.z;
            w += rhs.w;
        }


        void
        operator-=(const Q& rhs)
        {
            x -= rhs.x;
            y -= rhs.y;
            z -= rhs.z;
            w -= rhs.w;
        }


        void
        operator*=(const T& rhs)
        {
            x *= rhs;
            y *= rhs;
            z *= rhs;
            w *= rhs;
        }


        void
        operator*=(const Q& rhs)
        {
#define VAR(a, b) const T a##1##b##2 = a * rhs.b
            VAR(w, w);
            VAR(w, x);
            VAR(w, y);
            VAR(w, z);

            VAR(x, w);
            VAR(x, x);
            VAR(x, y);
            VAR(x, z);

            VAR(y, w);
            VAR(y, x);
            VAR(y, y);
            VAR(y, z);

            VAR(z, w);
            VAR(z, x);
            VAR(z, y);
            VAR(z, z);
#undef VAR

            w = w1w2 - x1x2 - y1y2 - z1z2;
            x = w1x2 + x1w2 + y1z2 - z1y2;
            y = w1y2 + y1w2 + z1x2 - x1z2;
            z = w1z2 + z1w2 + x1y2 - y1x2;
        }
    };


    template <typename T>
    std::ostream&
    operator<<(std::ostream& stream, const quat<T>& v)
    {
        return stream << "(" << v.w << " (" << v.x << ", " << v.y << ", " << v.z
                      << "))";
    }


    template <typename T>
    T
    dot(const quat<T>& lhs, const quat<T>& rhs)
    {
        return lhs.x * rhs.x + lhs.y * rhs.y + lhs.z * rhs.z + lhs.w * rhs.w;
    }


    template <typename T>
    quat<T> operator*(const quat<T>& lhs, const quat<T>& rhs)
    {
        quat<T> r = lhs;
        r *= rhs;
        return r;
    }


    template <typename T>
    quat<T> operator*(T scale, const quat<T>& q)
    {
        quat<T> r = q;
        r *= scale;
        return r;
    }


    template <typename T>
    quat<T> operator*(const quat<T>& q, T scale)
    {
        quat<T> r = q;
        r *= scale;
        return r;
    }


    template <typename T>
    quat<T>
    operator+(const quat<T>& lhs, const quat<T>& rhs)
    {
        quat<T> r = rhs;
        r += lhs;
        return r;
    }


    template <typename T>
    quat<T>
    operator-(const quat<T>& lhs, const quat<T>& rhs)
    {
        quat<T> r = rhs;
        r -= lhs;
        return r;
    }


    template <typename T>
    bool
    operator==(const quat<T>& lhs, const quat<T>& rhs)
    {
        return lhs.x == rhs.x && lhs.y == rhs.y && lhs.z == rhs.z
               && lhs.w == rhs.w;
    }


    using quatf = quat<float>;
    using quati = quat<int>;
}


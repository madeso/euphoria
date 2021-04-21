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
        using self = quat<T>;
        using vec = vec3<T>;
        using point = vec3<T>;
        using unit = unit3<T>;

        T w;
        T x;
        T y;
        T z;

        vec
        get_vec_part() const
        {
            return vec(x, y, z);
        }


        quat(T w, const vec& v) : w(w), x(v.x), y(v.y), z(v.z) {}


        [[nodiscard]] static self
        from_axis_angle(const axis_angle& aa)
        {
            const T sin_a = sin(aa.angle / 2);
            const T cos_a = cos(aa.angle / 2);
            self r(cos_a, aa.axis * sin_a);
            r.normalize();
            return r;
        }


        [[nodiscard]] static self
        from_random(Random* random)
        {
            const auto axis = create_random_unit3(random);
            const auto angle = angle::Random(random);

            return self::from_axis_angle(axis_angle::right_hand_around(axis, angle));
        }


        [[nodiscard]] mat4<T>
        to_mat4() const
        {
            return mat4<T>::from_axis_angle(to_axis_angle());
        }


        [[nodiscard]] axis_angle
        to_axis_angle() const
        {
            const T cos_a = w;
            const auto angle = acos(cos_a) * 2;
            const T sin_a = DefaultIfCloseToZero<T>(
                    Sqrt(1.0f - cos_a * cos_a), 1, 0.0005f);
            // todo(Gustav): do we need to normalize here?
            return axis_angle::right_hand_around(
                    (get_vec_part() / sin_a).get_normalized(), angle);
        }


        // static Q FromAngles(T x, T y, T z);

        [[nodiscard]] static self
        identity()
        {
            return self(1, vec(0, 0, 0));
        }


        [[nodiscard]] static self
        look_at(const point& from, const point& to, const typename vec::unit up)
        {
            return look_in_direction(vec::from_to(from, to).get_normalized(), up);
        }


        [[nodiscard]] static self
        look_in_direction(const unit& dir, const unit& up);


        self
        rotate(const self& q) const
        {
            return q * *this;
        }


        self
        get_conjugate() const
        {
            return quat(w, -get_vec_part());
        }


        // the inverse represent the same rotation
        self
        get_inverse() const
        {
            return quat(-w, -get_vec_part());
        }


        self
        get_identity() const
        {
            const T l2 = get_length_squared();
            if(IsEqual(l2, 0)) { return identity(); }
            else if(IsEqual(l2, 1)) { return get_conjugate(); }
            else { return quat(w / Sqrt(l2), -get_vec_part()); }
        }


        T
        get_length() const
        {
            return Sqrt(get_length_squared());
        }


        T
        get_length_squared() const
        {
            return x * x + y * y + z * z + w * w;
        }


        void
        normalize()
        {
            const T l = get_length();
            if(IsZero(l))
            {
                *this = identity();
            }
            else
            {
                x /= l;
                y /= l;
                z /= l;
                w /= l;
            }
        }


        self
        get_normalized() const
        {
            self r = *this;
            r.normalize();
            return r;
        }


        unit
        in() const
        {
            return rotate_around_origo(-unit::z_axis());
        }


        unit
        out() const
        {
            return rotate_around_origo(unit::z_axis());
        }


        unit
        right() const
        {
            return rotate_around_origo(unit::x_axis());
        }


        unit
        left() const
        {
            return rotate_around_origo(-unit::x_axis());
        }


        unit
        up() const
        {
            return rotate_around_origo(unit::y_axis());
        }


        unit
        down() const
        {
            return rotate_around_origo(-unit::y_axis());
        }


        // In*Z + Right*X + Up*Y
        vec
        create_from_right_up_in(const vec v) const
        {
            return in() * v.z + right() * v.x + up() * v.y;
        }


        unit
        rotate_around_origo(const unit& v) const
        {
            // http://gamedev.stackexchange.com/questions/28395/rotating-vector3-by-a-quaternion
            const self pure(0, v);
            const self a = *this * pure;
            const self ret = a * get_conjugate();
            return ret.get_vec_part().get_normalized();
        }


        [[nodiscard]] static self
        lerp(const self& f, const T scale, const self& t)
        {
            return f * (1 - scale) + t * scale;
        }


        [[nodiscard]] static self
        slerp(const self& qa, const float t, const self& qb)
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
                const self qt = qa + qb;
                return self
                {
                    qt.w * 0.5f,
                    vec
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


        [[nodiscard]] static self
        slerp_shortway(const self& f, const T scale, const self& t)
        {
            if(dot(f, t) < 0)
            {
                return slerp(f.get_inverse(), scale, t);
            }
            else
            {
                return slerp(f, scale, t);
            }
        }


        void
        operator+=(const self& rhs)
        {
            x += rhs.x;
            y += rhs.y;
            z += rhs.z;
            w += rhs.w;
        }


        void
        operator-=(const self& rhs)
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
        operator*=(const self& rhs)
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
    typename quat<T>::self quat<T>::look_in_direction(const unit& dir, const unit& up)
    {
        const vec in = unit::in();
        float dot_value = dot(in, dir);

        if (Abs(dot_value - (-1.0f)) < 0.000001f)
        {
            return self(3.1415926535897932f, up);
        }
        if (Abs(dot_value - (1.0f)) < 0.000001f)
        {
            return identity();
        }

        const auto rotAngle = acos(dot_value);
        const auto rotAxis = cross(in, dir).get_normalized();
        return self::from_axis_angle(
            axis_angle::right_hand_around(rotAxis, rotAngle));
    }


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


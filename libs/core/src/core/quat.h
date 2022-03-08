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
    struct Quat
    {
        using Self = Quat<T>;
        using Vec = Vec3<T>;
        using Point = Vec3<T>;
        using Unit = Unit3<T>;

        T w;
        T x;
        T y;
        T z;

        Vec
        get_vec_part() const
        {
            return Vec(x, y, z);
        }


        Quat(T aw, const Vec& v) : w(aw), x(v.x), y(v.y), z(v.z) {}


        [[nodiscard]] static Self
        from_axis_angle(const AxisAngle& aa)
        {
            const T sin_a = sin(aa.angle / 2);
            const T cos_a = cos(aa.angle / 2);
            Self r(cos_a, aa.axis * sin_a);
            r.normalize();
            return r;
        }


        [[nodiscard]] static Self
        from_ypr(const Angle& yaw, const Angle& pitch, const Angle& roll)
        {
            // Abbreviations for the various angular functions
            const auto cy = cos(yaw * 0.5);
            const auto sy = sin(yaw * 0.5);
            const auto cp = cos(pitch * 0.5);
            const auto sp = sin(pitch * 0.5);
            const auto cr = cos(roll * 0.5);
            const auto sr = sin(roll * 0.5);

            return
            {
                cr * cp * cy + sr * sp * sy,
                {
                    sr * cp * cy - cr * sp * sy,
                    cr * sp * cy + sr * cp * sy,
                    cr * cp * sy - sr * sp * cy
                }
            };
        }


        [[nodiscard]] static Self
        from_random(Random* random)
        {
            const auto axis = get_random_unit3(random);
            const auto angle = Angle::random(random);

            return Self::from_axis_angle(AxisAngle::right_hand_around(axis, angle));
        }


        [[nodiscard]] Mat4<T>
        to_mat4() const
        {
            return Mat4<T>::from_axis_angle(to_axis_angle());
        }


        [[nodiscard]] AxisAngle
        to_axis_angle() const
        {
            const T cos_a = w;
            const auto angle = acos(cos_a) * 2;
            const T sin_a = get_default_if_close_to_zero<T>(
                    sqrt(1.0f - cos_a * cos_a), 1, 0.0005f);
            // todo(Gustav): do we need to normalize here?
            return AxisAngle::right_hand_around(
                    (get_vec_part() / sin_a).get_normalized(), angle);
        }


        // static Q FromAngles(T x, T y, T z);

        [[nodiscard]] static Self
        identity()
        {
            return Self(1, Vec(0, 0, 0));
        }


        [[nodiscard]] static Self
        look_at(const Point& from, const Point& to, const typename Vec::Unit up)
        {
            return look_in_direction(Vec::from_to(from, to).get_normalized(), up);
        }


        [[nodiscard]] static Self
        look_in_direction(const Unit& dir, const Unit& up);


        Self
        rotate(const Self& q) const
        {
            return q * *this;
        }


        Self
        get_conjugate() const
        {
            return Quat(w, -get_vec_part());
        }


        // the inverse represent the same rotation
        Self
        get_inverse() const
        {
            return Quat(-w, -get_vec_part());
        }


        Self
        get_identity() const
        {
            const T l2 = get_length_squared();
            if(is_equal(l2, 0)) { return identity(); }
            else if(is_equal(l2, 1)) { return get_conjugate(); }
            else { return Quat(w / sqrt(l2), -get_vec_part()); }
        }


        T
        get_length() const
        {
            return sqrt(get_length_squared());
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
            if(is_zero(l))
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


        Self
        get_normalized() const
        {
            Self r = *this;
            r.normalize();
            return r;
        }


        Unit
        in() const
        {
            return rotate_around_origo(-Unit::z_axis());
        }


        Unit
        out() const
        {
            return rotate_around_origo(Unit::z_axis());
        }


        Unit
        right() const
        {
            return rotate_around_origo(Unit::x_axis());
        }


        Unit
        left() const
        {
            return rotate_around_origo(-Unit::x_axis());
        }


        Unit
        up() const
        {
            return rotate_around_origo(Unit::y_axis());
        }


        Unit
        down() const
        {
            return rotate_around_origo(-Unit::y_axis());
        }


        // In*Z + Right*X + Up*Y
        Vec
        create_from_right_up_in(const Vec v) const
        {
            return in() * v.z + right() * v.x + up() * v.y;
        }


        Unit
        rotate_around_origo(const Unit& v) const
        {
            // http://gamedev.stackexchange.com/questions/28395/rotating-vector3-by-a-quaternion
            const Self pure(0, v);
            const Self a = *this * pure;
            const Self ret = a * get_conjugate();
            return ret.get_vec_part().get_normalized();
        }


        [[nodiscard]] static Self
        lerp(const Self& f, const T scale, const Self& t)
        {
            return f * (1 - scale) + t * scale;
        }


        [[nodiscard]] static Self
        slerp(const Self& qa, const float t, const Self& qb)
        {
            // from:
            // http://www.euclideanspace.com/maths/algebra/realNormedAlgebra/quaternions/slerp/
            // Calculate angle between them.
            const float cos_half_theta = qa.w * qb.w + qa.x * qb.x + qa.y * qb.y + qa.z * qb.z;
            // if qa=qb or qa=-qb then theta = 0 and we can return qa
            if(abs(cos_half_theta) >= 1.0f)
            {
                return qa;
            }
            // Calculate temporary values.
            const auto half_theta = acos(cos_half_theta);
            const auto sin_half_theta = sqrt(1.0f - cos_half_theta * cos_half_theta);
            if(abs(sin_half_theta) < 0.001f)
            {
                // if theta = 180 degrees then result is not fully defined
                // we could rotate around any axis normal to qa or qb
                const Self qt = qa + qb;
                return Self
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
            const float ratio_a = sin((1 - t) * half_theta) / sin_half_theta;
            const float ratio_b = sin(t * half_theta) / sin_half_theta;
            return qa * ratio_a + qb * ratio_b;
        }


        [[nodiscard]] static Self
        slerp_shortway(const Self& f, const T scale, const Self& t)
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
        operator+=(const Self& rhs)
        {
            x += rhs.x;
            y += rhs.y;
            z += rhs.z;
            w += rhs.w;
        }


        void
        operator-=(const Self& rhs)
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
        operator*=(const Self& rhs)
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
    typename Quat<T>::Self Quat<T>::look_in_direction(const Unit& dir, const Unit& up)
    {
        const Vec in = Unit::in();
        float dot_value = dot(in, dir);

        if (abs(dot_value - (-1.0f)) < 0.000001f)
        {
            return Self(3.1415926535897932f, up);
        }
        if (abs(dot_value - (1.0f)) < 0.000001f)
        {
            return identity();
        }

        const auto rot_angle = acos(dot_value);
        const auto rot_axis = cross(in, dir).get_normalized();
        return Self::from_axis_angle
        (
            AxisAngle::right_hand_around(rot_axis, rot_angle)
        );
    }


    template <typename T>
    std::ostream&
    operator<<(std::ostream& stream, const Quat<T>& v)
    {
        return stream << "(" << v.w << " (" << v.x << ", " << v.y << ", " << v.z
                      << "))";
    }


    template <typename T>
    T
    dot(const Quat<T>& lhs, const Quat<T>& rhs)
    {
        return lhs.x * rhs.x + lhs.y * rhs.y + lhs.z * rhs.z + lhs.w * rhs.w;
    }


    template <typename T>
    Quat<T> operator*(const Quat<T>& lhs, const Quat<T>& rhs)
    {
        Quat<T> r = lhs;
        r *= rhs;
        return r;
    }


    template <typename T>
    Quat<T> operator*(T scale, const Quat<T>& q)
    {
        Quat<T> r = q;
        r *= scale;
        return r;
    }


    template <typename T>
    Quat<T> operator*(const Quat<T>& q, T scale)
    {
        Quat<T> r = q;
        r *= scale;
        return r;
    }


    template <typename T>
    Quat<T>
    operator+(const Quat<T>& lhs, const Quat<T>& rhs)
    {
        Quat<T> r = rhs;
        r += lhs;
        return r;
    }


    template <typename T>
    Quat<T>
    operator-(const Quat<T>& lhs, const Quat<T>& rhs)
    {
        Quat<T> r = rhs;
        r -= lhs;
        return r;
    }


    template <typename T>
    bool
    operator==(const Quat<T>& lhs, const Quat<T>& rhs)
    {
        return lhs.x == rhs.x && lhs.y == rhs.y && lhs.z == rhs.z
               && lhs.w == rhs.w;
    }


    using Quatf = Quat<float>;
    using Quati = Quat<int>;
}


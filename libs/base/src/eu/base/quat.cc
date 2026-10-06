#include "eu/base/quat.h"

#include <algorithm>
#include <cmath>

namespace
{
    template <typename T, size_t array_size>
    size_t argmax(const std::array<T, array_size>& array)
    {
        ASSERT(array.empty() == false);
        return std::distance(array.begin(), std::max_element(array.begin(), array.end()));
    }
}

namespace eu
{
    v3 Q::get_vec_part() const
    {
        return {x, y, z};
    }


    [[nodiscard]] Q
    Q::from(const AA& aa)
    {
        const float sin_a = sin(aa.angle / 2);
        const float cos_a = cos(aa.angle / 2);
        Q r(cos_a, aa.axis * sin_a);
        r.normalize();
        return r;
    }


    [[nodiscard]] Q
    Q::from(const Ypr& ypr)
    {
        const auto yaw = Q::from(AA{kk::y_axis, -ypr.yaw});
        const auto pitch = Q::from(AA{ kk::x_axis, -ypr.pitch});
        const auto yp = pitch.then_get_rotated(yaw);
        const auto roll = Q::from(AA{ yp.get_local_out(), ypr.roll });
        return yp.then_get_rotated(roll);
    }

    [[nodiscard]] Q
    Q::from_fast(const Ypr& ypr)
    {
        // Abbreviations for the various angular functions
        const auto cy = cos(ypr.yaw * 0.5);
        const auto sy = sin(ypr.yaw * 0.5);
        const auto cp = cos(ypr.pitch * 0.5);
        const auto sp = sin(ypr.pitch * 0.5);
        const auto cr = cos(ypr.roll * 0.5);
        const auto sr = sin(ypr.roll * 0.5);

        return
        {
            cy * cp * cr + sy * sp * sr,
            {
                -(cy * sp * cr + sy * cp * sr),
                cy * sp * sr - sy * cp * cr,
                cy * cp * sr - sy * sp * cr
            }
        };
    }


    [[nodiscard]] Q
    Q::from_to(const Q& from, const Q& to)
    {
        // https://stackoverflow.com/a/22167097
        return to * from.get_inverse();
    }


    [[nodiscard]] std::optional<Q>
    Q::look_at(const v3& from, const v3& to, const n3& up)
    {
        const auto direction = v3::from_to(from, to).get_normalized();
        if (direction.has_value() == false)
            { return std::nullopt; }
        return look_in_direction(*direction, up);
    }


    Q
    Q::then_get_rotated(const Q& q) const
    {
        return q * *this;
    }


    Q
    Q::get_conjugate() const
    {
        return {w, -get_vec_part()};
    }


    Q
    Q::get_inverse() const
    {
        ASSERT(is_equal(get_length(), 1.0f));
        return get_conjugate();
    }


    // the negated represents the same rotation
    Q
    Q::get_negated() const
    {
        return {-w, -get_vec_part()};
    }

    float
    Q::get_length() const
    {
        const auto l2 = x * x + y * y + z * z + w * w;
        return std::sqrt(l2);
    }


    void
    Q::normalize()
    {
        const float l = get_length();
        if(is_zero(l))
        {
            *this = q_identity;
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
    Q::get_normalized() const
    {
        Q r = *this;
        r.normalize();
        return r;
    }


    n3 Q::get_local_in   () const { return get_rotated(-kk::z_axis); }
    n3 Q::get_local_out  () const { return get_rotated( kk::z_axis); }
    n3 Q::get_local_right() const { return get_rotated( kk::x_axis); }
    n3 Q::get_local_left () const { return get_rotated(-kk::x_axis); }
    n3 Q::get_local_up   () const { return get_rotated( kk::y_axis); }
    n3 Q::get_local_down () const { return get_rotated(-kk::y_axis); }


    n3
    Q::get_rotated(const n3& v) const
    {
        // http://gamedev.stackexchange.com/questions/28395/rotating-vector3-by-a-quaternion
        const Q pure = {0, v};
        const Q a = *this * pure;
        const Q ret = a * get_conjugate();
        // todo(Gustav): should we normalize here? Can we get a invalid vector?
        const auto normalized = ret.get_vec_part().get_normalized();
        if(normalized.has_value() == false)
        {
            DIE("invalid rotation vector");
            return kk::up;
        }

        return *normalized;
    }

    Q add(const Q& lhs, const Q& rhs)
    {
        return { lhs.w + rhs.w, {lhs.x + rhs.x, lhs.y + rhs.y, lhs.z + rhs.z} };
    }


    Q
    Q::nlerp(const Q& f, const float scale, const Q& t)
    {
        const auto lhs = f * (1 - scale);
        const auto rhs = t * scale;
        return add(lhs, rhs).get_normalized();
    }


    Q
    Q::slerp_fast(const Q& qa, const float t, const Q& qb)
    {
        // from:
        // http://www.euclideanspace.com/maths/algebra/realNormedAlgebra/quaternions/slerp/
        // Calculate angle between them.
        const float cos_half_theta = qa.w * qb.w + qa.x * qb.x + qa.y * qb.y + qa.z * qb.z;
        // if qa=qb or qa=-qb then theta = 0 and we can return qa
        if(cabs(cos_half_theta) >= 1.0f)
        {
            return qa;
        }
        // Calculate temporary values.
        const auto half_theta = eu::acos(cos_half_theta);
        const auto sin_half_theta = std::sqrt(1.0f - cos_half_theta * cos_half_theta);
        if(cabs(sin_half_theta) < 0.001f)
        {
            // if theta = 180 degrees then result is not fully defined
            // we could rotate around any axis normal to qa or qb
            const Q qt = add(qa, qb);
            return Q
            {
                qt.w * 0.5f,
                v3
                {
                    qt.x * 0.5f,
                    qt.y * 0.5f,
                    qt.z * 0.5f
                }
            };
        }
        const float ratio_a = eu::sin((1 - t) * half_theta) / sin_half_theta;
        const float ratio_b = eu::sin(t * half_theta) / sin_half_theta;
        return add(qa * ratio_a, qb * ratio_b);
    }


    Q
    Q::slerp(const Q& from, const float scale, const Q& to)
    {
        if(dot(from, to) < 0)
        {
            return slerp_fast(from.get_negated(), scale, to);
        }
        else
        {
            return slerp_fast(from, scale, to);
        }
    }


    void
    Q::operator*=(float rhs)
    {
        x *= rhs;
        y *= rhs;
        z *= rhs;
        w *= rhs;
    }


    void
    Q::operator*=(const Q& rhs)
    {
#define VAR(a, b) const float a##1##b##2 = a * rhs.b
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

    Q Q::look_in_direction(const n3& forward, const n3& upwards)
    {
        const auto& z = forward;
        const auto x_new = z.cross_norm(upwards);

        if (x_new.has_value() == false)
        {
            return look_in_direction(forward, forward.y > 0 ? kk::out : kk::in);
        }
        const auto& x = *x_new;

        const auto y_new = x.cross_norm(z);
        ASSERT(y_new.has_value());
        const auto& y = *y_new;

        const auto m = m4::from_basis(x, y, -z);
        return Q::from_rotation_matrix(m);
    }


    std::string string_from(const Q& v)
    {
        return fmt::format("({}, ({}, {}, {}))", v.w, v.x, v.y, v.z);
    }


    float
    dot(const Q& lhs, const Q& rhs)
    {
        return lhs.x * rhs.x + lhs.y * rhs.y + lhs.z * rhs.z + lhs.w * rhs.w;
    }


    Q operator*(const Q& lhs, const Q& rhs)
    {
        Q r = lhs;
        r *= rhs;
        return r;
    }


    Q operator*(float scale, const Q& q)
    {
        Q r = q;
        r *= scale;
        return r;
    }


    Q operator*(const Q& q, float scale)
    {
        Q r = q;
        r *= scale;
        return r;
    }

    Q
    Q::from_rotation_matrix(const m4& m)
    {
        /// Implements the "Shepperd's method" as described in
        /// "3-D Computer Graphics A Mathematical Introduction with OpenGL" by Samuel R. Buss (2022)
        /// in section XII 3.6 Quaternion and rotation matrix conversions on page 465
        const auto m11 = m.get(0, 0);
        const auto m22 = m.get(1, 1);
        const auto m33 = m.get(2, 2);

        // aka trace
        const auto m00 = m11 + m22 + m33;

        switch (argmax(std::array{ m00, m11, m22, m33 }))
        {
        case 0:
        {
            const auto d = 0.5f * std::sqrt(m00 + 1.0f);

            const auto a = (m.get1(3, 2) - m.get1(2, 3)) / (4.0f * d);
            const auto b = (m.get1(1, 3) - m.get1(3, 1)) / (4.0f * d);
            const auto c = (m.get1(2, 1) - m.get1(1, 2)) / (4.0f * d);

            return Q{ d, {a, b, c} }.get_normalized();
        }
        case 1:
        {
            const auto a = 0.5f * std::sqrt(2.0f * m11 - m00 + 1.0f);

            const auto d = (m.get1(3, 2) - m.get1(2, 3)) / (4.0f * a);
            const auto b = (m.get1(2, 1) + m.get1(1, 2)) / (4.0f * a);
            const auto c = (m.get1(1, 3) + m.get1(3, 1)) / (4.0f * a);

            return Q{ d, {a, b, c} }.get_normalized();
        }
        case 2:
        {
            const auto b = 0.5f * std::sqrt(2.0f * m22 - m00 + 1.0f);

            const auto d = (m.get1(1, 3) - m.get1(3, 1)) / (4.0f * b);
            const auto a = (m.get1(2, 1) + m.get1(1, 2)) / (4.0f * b);
            const auto c = (m.get1(3, 2) + m.get1(2, 3)) / (4.0f * b);

            return Q{ d, {a, b, c} }.get_normalized();
        }
        case 3:
        {
            const auto c = 0.5f * std::sqrt(2.0f * m33 - m00 + 1.0f);

            const auto d = (m.get1(2, 1) - m.get1(1, 2)) / (4.0f * c);
            const auto a = (m.get1(1, 3) + m.get1(3, 1)) / (4.0f * c);
            const auto b = (m.get1(3, 2) + m.get1(2, 3)) / (4.0f * c);

            return Q{ d, {a, b, c} }.get_normalized();
        }
        default:
            DIE("shouldn't happen!!!");
            return q_identity;
        }
    }

    ADD_CATCH_FORMATTER_IMPL(Q)
}


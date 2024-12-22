#include "base/quat.h"


namespace eu
{
    float* Q::get_data_ptr()
    {
        return &w;
    }

    [[nodiscard]] const float* Q::get_data_ptr() const
    {
        return &w;
    }

    v3 Q::get_vec_part() const
    {
        return {x, y, z};
    }


    [[nodiscard]] Q
    Q::from_axis_angle(const AA& aa)
    {
        const float sin_a = sin(aa.angle / 2);
        const float cos_a = cos(aa.angle / 2);
        Q r(cos_a, aa.axis * sin_a);
        r.normalize();
        return r;
    }


    [[nodiscard]] Q
    Q::from_ypr(const An& yaw, const An& pitch, const An& roll)
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


    [[nodiscard]] Q
    Q::from_to(const Q& from, const Q& to)
    {
        // https://stackoverflow.com/a/22167097
        return to * from.get_inverse();
    }


    [[nodiscard]] m4
    Q::to_mat4() const
    {
        return m4::from_axis_angle(to_axis_angle());
    }


    [[nodiscard]] AA
    Q::to_axis_angle() const
    {
        const float cos_a = w;
        const auto angle = acos(cos_a) * 2;
        const auto sin_a = get_default_if_close_to_zero<float>(
                sqrt(1.0f - cos_a * cos_a), 1, 0.0005f);
        // todo(Gustav): do we need to normalize here?
        return AA::from_right_hand_around(
                (get_vec_part() / sin_a).get_normalized(), angle);
    }


    // static Q FromAngles(float x, float y, float z);


    [[nodiscard]] Q
    Q::look_at(const v3& from, const v3& to, const n3& up)
    {
        return look_in_direction(v3::from_to(from, to).get_normalized(), up);
    }


    Q
    Q::get_rotated(const Q& q) const
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
        // todo(Gustav): assert here
        // get_length_squared() == 1
        return get_conjugate();
    }


    // the negated represents the same rotation
    Q
    Q::get_negated() const
    {
        return {-w, -get_vec_part()};
    }


    Q
    Q::get_identity() const
    {
        const float l2 = get_length_squared();
        if(is_equal(l2, 0.0f)) { return q_identity; }
        else if(is_equal(l2, 1.0f)) { return get_conjugate(); }
        else { return {w / sqrt(l2), -get_vec_part()}; }
    }


    float
    Q::get_length() const
    {
        return sqrt(get_length_squared());
    }


    float
    Q::get_length_squared() const
    {
        return x * x + y * y + z * z + w * w;
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


    n3 Q::get_in   () const { return rotate_around_origo(-common::z_axis); }
    n3 Q::get_out  () const { return rotate_around_origo( common::z_axis); }
    n3 Q::get_right() const { return rotate_around_origo( common::x_axis); }
    n3 Q::get_left () const { return rotate_around_origo(-common::x_axis); }
    n3 Q::get_up   () const { return rotate_around_origo( common::y_axis); }
    n3 Q::get_down () const { return rotate_around_origo(-common::y_axis); }


    // In*Z + Right*X + Up*Y
    v3
    Q::create_from_right_up_in(const v3& v) const
    {
        return get_in() * v.z + get_right() * v.x + get_up() * v.y;
    }


    n3
    Q::rotate_around_origo(const n3& v) const
    {
        // http://gamedev.stackexchange.com/questions/28395/rotating-vector3-by-a-quaternion
        const Q pure = {0, v};
        const Q a = *this * pure;
        const Q ret = a * get_conjugate();
        return ret.get_vec_part().get_normalized();
    }


    Q
    lerp_quatf(const Q& f, const float scale, const Q& t)
    {
        return f * (1 - scale) + t * scale;
    }


    Q
    slerp_fast(const Q& qa, const float t, const Q& qb)
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
            const Q qt = qa + qb;
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
        const float ratio_a = sin((1 - t) * half_theta) / sin_half_theta;
        const float ratio_b = sin(t * half_theta) / sin_half_theta;
        return qa * ratio_a + qb * ratio_b;
    }


    Q
    slerp_shortway(const Q& from, const float scale, const Q& to)
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
    Q::operator+=(const Q& rhs)
    {
        x += rhs.x;
        y += rhs.y;
        z += rhs.z;
        w += rhs.w;
    }


    void
    Q::operator-=(const Q& rhs)
    {
        x -= rhs.x;
        y -= rhs.y;
        z -= rhs.z;
        w -= rhs.w;
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

    Q Q::look_in_direction(const n3& dir, const n3& up)
    {
        const v3 in = common::in;
        float dot_value = in.dot(dir);

        if (abs(dot_value - (-1.0f)) < 0.000001f)
        {
            // todo(Gustav): replace with a constant in general but this line specifically
            return {3.1415926535897932f, up};
        }
        if (abs(dot_value - (1.0f)) < 0.000001f)
        {
            return q_identity;
        }

        const auto rot_angle = acos(dot_value);
        const auto rot_axis = in.cross(dir).get_normalized();
        return Q::from_axis_angle
        (
            AA::from_right_hand_around(rot_axis, rot_angle)
        );
    }


    std::string to_string(const Q& v)
    {
        return fmt::format("({} ({}, {}, {}))", v.w, v.x, v.y, v.z);
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
    operator+(const Q& lhs, const Q& rhs)
    {
        Q r = rhs;
        r += lhs;
        return r;
    }


    Q
    operator-(const Q& lhs, const Q& rhs)
    {
        Q r = rhs;
        r -= lhs;
        return r;
    }
}


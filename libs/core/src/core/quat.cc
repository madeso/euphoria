#include "core/quat.h"


namespace euphoria::core
{
    float* quatf::get_data_ptr()
    {
        return &w;
    }

    [[nodiscard]] const float* quatf::get_data_ptr() const
    {
        return &w;
    }

    vec3f quatf::get_vec_part() const
    {
        return vec3f(x, y, z);
    }


    quatf::quatf(float aw, const vec3f& v)
        : w(aw)
        , x(v.x)
        , y(v.y)
        , z(v.z)
    {
    }


    [[nodiscard]] quatf
    quatf::from_axis_angle(const AxisAngle& aa)
    {
        const float sin_a = sin(aa.angle / 2);
        const float cos_a = cos(aa.angle / 2);
        quatf r(cos_a, aa.axis * sin_a);
        r.normalize();
        return r;
    }


    [[nodiscard]] quatf
    quatf::from_ypr(const angle& yaw, const angle& pitch, const angle& roll)
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


    [[nodiscard]] quatf
    quatf::from_to(const quatf& from, const quatf& to)
    {
        // https://stackoverflow.com/a/22167097
        return to * from.get_inverse();
    }


    [[nodiscard]] quatf
    quatf::from_random(Random* random)
    {
        const auto axis = get_random_unit3(random);
        const auto angle = angle::random(random);

        return quatf::from_axis_angle(AxisAngle::right_hand_around(axis, angle));
    }


    [[nodiscard]] mat4<float>
    quatf::to_mat4() const
    {
        return mat4<float>::from_axis_angle(to_axis_angle());
    }


    [[nodiscard]] AxisAngle
    quatf::to_axis_angle() const
    {
        const float cos_a = w;
        const auto angle = acos(cos_a) * 2;
        const float sin_a = get_default_if_close_to_zero<float>(
                sqrt(1.0f - cos_a * cos_a), 1, 0.0005f);
        // todo(Gustav): do we need to normalize here?
        return AxisAngle::right_hand_around(
                (get_vec_part() / sin_a).get_normalized(), angle);
    }


    // static Q FromAngles(float x, float y, float z);

    [[nodiscard]] quatf
    quatf::identity()
    {
        return quatf(1, vec3f(0, 0, 0));
    }


    [[nodiscard]] quatf
    quatf::look_at(const vec3f& from, const vec3f& to, const unit3f up)
    {
        return look_in_direction(vec3f::from_to(from, to).get_normalized(), up);
    }


    quatf
    quatf::rotate(const quatf& q) const
    {
        return q * *this;
    }


    quatf
    quatf::get_conjugate() const
    {
        return quatf(w, -get_vec_part());
    }


    quatf
    quatf::get_inverse() const
    {
        // todo(Gustav): assert here
        // get_length_squared() == 1
        return get_conjugate();
    }


    // the negated represents the same rotation
    quatf
    quatf::get_negated() const
    {
        return quatf(-w, -get_vec_part());
    }


    quatf
    quatf::get_identity() const
    {
        const float l2 = get_length_squared();
        if(is_equal(l2, 0.0f)) { return identity(); }
        else if(is_equal(l2, 1.0f)) { return get_conjugate(); }
        else { return quatf(w / sqrt(l2), -get_vec_part()); }
    }


    float
    quatf::get_length() const
    {
        return sqrt(get_length_squared());
    }


    float
    quatf::get_length_squared() const
    {
        return x * x + y * y + z * z + w * w;
    }


    void
    quatf::normalize()
    {
        const float l = get_length();
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


    quatf
    quatf::get_normalized() const
    {
        quatf r = *this;
        r.normalize();
        return r;
    }


    unit3f
    quatf::in() const
    {
        return rotate_around_origo(-unit3f::z_axis());
    }


    unit3f
    quatf::out() const
    {
        return rotate_around_origo(unit3f::z_axis());
    }


    unit3f
    quatf::right() const
    {
        return rotate_around_origo(unit3f::x_axis());
    }


    unit3f
    quatf::left() const
    {
        return rotate_around_origo(-unit3f::x_axis());
    }


    unit3f
    quatf::up() const
    {
        return rotate_around_origo(unit3f::y_axis());
    }


    unit3f
    quatf::down() const
    {
        return rotate_around_origo(-unit3f::y_axis());
    }


    // In*Z + Right*X + Up*Y
    vec3f
    quatf::create_from_right_up_in(const vec3f v) const
    {
        return in() * v.z + right() * v.x + up() * v.y;
    }


    unit3f
    quatf::rotate_around_origo(const unit3f& v) const
    {
        // http://gamedev.stackexchange.com/questions/28395/rotating-vector3-by-a-quaternion
        const quatf pure(0, v);
        const quatf a = *this * pure;
        const quatf ret = a * get_conjugate();
        return ret.get_vec_part().get_normalized();
    }


    [[nodiscard]] quatf
    quatf::lerp(const quatf& f, const float scale, const quatf& t)
    {
        return f * (1 - scale) + t * scale;
    }


    [[nodiscard]] quatf
    quatf::slerp(const quatf& qa, const float t, const quatf& qb)
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
            const quatf qt = qa + qb;
            return quatf
            {
                qt.w * 0.5f,
                vec3f
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


    [[nodiscard]] quatf
    quatf::slerp_shortway(const quatf& from, const float scale, const quatf& to)
    {
        if(dot(from, to) < 0)
        {
            return slerp(from.get_negated(), scale, to);
        }
        else
        {
            return slerp(from, scale, to);
        }
    }


    void
    quatf::operator+=(const quatf& rhs)
    {
        x += rhs.x;
        y += rhs.y;
        z += rhs.z;
        w += rhs.w;
    }


    void
    quatf::operator-=(const quatf& rhs)
    {
        x -= rhs.x;
        y -= rhs.y;
        z -= rhs.z;
        w -= rhs.w;
    }


    void
    quatf::operator*=(const float& rhs)
    {
        x *= rhs;
        y *= rhs;
        z *= rhs;
        w *= rhs;
    }


    void
    quatf::operator*=(const quatf& rhs)
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

    quatf quatf::look_in_direction(const unit3f& dir, const unit3f& up)
    {
        const vec3f in = unit3f::in();
        float dot_value = dot(in, dir);

        if (abs(dot_value - (-1.0f)) < 0.000001f)
        {
            return quatf(3.1415926535897932f, up);
        }
        if (abs(dot_value - (1.0f)) < 0.000001f)
        {
            return identity();
        }

        const auto rot_angle = acos(dot_value);
        const auto rot_axis = cross(in, dir).get_normalized();
        return quatf::from_axis_angle
        (
            AxisAngle::right_hand_around(rot_axis, rot_angle)
        );
    }


    std::ostream&
    operator<<(std::ostream& stream, const quatf& v)
    {
        return stream << "(" << v.w << " (" << v.x << ", " << v.y << ", " << v.z
                      << "))";
    }


    float
    dot(const quatf& lhs, const quatf& rhs)
    {
        return lhs.x * rhs.x + lhs.y * rhs.y + lhs.z * rhs.z + lhs.w * rhs.w;
    }


    quatf operator*(const quatf& lhs, const quatf& rhs)
    {
        quatf r = lhs;
        r *= rhs;
        return r;
    }


    quatf operator*(float scale, const quatf& q)
    {
        quatf r = q;
        r *= scale;
        return r;
    }


    quatf operator*(const quatf& q, float scale)
    {
        quatf r = q;
        r *= scale;
        return r;
    }


    quatf
    operator+(const quatf& lhs, const quatf& rhs)
    {
        quatf r = rhs;
        r += lhs;
        return r;
    }


    quatf
    operator-(const quatf& lhs, const quatf& rhs)
    {
        quatf r = rhs;
        r -= lhs;
        return r;
    }


    bool
    operator==(const quatf& lhs, const quatf& rhs)
    {
        return lhs.x == rhs.x && lhs.y == rhs.y && lhs.z == rhs.z
               && lhs.w == rhs.w;
    }
}


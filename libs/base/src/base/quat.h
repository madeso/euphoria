#pragma once

#include "base/vec3.h"
#include "base/numeric.h"
#include "base/axisangle.h"
#include "base/angle.h"
#include "base/mat4.h"


namespace eu
{
    struct quatf
    {
        float w;
        float x;
        float y;
        float z;

        constexpr quatf(float aw, const v3& v)
            : w(aw)
            , x(v.x)
            , y(v.y)
            , z(v.z)
        {
        }

        [[nodiscard]] static quatf from_axis_angle(const AxisAngle& aa);
        [[nodiscard]] static quatf from_ypr(const Angle& yaw, const Angle& pitch, const Angle& roll);
        [[nodiscard]] static quatf from_to(const quatf& from, const quatf& to);
        [[nodiscard]] static quatf look_at(const v3& from, const v3& to, const n3& up);
        [[nodiscard]] static quatf look_in_direction(const n3& dir, const n3& up);

        float* get_data_ptr();
        void normalize();

        /// returns In*Z + Right*X + Up*Y
        [[nodiscard]] v3 create_from_right_up_in(const v3& v) const;
        [[nodiscard]] quatf get_rotated(const quatf& q) const;
        [[nodiscard]] n3 rotate_around_origo(const n3& v) const;

        // the negated represents the same rotation
        [[nodiscard]] quatf get_negated() const;

        [[nodiscard]] m4 to_mat4() const;
        [[nodiscard]] AxisAngle to_axis_angle() const;
        [[nodiscard]] const float* get_data_ptr() const;
        [[nodiscard]] v3 get_vec_part() const;
        [[nodiscard]] quatf get_conjugate() const;
        [[nodiscard]] quatf get_inverse() const;
        [[nodiscard]] quatf get_identity() const;
        [[nodiscard]] float get_length() const;
        [[nodiscard]] float get_length_squared() const;
        [[nodiscard]] quatf get_normalized() const;

        [[nodiscard]] n3 get_in() const;
        [[nodiscard]] n3 get_out() const;
        [[nodiscard]] n3 get_right() const;
        [[nodiscard]] n3 get_left() const;
        [[nodiscard]] n3 get_up() const;
        [[nodiscard]] n3 get_down() const;

        void operator+=(const quatf& rhs);
        void operator-=(const quatf& rhs);
        void operator*=(float rhs);
        void operator*=(const quatf& rhs);
    };

    constexpr quatf q_identity = quatf(1, v3(0, 0, 0));

    quatf lerp_quatf(const quatf& f, float scale, const quatf& t);
    quatf slerp_fast(const quatf& qa, float t, const quatf& qb);
    quatf slerp_shortway(const quatf& from, float scale, const quatf& to);

    float dot(const quatf& lhs, const quatf& rhs);
    
    std::string to_string(const quatf& v);

    quatf operator*(const quatf& lhs, const quatf& rhs);
    quatf operator*(float scale, const quatf& q);
    quatf operator*(const quatf& q, float scale);
    quatf operator+(const quatf& lhs, const quatf& rhs);
    quatf operator-(const quatf& lhs, const quatf& rhs);
}

ADD_DEFAULT_FORMATTER(eu::quatf, std::string, eu::to_string);

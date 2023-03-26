#pragma once

#include "core/vec3.h"
#include "core/numeric.h"
#include "core/axisangle.h"
#include "core/angle.h"
#include "core/mat4.h"
#include "core/polarcoord.h"
#include "core/interpolate.default.h"


namespace euphoria::core
{
    struct quatf
    {
        float w;
        float x;
        float y;
        float z;

        constexpr quatf(float aw, const vec3f& v)
            : w(aw)
            , x(v.x)
            , y(v.y)
            , z(v.z)
        {
        }

        [[nodiscard]] static quatf from_axis_angle(const AxisAngle& aa);
        [[nodiscard]] static quatf from_ypr(const angle& yaw, const angle& pitch, const angle& roll);
        [[nodiscard]] static quatf from_to(const quatf& from, const quatf& to);
        [[nodiscard]] static quatf from_random(Random* random);
        [[nodiscard]] static quatf look_at(const vec3f& from, const vec3f& to, const unit3f& up);
        [[nodiscard]] static quatf look_in_direction(const unit3f& dir, const unit3f& up);

        float* get_data_ptr();
        void normalize();

        /// returns In*Z + Right*X + Up*Y
        [[nodiscard]] vec3f create_from_right_up_in(const vec3f& v) const;
        [[nodiscard]] quatf get_rotated(const quatf& q) const;
        [[nodiscard]] unit3f rotate_around_origo(const unit3f& v) const;

        // the negated represents the same rotation
        [[nodiscard]] quatf get_negated() const;

        [[nodiscard]] mat4f to_mat4() const;
        [[nodiscard]] AxisAngle to_axis_angle() const;
        [[nodiscard]] const float* get_data_ptr() const;
        [[nodiscard]] vec3f get_vec_part() const;
        [[nodiscard]] quatf get_conjugate() const;
        [[nodiscard]] quatf get_inverse() const;
        [[nodiscard]] quatf get_identity() const;
        [[nodiscard]] float get_length() const;
        [[nodiscard]] float get_length_squared() const;
        [[nodiscard]] quatf get_normalized() const;

        [[nodiscard]] unit3f get_in() const;
        [[nodiscard]] unit3f get_out() const;
        [[nodiscard]] unit3f get_right() const;
        [[nodiscard]] unit3f get_left() const;
        [[nodiscard]] unit3f get_up() const;
        [[nodiscard]] unit3f get_down() const;

        void operator+=(const quatf& rhs);
        void operator-=(const quatf& rhs);
        void operator*=(float rhs);
        void operator*=(const quatf& rhs);
    };

    constexpr quatf q_identity = quatf(1, vec3f(0, 0, 0));

    quatf lerp_quatf(const quatf& f, float scale, const quatf& t);
    quatf slerp_fast(const quatf& qa, float t, const quatf& qb);
    quatf slerp_shortway(const quatf& from, float scale, const quatf& to);

    DEFAULT_INTERPOLATE(quatf, slerp_shortway);

    float dot(const quatf& lhs, const quatf& rhs);
    
    std::string to_string(const quatf& v);

    quatf operator*(const quatf& lhs, const quatf& rhs);
    quatf operator*(float scale, const quatf& q);
    quatf operator*(const quatf& q, float scale);
    quatf operator+(const quatf& lhs, const quatf& rhs);
    quatf operator-(const quatf& lhs, const quatf& rhs);
}

ADD_DEFAULT_FORMATTER(euphoria::core::quatf, std::string, euphoria::core::to_string);

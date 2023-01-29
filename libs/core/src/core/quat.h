#pragma once

#include "core/vec3.h"
#include "core/numeric.h"
#include "core/axisangle.h"
#include "core/angle.h"
#include "core/mat4.h"
#include "core/polarcoord.h"


namespace euphoria::core
{
    struct quatf
    {
        float w;
        float x;
        float y;
        float z;

        ///////////////////////////////////////////////////////////////////////////////////////////
        // construction

        quatf(float aw, const vec3f& v);

        [[nodiscard]] static quatf
        from_axis_angle(const AxisAngle& aa);

        [[nodiscard]] static quatf
        from_ypr(const angle& yaw, const angle& pitch, const angle& roll);

        [[nodiscard]] static quatf
        from_to(const quatf& from, const quatf& to);

        [[nodiscard]] static quatf
        from_random(Random* random);

        [[nodiscard]] static quatf
        identity();

        [[nodiscard]] static quatf
        look_at(const vec3f& from, const vec3f& to, const unit3f up);

        [[nodiscard]] static quatf
        look_in_direction(const unit3f& dir, const unit3f& up);


        ///////////////////////////////////////////////////////////////////////////////////////////
        // To functions

        [[nodiscard]] mat4<float>
        to_mat4() const;

        [[nodiscard]] AxisAngle
        to_axis_angle() const;

        float* get_data_ptr();

        [[nodiscard]] const float* get_data_ptr() const;

        vec3f get_vec_part() const;

        quatf rotate(const quatf& q) const;

        quatf get_conjugate() const;

        quatf get_inverse() const;

        // the negated represents the same rotation
        quatf get_negated() const;

        quatf get_identity() const;

        float get_length() const;
        float get_length_squared() const;

        void normalize();
        quatf get_normalized() const;

        unit3f in() const;
        unit3f out() const;
        unit3f right() const;
        unit3f left() const;
        unit3f up() const;
        unit3f down() const;

        // In*Z + Right*X + Up*Y
        vec3f
        create_from_right_up_in(const vec3f v) const;

        unit3f
        rotate_around_origo(const unit3f& v) const;

        [[nodiscard]] static quatf
        lerp(const quatf& f, const float scale, const quatf& t);

        [[nodiscard]] static quatf
        slerp(const quatf& qa, const float t, const quatf& qb);

        // todo(Gustav): rename this to slerp and slerp to slerp_fast or slerp_base
        [[nodiscard]] static quatf
        slerp_shortway(const quatf& from, const float scale, const quatf& to);

        void operator+=(const quatf& rhs);
        void operator-=(const quatf& rhs);
        void operator*=(const float& rhs);
        void operator*=(const quatf& rhs);
    };

    
    std::ostream& operator<<(std::ostream& stream, const quatf& v);

    float dot(const quatf& lhs, const quatf& rhs);

    quatf operator*(const quatf& lhs, const quatf& rhs);
    quatf operator*(float scale, const quatf& q);
    quatf operator*(const quatf& q, float scale);
    quatf operator+(const quatf& lhs, const quatf& rhs);
    quatf operator-(const quatf& lhs, const quatf& rhs);
    bool operator==(const quatf& lhs, const quatf& rhs);
}


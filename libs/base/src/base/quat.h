#pragma once

#include "base/vec3.h"
#include "base/numeric.h"
#include "base/axisangle.h"
#include "base/angle.h"
#include "base/mat4.h"


namespace eu
{
    /** \addtogroup math
     *  @{
     */


    /// A quaternion.
    /// It is useful for representing a rotation in 3d.
    struct Q
    {
        float w;
        float x;
        float y;
        float z;

        constexpr Q(float aw, const v3& v)
            : w(aw)
            , x(v.x)
            , y(v.y)
            , z(v.z)
        {
        }

        [[nodiscard]] static Q from(const AA& aa);
        [[nodiscard]] static Q from(const Ypr& ypr);
        [[nodiscard]] static Q from_to(const Q& from, const Q& to);
        [[nodiscard]] static std::optional<Q> look_at(const v3& from, const v3& to, const n3& up);
        [[nodiscard]] static Q look_in_direction(const n3& dir, const n3& up);

        float* get_data_ptr();
        void normalize();

        /// returns In*Z + Right*X + Up*Y
        [[nodiscard]] v3 create_from_right_up_in(const v3& v) const;
        [[nodiscard]] Q get_rotated(const Q& q) const;
        [[nodiscard]] n3 rotate_around_origo(const n3& v) const;

        // the negated represents the same rotation
        [[nodiscard]] Q get_negated() const;

        [[nodiscard]] const float* get_data_ptr() const;
        [[nodiscard]] v3 get_vec_part() const;
        [[nodiscard]] Q get_conjugate() const;
        [[nodiscard]] Q get_inverse() const;
        [[nodiscard]] Q get_identity() const;
        [[nodiscard]] float get_length() const;
        [[nodiscard]] float get_length_squared() const;
        [[nodiscard]] Q get_normalized() const;

        [[nodiscard]] n3 get_in() const;
        [[nodiscard]] n3 get_out() const;
        [[nodiscard]] n3 get_right() const;
        [[nodiscard]] n3 get_left() const;
        [[nodiscard]] n3 get_up() const;
        [[nodiscard]] n3 get_down() const;

        void operator+=(const Q& rhs);
        void operator-=(const Q& rhs);
        void operator*=(float rhs);
        void operator*=(const Q& rhs);
    };

    constexpr Q q_identity = Q(1, v3(0, 0, 0));

    Q lerp_quatf(const Q& f, float scale, const Q& t);
    Q slerp_fast(const Q& qa, float t, const Q& qb);
    Q slerp_shortway(const Q& from, float scale, const Q& to);

    float dot(const Q& lhs, const Q& rhs);
    
    std::string string_from(const Q& v);

    Q operator*(const Q& lhs, const Q& rhs);
    Q operator*(float scale, const Q& q);
    Q operator*(const Q& q, float scale);
    Q operator+(const Q& lhs, const Q& rhs);
    Q operator-(const Q& lhs, const Q& rhs);

    /** @}*/
}

ADD_DEFAULT_FORMATTER(eu::Q, std::string, eu::string_from);


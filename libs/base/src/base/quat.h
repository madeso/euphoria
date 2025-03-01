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


    /// A quaternion representing a rotation in 3d.
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

        /// Create a quaternion from an axis angle
        [[nodiscard]] static Q from(const AA& aa);

        /// Create a quaternion from a yaw-pitch-roll
        [[nodiscard]] static Q from(const Ypr& ypr);

        /// Create a quaternion going from `from` to `to`.
        [[nodiscard]] static Q from_to(const Q& from, const Q& to);

        /// Creates a look-at quaternion from 2 positions.
        /// Standing at `from` and up is `up`,  the result will be a quaternion looking at `to`
        [[nodiscard]] static std::optional<Q> look_at(const v3& from, const v3& to, const n3& up);

        /// Creates a look-at quaternion looking in a direction.
        [[nodiscard]] static Q look_in_direction(const n3& dir, const n3& up);

        /// Normalize the quaternion.
        /// If it can't be normalized, it is set to the identity.
        void normalize();

        /// Return the passed rotation composed after the current rotation.
        [[nodiscard]] Q then_get_rotated(const Q& q) const;

        /// Rotate a unit vector according to the quaternion
        [[nodiscard]] n3 get_rotated(const n3& v) const;

        /// Gets the negated quaternion.
        /// The negated represents the same rotation
        [[nodiscard]] Q get_negated() const;

        /// Get the `[x,y,z]` part as a regular 3d vector.
        [[nodiscard]] v3 get_vec_part() const;

        /// Returns the conjugate of the quaternion.
        [[nodiscard]] Q get_conjugate() const;

        /// Gets the inverse rotation.
        /// Implemented as a conjugate with assert that the quaternion is normalized.
        /// @see \ref get_conjugate()
        [[nodiscard]] Q get_inverse() const;

        /// Gets the length of the quaternion.
        /// Since the quaternion should be a unit, this should always be `1`
        [[nodiscard]] float get_length() const;

        /// Return a normalized quaternion
        /// If it can't be normalized, the identity is returned
        [[nodiscard]] Q get_normalized() const;

        /// Get the local in vector.
        [[nodiscard]] n3 get_local_in() const;

        /// Get the local out vector.
        [[nodiscard]] n3 get_local_out() const;

        /// Get the local right vector.
        [[nodiscard]] n3 get_local_right() const;

        /// Get the local left vector.
        [[nodiscard]] n3 get_local_left() const;

        /// Get the local up vector.
        [[nodiscard]] n3 get_local_up() const;

        /// Get the local down vector.
        [[nodiscard]] n3 get_local_down() const;

        void operator+=(const Q& rhs);
        void operator-=(const Q& rhs);
        void operator*=(float rhs);
        void operator*=(const Q& rhs);

        /// Normalized lerp between 2 quaternions
        /// This will result in a non-linear rotation
        /// @see \ref slerp()
        static Q nlerp(const Q& f, float scale, const Q& t);

        /// Spherical lerp between 2 quaternions.
        /// Will take the longer route sometimes but is technically faster.
        /// @see \ref nlerp()
        static Q slerp_fast(const Q& qa, float t, const Q& qb);

        /// Shortest spherical lerp between 2 quaternions.
        /// Has extra logic to take the shortest route.
        /// @see \ref slerp_fast()
        /// @see \ref nlerp()
        static Q slerp(const Q& from, float scale, const Q& to);
    };

    /// The identity quaternion.
    constexpr Q q_identity = Q(1, v3(0, 0, 0));

    float dot(const Q& lhs, const Q& rhs);

    /// Converts a quaternion to string, prefer fmt.
    std::string string_from(const Q& v);

    Q operator*(const Q& lhs, const Q& rhs);
    Q operator*(float scale, const Q& q);
    Q operator*(const Q& q, float scale);
    Q operator+(const Q& lhs, const Q& rhs);
    Q operator-(const Q& lhs, const Q& rhs);

    /** @}*/
    ADD_CATCH_FORMATTER_DEF(Q)
}

ADD_DEFAULT_FORMATTER(eu::Q, std::string, eu::string_from);


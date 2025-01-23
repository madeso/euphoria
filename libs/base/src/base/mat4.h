#pragma once

#include "base/vec3.h"
#include "base/vec4.h"
#include "base/angle.h"
#include "base/axisangle.h"


namespace eu
{
    /** \addtogroup math
     *  @{
     */

    struct Q;

    /// 4x4 matrix
    struct m4
    {
        /// Create a new matrix from column major format.
        [[nodiscard]] static m4 from_col_major(
            float t00, float t01, float t02, float t03,
            float t10, float t11, float t12, float t13,
            float t20, float t21, float t22, float t23,
            float t30, float t31, float t32, float t33);

        /// Create a new matrix from row major format.
        [[nodiscard]] constexpr static m4 from_row_major(
            float t00, float t10, float t20, float t30,
            float t01, float t11, float t21, float t31,
            float t02, float t12, float t22, float t32,
            float t03, float t13, float t23, float t33)
        {
            return {
                t00, t01, t02, t03,
                t10, t11, t12, t13,
                t20, t21, t22, t23,
                t30, t31, t32, t33};
        }

        /// Create a matrix from the major, aka the diagonal.
        /// @see from_scalar
        [[nodiscard]] static m4 from_major(const v4 &major);

        /// Create a matrix from a single scalar.
        /// @see from_major
        [[nodiscard]] constexpr static m4 from_scalar(float scalar)
        {
            const float z = 0;
            return from_row_major(
                scalar, z, z, z,
                z, scalar, z, z,
                z, z, scalar, z,
                z, z, z, scalar);
        }

        /// Create a translation matrix.
        [[nodiscard]] static m4 from_translation(const v3 &v);

        /// Create a rotation matrix, around the X axis.
        [[nodiscard]] static m4 from_rot_x(const An &a);

        /// Create a rotation matrix, around the Y axis.
        [[nodiscard]] static m4 from_rot_y(const An &a);

        /// Create a rotation matrix, around the Z axis.
        [[nodiscard]] static m4 from_rot_z(const An &a);

        /// Create a rotation matrix, from an axis angle.
        [[nodiscard]] static m4 from(const AA &aa);

        /// Create a rotation matrix, from a quaternion.
        [[nodiscard]] static std::optional<m4> from(const Q& q);

        /// Create an orthographic projection matrix.
        /// Also known as a `clip_from_view` transformation.
        /// Clip Space is in OpenGL NDC (-1 to +1) z range.
        /// @param l the left side
        /// @param r the right side
        /// @param t the up side
        /// @param b the down side
        /// @param n near
        /// @param f far
        [[nodiscard]] static m4 create_ortho_lrud(float l, float r, float t, float b, float n, float f);

        /// Create a perspective projection matrix.
        [[nodiscard]] static m4 create_perspective(const An &fov, float aspect_ratio, float near, float far);

        /// Get a direct pointer to the data in column major format, for API integration.
        float *get_column_major_data_ptr();

        /// Get a direct pointer to the const data in column major format, for API integration.
        [[nodiscard]] const float* get_column_major_data_ptr() const;

        /// Invert the current matrix.
        /// @see get_inverted
        bool invert();

        /// Return the inverted matrix
        /// @see invert
        [[nodiscard]] m4 get_inverted() const;

        /// Return a single value given a row and column.
        [[nodiscard]] float get(int row, int col) const;

        /// Get a transformed vec4.
        [[nodiscard]] v4 get_transformed(const v4 &p) const;

        /// Get a transformed vec3, using the w component of the vector.
        [[nodiscard]] v3 get_transformed(const v3 &p, float w) const;

        /// Get a transformed vec3 assuming it's a point
        [[nodiscard]] v3 get_transformed_point(const v3 &p) const;

        /// Get a transformed vec3, assuming it's a normal vector
        [[nodiscard]] v3 get_transformed_vec(const v3 &p) const;

        /// Get a transformed unit3
        [[nodiscard]] n3 get_transformed_vec(const n3 &p) const;

        /// Get a row as a vec4.
        [[nodiscard]] v4 get_row(int r) const;

        /// Get a column as vec4.
        [[nodiscard]] v4 get_column(int c) const;

        /// Combine this with a translation matrix.
        [[nodiscard]] m4 get_translated(const v3 &t) const;

        /// Combine this with a rotation matrix.
        [[nodiscard]] m4 get_rotated(const AA &aa) const;
        
        /// Get the current translation of the transformation matrix.
        [[nodiscard]] v3 get_translation() const;

        /// Get the major vector.
        [[nodiscard]] v4 get_major() const;

        /// Get the local X axis.
        [[nodiscard]] n3 get_x_axis() const;

        /// Get the local Y axis.
        [[nodiscard]] n3 get_y_axis() const;

        /// Get the local Z azis.
        [[nodiscard]] n3 get_z_axis() const;

        /// Gets the transpose of a matrix.
        /// If the matrix is a rotation matrix, then the transpose is guaranteed to be the inverse of the matrix.
        [[nodiscard]] m4 get_transposed() const;
        

        void operator+=(const m4 &rhs);
        void operator-=(const m4 &rhs);

    private:
        /// stored in column major
        float data[16];

        m4() = default;

        constexpr m4(
            float t00, float t01, float t02, float t03,
            float t10, float t11, float t12, float t13,
            float t20, float t21, float t22, float t23,
            float t30, float t31, float t32, float t33)
            : data{
                  t00, t01, t02, t03,
                  t10, t11, t12, t13,
                  t20, t21, t22, t23,
                  t30, t31, t32, t33}
        {
        }
    };

    /// The identity matrix.
    constexpr m4 m4_identity = m4::from_scalar(1);

    /// Convert a matrix to a string representation, prefer fmt.
    std::string string_from(const m4 &m);

    m4 operator+(const m4 &lhs, const m4 &rhs);
    m4 operator-(const m4 &lhs, const m4 &rhs);
    m4 operator*(const m4 &lhs, const m4 &rhs);
    v4 operator*(const m4 &lhs, const v4 &rhs);

    /** @}*/

    ADD_CATCH_FORMATTER(m4)
}

ADD_DEFAULT_FORMATTER(eu::m4, std::string, eu::string_from);

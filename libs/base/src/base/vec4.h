#pragma once

#include "base/vec3.h"

#include "assert/assert.h"
#include "base/numeric.h"

namespace eu
{
    /** \addtogroup math
     *  @{
     */

    /// A 4d vector.
    /// It represents a homogeneous coordinate/projective coordinate.
    struct v4
    {
        float x;
        float y;
        float z;
        float w;

        v4(float ax, float ay, float az, float aw);

        /// @param a the xyz components
        /// @param aw the w component, point=1, vector=0
        v4(const v3 &a, float aw);

        /// Construct the vector from an array.
        /// It is assumed the array is of at least 4 elements in xyzw order.
        explicit v4(const float *a);

        /// asserts that the w component is what is expected, point=1, vector=0
        [[nodiscard]] v3 to_vec3(float ww) const;

        /// returns a 3d vector using perspective division
        [[nodiscard]] v3 to_vec3_persp_div() const;

        /// returns an array to the data
        float* get_data_ptr();

        /// returns an array to the data
        [[nodiscard]] const float *get_data_ptr() const;

        bool operator==(const v4 &rhs) = delete;
    };

    /// converts a 4d vector to a string, prefer fmt
    std::string string_from(const v4 &v);

    /** @}*/

    ADD_CATCH_FORMATTER(v4)
}

ADD_DEFAULT_FORMATTER(eu::v4, std::string, eu::string_from);


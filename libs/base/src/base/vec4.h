#pragma once

#include "base/vec3.h"

#include "assert/assert.h"
#include "base/numeric.h"

namespace eu
{
    /// A 4d vector, it represents a homogeneous coordinate/projective coordinate.
    struct v4
    {
        float x;
        float y;
        float z;
        float w;

        explicit v4(float a);
        v4(float ax, float ay, float az, float aw);

        /// @param aw point is 1, vector is 0
        v4(const v3 &a, float aw);

        explicit v4(const float *a);

        float *get_data_ptr();

        /// asserts that the w component is what is expected
        [[nodiscard]] v3 to_vec3(float ww) const;

        [[nodiscard]] const float *get_data_ptr() const;

        bool operator==(const v4 &rhs) = delete;
    };

    std::string to_string(const v4 &v);
}

ADD_DEFAULT_FORMATTER(eu::v4, std::string, eu::to_string);

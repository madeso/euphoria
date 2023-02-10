#pragma once

#include "core/vec3.h"

#include "assert/assert.h"
#include "core/numeric.h"



namespace euphoria::core
{
    // represents a homogeneous coordinate
    struct vec4f
    {
        float x;
        float y;
        float z;
        float w;

        explicit vec4f(float a);
        vec4f(float ax, float ay, float az, float aw);

        /// @param w point is 1, vector is 0
        vec4f(const vec3f& a, float aw);

        // todo(Gustav): make explicit?
        vec4f(const Scale3f& a);

        explicit vec4f(const float* a);

        /// asserts that the w component is what is expected
        [[nodiscard]] vec3f to_vec3(float ww) const;

        // todo(Gustav): replace this and instead always call the assert version (above)
        [[nodiscard]] vec3f to_vec3() const;

        float* get_data_ptr();

        [[nodiscard]] const float* get_data_ptr() const;

        [[nodiscard]] float get_component_sum() const;

        bool operator==(const vec4f& rhs) = delete;
    };

    std::ostream& operator<<(std::ostream& stream, const vec4f& v);
    vec4f component_multiply(const vec4f& lhs, const vec4f& rhs);
}

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

        /// @param aw point is 1, vector is 0
        vec4f(const vec3f& a, float aw);

        // todo(Gustav): make explicit?
        vec4f(const Scale3f& a);

        explicit vec4f(const float* a);

        float* get_data_ptr();

        /// asserts that the w component is what is expected
        [[nodiscard]] vec3f to_vec3(float ww) const;

        // todo(Gustav): replace this and instead always call the assert version (above)
        [[nodiscard]] vec3f to_vec3() const;

        [[nodiscard]] const float* get_data_ptr() const;

        bool operator==(const vec4f& rhs) = delete;
    };

    std::string to_string(const vec4f& v);
}

ADD_DEFAULT_FORMATTER(euphoria::core::vec4f, std::string, euphoria::core::to_string);

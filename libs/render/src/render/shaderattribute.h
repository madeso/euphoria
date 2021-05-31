#pragma once

#include <string>

#include "render/gltypes.h"

namespace euphoria::render
{
    enum class shader_attribute_type
    {
        float1,
        float2,
        float3,
        float4,
        float33,
        float44
    };

    enum class shader_attribute_source
    {
        unknown,
        vertex,
        normal,
        uv,
        color
    };

    /** Represents a shader attribute like vertex, normal or uv coord.
     */
    struct shader_attribute
    {
        // todo(Gustav): can this be made into a constexpr?
        shader_attribute
        (
            glint a_id,
            shader_attribute_type a_type,
            std::string a_name,
            shader_attribute_source a_source,
            bool a_normalize = false
        );

        [[nodiscard]] int
        get_element_count() const;

        [[nodiscard]] int
        get_byte_size() const;

        /// the id of the attribute
        glint id;

        /// the type of the attribute
        shader_attribute_type type;

        /// the name of the shader attribute
        std::string name;

        /// the source of the shader attribute
        shader_attribute_source source;

        // true if the attribute is normalized
        bool normalize;
    };

    bool
    operator==(const shader_attribute& lhs, const shader_attribute& rhs);
}

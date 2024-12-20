#pragma once

#include "render/gltypes.h"


namespace eu::render
{
    enum class ShaderAttributeType
    {
        float1,
        float2,
        float3,
        float4,
        float33,
        float44
    };

    enum class ShaderAttributeSource
    {
        unknown,
        vertex,
        normal,
        uv,
        color
    };

    /** Represents a shader attribute like vertex, normal or uv coord.
     */
    struct ShaderAttribute
    {
        // todo(Gustav): can this be made into a constexpr?
        constexpr ShaderAttribute
        (
            gl::Int a_id,
            ShaderAttributeType a_type,
            std::string_view a_name,
            ShaderAttributeSource a_source,
            bool a_normalize = false
        )
            : id(a_id)
            , type(a_type)
            , name(a_name)
            , source(a_source)
            , normalize(a_normalize)
        {
        }

        [[nodiscard]] int
        get_element_count() const;

        [[nodiscard]] int
        get_byte_size() const;

        /// the id of the attribute
        gl::Int id;

        /// the type of the attribute
        ShaderAttributeType type;

        /// the name of the shader attribute
        std::string_view name;

        /// the source of the shader attribute
        ShaderAttributeSource source;

        // true if the attribute is normalized
        bool normalize;
    };

    bool
    operator==(const ShaderAttribute& lhs, const ShaderAttribute& rhs);
}

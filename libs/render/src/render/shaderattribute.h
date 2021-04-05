#pragma once

#include <string>

#include "render/gltypes.h"

namespace euphoria::render
{
    enum class ShaderAttributeType
    {
        FLOAT1,
        FLOAT2,
        FLOAT3,
        FLOAT4,
        FLOAT33,
        FLOAT44
    };

    enum class ShaderAttributeSource
    {
        Unknown,
        Vertex,
        Normal,
        Uv,
        Color
    };

    /** Respresents a shader attribute like vertex, normal or uv coord.
     */
    struct ShaderAttribute
    {
        // todo(Gustav): can this be maded constexpr?
        ShaderAttribute
        (
            glint a_id,
            ShaderAttributeType a_type,
            std::string a_name,
            ShaderAttributeSource a_source,
            bool a_normalize = false
        );

        [[nodiscard]] int
        GetElementCount() const;

        [[nodiscard]] int
        GetByteSize() const;

        /// the id of the attribute
        glint id;

        /// the type of the attribute
        ShaderAttributeType type;

        /// the name of the shader attribute
        std::string name;

        /// the source of the shader attribute
        ShaderAttributeSource source;

        // true if the attribute is normalized
        bool normalize;
    };

    bool
    operator==(const ShaderAttribute& lhs, const ShaderAttribute& rhs);
}

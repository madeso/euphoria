#pragma once

#include "render/shaderattribute.h"


namespace eu::render
{
    struct ShaderProgram;
}

// todo(Gustav): Refactor into a pipeline/layout structure
/*
Currently attributes are hardcoded into a special layout.
It would be preferable if the layout is written in script
and the actual values are dynamically created in script.
*/
namespace eu::render::attributes3d
{
    constexpr ShaderAttribute vertex = ShaderAttribute
    {
        1,
        ShaderAttributeType::float3,
        "aPosition",
        ShaderAttributeSource::vertex
    };

    constexpr ShaderAttribute normal = ShaderAttribute
    {
        2,
        ShaderAttributeType::float3,
        "aNormal",
        ShaderAttributeSource::normal
    };

    constexpr ShaderAttribute tex_coord = ShaderAttribute
    {
        3,
        ShaderAttributeType::float2,
        "aTexCoord",
        ShaderAttributeSource::uv
    };

    constexpr ShaderAttribute color = ShaderAttribute
    {
        4,
        ShaderAttributeType::float3,
        "aColor",
        ShaderAttributeSource::color
    };


    void add_attributes_to_shader(ShaderProgram* shader);
}

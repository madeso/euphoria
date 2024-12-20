#pragma once

#include "render/shaderattribute.h"

namespace eu::render
{
    struct ShaderProgram;
}

namespace eu::render::attributes2d
{
    constexpr ShaderAttribute vertex = ShaderAttribute
    {
        1,
        ShaderAttributeType::float4,
        "vertex",
        ShaderAttributeSource::unknown
    };

    void add_attributes_to_shader(ShaderProgram* shader);
}

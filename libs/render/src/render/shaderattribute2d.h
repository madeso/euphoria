#pragma once

#include "render/shaderattribute.h"

namespace euphoria::render
{
    struct ShaderProgram;
}

namespace euphoria::render::attributes2d
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

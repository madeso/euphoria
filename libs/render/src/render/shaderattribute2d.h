#pragma once

#include "render/shaderattribute.h"

namespace euphoria::render
{
    struct ShaderProgram;
}

namespace euphoria::render::attributes2d
{
    const ShaderAttribute&
    vertex();

    void
    prebind_shader(ShaderProgram* shader);
}

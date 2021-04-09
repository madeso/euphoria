#pragma once

#include "render/shaderattribute.h"

namespace euphoria::render
{
    struct Shader;
}

namespace euphoria::render::attributes2d
{
    const ShaderAttribute&
    Vertex();

    void
    PrebindShader(Shader* shader);
}

#pragma once

#include "render/shaderattribute.h"

namespace euphoria::render
{
    struct shader;
}

namespace euphoria::render::attributes2d
{
    const shader_attribute&
    vertex();

    void
    prebind_shader(shader* shader);
}

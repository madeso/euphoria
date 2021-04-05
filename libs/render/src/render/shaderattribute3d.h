#pragma once

#include "render/shaderattribute.h"


namespace euphoria::render
{
    struct Shader;
}

// todo(Gustav): Refactor into a pipeline/layout structure
/*
Currently attributes are hardcoded into a special layout.
It would be preferable if the layout is written in script
and the actual values are dynamically created in script.
*/
namespace euphoria::render::attributes3d
{
    const ShaderAttribute&
    Vertex();


    const ShaderAttribute&
    Normal();


    const ShaderAttribute&
    TexCoord();


    const ShaderAttribute&
    Color();


    void
    PrebindShader(Shader* shader);
}

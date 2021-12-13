#pragma once

#include "render/shaderattribute.h"


namespace euphoria::render
{
    struct ShaderProgram;
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
    vertex();


    const ShaderAttribute&
    normal();


    const ShaderAttribute&
    tex_coord();


    const ShaderAttribute&
    color();


    void
    prebind_shader(ShaderProgram* shader);
}

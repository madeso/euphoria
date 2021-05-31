#pragma once

#include "render/shaderattribute.h"


namespace euphoria::render
{
    struct shader;
}

// todo(Gustav): Refactor into a pipeline/layout structure
/*
Currently attributes are hardcoded into a special layout.
It would be preferable if the layout is written in script
and the actual values are dynamically created in script.
*/
namespace euphoria::render::attributes3d
{
    const shader_attribute&
    vertex();


    const shader_attribute&
    normal();


    const shader_attribute&
    tex_coord();


    const shader_attribute&
    color();


    void
    prebind_shader(shader* shader);
}

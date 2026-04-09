#pragma once

#include "eu/render/shader.source.h"
#include "eu/render/texture.h"

namespace eu::render
{

struct Assets
{
    // todo(Gustav): some default assets?
    std::shared_ptr<Texture2d> white;
    std::shared_ptr<Texture2d> black;

    // todo(Gustav): load from file or pakwad
    ShaderSource default_shader_source = { .vertex = "", .fragment = "" };
    ShaderSource skybox_shader_source = { .vertex = "", .fragment = "" };

    std::string pp_vert_glsl;
    std::string pp_realize_frag_glsl;
    std::string pp_extract_frag_glsl;
    std::string pp_ping_pong_blur_frag_glsl;
};

}

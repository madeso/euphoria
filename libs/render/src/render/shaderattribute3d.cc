#include "render/shaderattribute3d.h"

#include "assert/assert.h"

#include "render/shader.h"


namespace euphoria::render::attributes3d
{
    void add_attributes_to_shader(ShaderProgram* shader)
    {
        ASSERT(shader);
        shader->add_attribute(vertex);
        shader->add_attribute(normal);
        shader->add_attribute(tex_coord);
        shader->add_attribute(color);
    }
}

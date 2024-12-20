#include "render/shaderattribute2d.h"

#include "assert/assert.h"

#include "render/shader.h"

namespace eu::render::attributes2d
{
    void add_attributes_to_shader(ShaderProgram* shader)
    {
        ASSERT(shader);
        shader->add_attribute(vertex);
    }
}

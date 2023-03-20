#include "render/shaderattribute2d.h"

#include "assert/assert.h"

#include "render/shader.h"

namespace euphoria::render::attributes2d
{
    const ShaderAttribute&
    vertex()
    {
        static const auto attribute = ShaderAttribute
        {
            1,
            ShaderAttributeType::float4,
            "vertex",
            ShaderAttributeSource::unknown
        };
        return attribute;
    }


    void add_attributes_to_shader(ShaderProgram* shader)
    {
        ASSERT(shader);
        shader->add_attribute(vertex());
    }
}

#include "render/shaderattribute2d.h"

#include "core/assert.h"

#include "render/shader.h"

namespace euphoria::render::attributes2d
{
    const shader_attribute&
    vertex()
    {
        static const auto attribute = shader_attribute
        {
            1,
            shader_attribute_type::float4,
            "vertex",
            shader_attribute_source::unknown
        };
        return attribute;
    }


    void
    prebind_shader(shader* shader)
    {
        ASSERT(shader);
        shader->pre_bind(vertex());
    }
}

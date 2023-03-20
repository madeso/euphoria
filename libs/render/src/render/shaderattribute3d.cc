#include "render/shaderattribute3d.h"

#include "assert/assert.h"

#include "render/shader.h"


namespace euphoria::render::attributes3d
{
    // todo(Gustav): make constexpr
    const ShaderAttribute& vertex()
    {
        static const auto attribute = ShaderAttribute
        {
            1,
            ShaderAttributeType::float3,
            "aPosition",
            ShaderAttributeSource::vertex
        };
        return attribute;
    }


    const ShaderAttribute& normal()
    {
        static const auto attribute = ShaderAttribute
        {
            2,
            ShaderAttributeType::float3,
            "aNormal",
            ShaderAttributeSource::normal
        };
        return attribute;
    }


    const ShaderAttribute& tex_coord()
    {
        static const auto attribute = ShaderAttribute
        {
            3,
            ShaderAttributeType::float2,
            "aTexCoord",
            ShaderAttributeSource::uv
        };
        return attribute;
    }


    const ShaderAttribute& color()
    {
        static const auto attribute = ShaderAttribute
        {
            4,
            ShaderAttributeType::float3,
            "aColor",
            ShaderAttributeSource::color
        };
        return attribute;
    }


    void add_attributes_to_shader(ShaderProgram* shader)
    {
        ASSERT(shader);
        shader->add_attribute(vertex());
        shader->add_attribute(normal());
        shader->add_attribute(tex_coord());
        shader->add_attribute(color());
    }
}

#include "render/shaderattribute3d.h"

#include "core/assert.h"

#include "render/shader.h"


namespace euphoria::render::attributes3d
{
    const shader_attribute&
    vertex()
    {
        static const auto attribute = shader_attribute
        {
            1,
            shader_attribute_type::float3,
            "aPosition",
            shader_attribute_source::vertex
        };
        return attribute;
    }


    const shader_attribute&
    normal()
    {
        static const auto attribute = shader_attribute
        {
            2,
            shader_attribute_type::float3,
            "aNormal",
            shader_attribute_source::normal
        };
        return attribute;
    }


    const shader_attribute&
    tex_coord()
    {
        static const auto attribute = shader_attribute
        {
            3,
            shader_attribute_type::float2,
            "aTexCoord",
            shader_attribute_source::uv
        };
        return attribute;
    }


    const shader_attribute&
    color()
    {
        static const auto attribute = shader_attribute
        {
            4,
            shader_attribute_type::float3,
            "aColor",
            shader_attribute_source::color
        };
        return attribute;
    }


    void
    prebind_shader(shader* shader)
    {
        ASSERT(shader);
        shader->pre_bind(vertex());
        shader->pre_bind(normal());
        shader->pre_bind(tex_coord());
        shader->pre_bind(color());
    }
}

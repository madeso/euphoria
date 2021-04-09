#include "render/shaderattribute3d.h"

#include "core/assert.h"

#include "render/shader.h"


namespace euphoria::render::attributes3d
{
    const ShaderAttribute&
    Vertex()
    {
        static const auto attribute = ShaderAttribute
        {
            1,
            ShaderAttributeType::FLOAT3,
            "aPosition",
            ShaderAttributeSource::Vertex
        };
        return attribute;
    }


    const ShaderAttribute&
    Normal()
    {
        static const auto attribute = ShaderAttribute
        {
            2,
            ShaderAttributeType::FLOAT3,
            "aNormal",
            ShaderAttributeSource::Normal
        };
        return attribute;
    }


    const ShaderAttribute&
    TexCoord()
    {
        static const auto attribute = ShaderAttribute
        {
            3,
            ShaderAttributeType::FLOAT2,
            "aTexCoord",
            ShaderAttributeSource::Uv
        };
        return attribute;
    }


    const ShaderAttribute&
    Color()
    {
        static const auto attribute = ShaderAttribute
        {
            4,
            ShaderAttributeType::FLOAT3,
            "aColor",
            ShaderAttributeSource::Color
        };
        return attribute;
    }


    void
    PrebindShader(Shader* shader)
    {
        ASSERT(shader);
        shader->PreBind(Vertex());
        shader->PreBind(Normal());
        shader->PreBind(TexCoord());
        shader->PreBind(Color());
    }
}

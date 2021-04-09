#include "render/shaderattribute2d.h"

#include "core/assert.h"

#include "render/shader.h"

namespace euphoria::render::attributes2d
{
    const ShaderAttribute&
    Vertex()
    {
        static const auto attribute = ShaderAttribute
        {
            1,
            ShaderAttributeType::FLOAT4,
            "vertex",
            ShaderAttributeSource::Unknown
        };
        return attribute;
    }


    void
    PrebindShader(Shader* shader)
    {
        ASSERT(shader);
        shader->PreBind(Vertex());
    }
}

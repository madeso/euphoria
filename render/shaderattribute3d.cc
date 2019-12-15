#include "render/shaderattribute3d.h"

#include "core/assert.h"

#include "render/shader.h"

namespace euphoria::render
{
    namespace attributes3d
    {
        const ShaderAttribute&
        Vertex()
        {
            static ShaderAttribute Attribute {1,
                                              ShaderAttributeType::FLOAT3,
                                              "aPosition",
                                              ShaderAttributeSource::Vertex};
            return Attribute;
        }


        const ShaderAttribute&
        Normal()
        {
            static ShaderAttribute Attribute {2,
                                              ShaderAttributeType::FLOAT3,
                                              "aNormal",
                                              ShaderAttributeSource::Normal};
            return Attribute;
        }


        const ShaderAttribute&
        TexCoord()
        {
            static ShaderAttribute Attribute {3,
                                              ShaderAttributeType::FLOAT2,
                                              "aTexCoord",
                                              ShaderAttributeSource::Uv};
            return Attribute;
        }


        const ShaderAttribute&
        Color()
        {
            static ShaderAttribute Attribute
            {
                4,
                ShaderAttributeType::FLOAT3,
                "aColor",
                ShaderAttributeSource::Color
            };
            return Attribute;
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
    }  // namespace attributes3d
}  // namespace euphoria::render

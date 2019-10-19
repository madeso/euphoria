#include "render/shaderattribute2d.h"

#include "core/assert.h"

#include "render/shader.h"

namespace euphoria::render
{
    namespace attributes2d
    {
        const ShaderAttribute&
        Vertex()
        {
            static ShaderAttribute Attribute {
                    1, ShaderAttributeType::FLOAT4, "vertex"};
            return Attribute;
        }

        void
        PrebindShader(Shader* shader)
        {
            ASSERT(shader);
            shader->PreBind(Vertex());
        }
    }  // namespace attributes2d

}  // namespace euphoria::render
